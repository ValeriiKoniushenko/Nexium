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
        explicit Report(std::vector<Sample> samples);

        [[nodiscard]] bool empty() const noexcept { return _samples.empty(); }
        [[nodiscard]] const std::vector<Sample>& getSamples() const noexcept { return _samples; }
        [[nodiscard]] const std::vector<Gap>& getGaps() const noexcept { return _gaps; }
        [[nodiscard]] const std::vector<GapSummary>& getGapSummaries() const noexcept
        {
            return _gapSummaries;
        }
        [[nodiscard]] Duration getDuration() const noexcept { return _duration; }
        [[nodiscard]] std::size_t getPointCount() const noexcept { return _pointCount; }
        [[nodiscard]] std::size_t getThreadCount() const noexcept { return _threadCount; }

    private:
        std::vector<Sample> _samples;
        std::vector<Gap> _gaps;
        std::vector<GapSummary> _gapSummaries;
        Duration _duration{};
        std::size_t _pointCount = 0;
        std::size_t _threadCount = 0;
    };

    class TerminalPrinter final
    {
    public:
        void print(const Report& report) const;
        void print(const Report& report, std::ostream& output) const;
    };
} // namespace Foundation::Latency
