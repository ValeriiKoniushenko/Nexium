// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Latency.h"

#include <cstdint>
#include <iosfwd>
#include <optional>
#include <thread>
#include <unordered_map>

namespace Foundation::Latency
{

    struct Hotspot final
    {
        std::source_location fromSource;
        std::source_location toSource;
        std::string_view fromDescription;
        std::string_view toDescription;
        std::size_t calls = 0;
        Duration total{};
        Duration min{};
        Duration max{};

        [[nodiscard]] Duration getAverage() const noexcept
        {
            return calls ? total / static_cast<Duration::rep>(calls) : Duration{};
        }
    };

    enum class HotspotComparisonState
    {
        Improved,
        Unchanged,
        Regressed,
        Added,
        Removed,
    };

    struct HotspotComparison final
    {
        std::optional<Hotspot> baseline;
        std::optional<Hotspot> current;
        HotspotComparisonState state = HotspotComparisonState::Unchanged;

        [[nodiscard]] Duration getAverageDelta() const noexcept;
        [[nodiscard]] Duration getTotalDelta() const noexcept;
        [[nodiscard]] std::int64_t getCallDelta() const noexcept;
        [[nodiscard]] double getAverageChangePercent() const noexcept;
    };

    class Report final
    {
    public:
        Report() = default;
        explicit Report(const std::vector<Sample>& samples);
        explicit Report(const Collector& collector);

        void setSamples(const std::vector<Sample>& samples);
        [[nodiscard]] Duration getDuration() const noexcept { return _duration; }
        [[nodiscard]] std::size_t getPointCount() const noexcept { return _pointCount; }
        [[nodiscard]] std::size_t getThreadCount() const noexcept { return _threadCount; }
        [[nodiscard]] const auto& getThreadSamples() const noexcept { return _perThreadSamples; }
        [[nodiscard]] const std::vector<Hotspot>& getHotspots() const noexcept { return _hotspots; }

        [[nodiscard]] std::vector<HotspotComparison> compareTo(const Report& baseline) const;
        [[nodiscard]] bool hasImprovementsComparedTo(const Report& baseline) const;
        [[nodiscard]] bool hasRegressionsComparedTo(const Report& baseline) const;

        void fullClear(bool isIgnoreRawSamples = true);

    private:
        std::unordered_map<std::thread::id, std::vector<Sample>> _perThreadSamples;
        std::vector<Hotspot> _hotspots;

        Duration _duration{};
        std::size_t _pointCount = 0;
        std::size_t _threadCount = 0;
    };

    class BasePrinter
    {
    public:
        explicit BasePrinter(const Report& report)
            : _report{ &report }
        {
        }
        BasePrinter(const BasePrinter&) = delete;
        BasePrinter(BasePrinter&&) = delete;
        BasePrinter& operator=(const BasePrinter&) = delete;
        BasePrinter& operator=(BasePrinter&&) = delete;
        virtual ~BasePrinter() = default;

        virtual void print() const = 0;

    protected:
        const Report* _report = nullptr;
    };
} // namespace Foundation::Latency
