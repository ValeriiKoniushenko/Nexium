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

#include <iosfwd>

namespace Foundation::Latency
{

    struct Gap final
    {
        std::size_t fromSample = 0;
        std::size_t toSample = 0;
        Duration duration{};
    };

    struct GapSummary final
    {
        std::size_t fromSample = 0;
        std::size_t toSample = 0;
        std::size_t count = 0;
        Duration total{};
        Duration min{};
        Duration max{};

        [[nodiscard]] Duration getAverage() const noexcept
        {
            return count ? total / static_cast<Duration::rep>(count) : Duration{};
        }
    };

    class Report final
    {
    public:
        Report() = default;
        explicit Report(const std::vector<Sample>& samples);
        explicit Report(const Collector& collector);

        [[nodiscard]] bool empty() const noexcept { return _rawSamples.empty(); }
        [[nodiscard]] const std::vector<Sample>& getSamples() const noexcept { return _rawSamples; }
        void setSamples(const std::vector<Sample>& samples);
        [[nodiscard]] const std::vector<Gap>& getGaps() const noexcept { return _gaps; }
        [[nodiscard]] const std::vector<GapSummary>& getGapSummaries() const noexcept
        {
            return _gapSummaries;
        }
        [[nodiscard]] Duration getDuration() const noexcept { return _duration; }
        [[nodiscard]] std::size_t getPointCount() const noexcept { return _pointCount; }
        [[nodiscard]] std::size_t getThreadCount() const noexcept { return _threadCount; }

        void processData();
        void fullClear(bool isIgnoreRawSamples = true);

    private:
        bool _isProcessedData = false;

        std::vector<Sample> _rawSamples;

        std::vector<Gap> _gaps;
        std::vector<GapSummary> _gapSummaries;
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
