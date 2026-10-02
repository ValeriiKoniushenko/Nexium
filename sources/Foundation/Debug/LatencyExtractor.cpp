// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "LatencyExtractor.h"

#include "Latency.h"

#include <algorithm>
#include <set>
#include <tuple>

namespace Foundation::Latency
{
    Report::Report(const std::vector<Sample>& samples)
    {
        setSamples(samples);
    }

    Report::Report(const Collector& collector)
        : Report(collector.getSamples())
    {
    }

    void Report::setSamples(const std::vector<Sample>& samples)
    {
        fullClear(false);

        if (samples.empty())
        {
            return;
        }

        auto first = samples.front().timestamp;
        auto last = first;
        std::set<std::tuple<std::string_view, std::string_view, std::uint_least32_t,
                            std::uint_least32_t>>
            points;

        for (const auto& sample : samples)
        {
            _perThreadSamples[sample.threadId].push_back(sample);
            first = std::min(first, sample.timestamp);
            last = std::max(last, sample.timestamp);
            points.emplace(sample.source.file_name(), sample.source.function_name(),
                           sample.source.line(), sample.source.column());
        }

        for (auto& [threadId, threadSamples] : _perThreadSamples)
        {
            std::ranges::stable_sort(threadSamples, {}, &Sample::timestamp);
        }

        _duration = last - first;
        _pointCount = points.size();
        _threadCount = _perThreadSamples.size();
    }

    void Report::fullClear(bool isIgnoreRawSamples)
    {
        if (!isIgnoreRawSamples)
        {
            _perThreadSamples.clear();
        }

        _duration = {};
        _pointCount = {};
        _threadCount = {};
    }

} // namespace Foundation::Latency
