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
#include <limits>
#include <map>
#include <set>
#include <tuple>
#include <utility>

namespace Foundation::Latency
{
    namespace
    {
        struct SourceLocationLess final
        {
            bool operator()(const std::source_location& lhs,
                            const std::source_location& rhs) const noexcept
            {
                return std::tuple{
                    std::string_view{ lhs.file_name() }, std::string_view{ lhs.function_name() },
                    lhs.line(), lhs.column()
                } < std::tuple{ std::string_view{ rhs.file_name() },
                                std::string_view{ rhs.function_name() }, rhs.line(), rhs.column() };
            }
        };

        struct Transition final
        {
            std::source_location from;
            std::source_location to;
        };

        struct TransitionLess final
        {
            bool operator()(const Transition& lhs, const Transition& rhs) const noexcept
            {
                const SourceLocationLess less;
                if (less(lhs.from, rhs.from))
                {
                    return true;
                }
                if (less(rhs.from, lhs.from))
                {
                    return false;
                }
                return less(lhs.to, rhs.to);
            }
        };

        Transition transitionOf(const Hotspot& hotspot)
        {
            return { hotspot.fromSource, hotspot.toSource };
        }

        std::vector<Hotspot> buildHotspots(
            const std::unordered_map<std::thread::id, std::vector<Sample>>& perThreadSamples)
        {
            std::map<Transition, Hotspot, TransitionLess> aggregated;
            for (const auto& [threadId, samples] : perThreadSamples)
            {
                for (std::size_t i = 1; i < samples.size(); ++i)
                {
                    const auto duration = samples[i].timestamp - samples[i - 1].timestamp;
                    const Transition transition{ samples[i - 1].source, samples[i].source };
                    auto [it, inserted] = aggregated.try_emplace(
                        transition, Hotspot{ samples[i - 1].source, samples[i].source,
                                             samples[i - 1].description, samples[i].description, 1,
                                             duration, duration, duration });
                    if (!inserted)
                    {
                        auto& hotspot = it->second;
                        ++hotspot.calls;
                        hotspot.total += duration;
                        hotspot.min = std::min(hotspot.min, duration);
                        hotspot.max = std::max(hotspot.max, duration);
                    }
                }
            }

            std::vector<Hotspot> hotspots;
            hotspots.reserve(aggregated.size());
            for (auto& [transition, hotspot] : aggregated)
            {
                hotspots.push_back(std::move(hotspot));
            }
            std::ranges::sort(hotspots, [](const Hotspot& lhs, const Hotspot& rhs)
                              { return lhs.total > rhs.total; });
            return hotspots;
        }

        HotspotComparison makeComparison(const Hotspot* baseline, const Hotspot* current)
        {
            HotspotComparison comparison;
            if (baseline)
            {
                comparison.baseline = *baseline;
            }
            if (current)
            {
                comparison.current = *current;
            }

            if (!baseline)
            {
                comparison.state = HotspotComparisonState::Added;
            }
            else if (!current)
            {
                comparison.state = HotspotComparisonState::Removed;
            }
            else if (current->getAverage() < baseline->getAverage())
            {
                comparison.state = HotspotComparisonState::Improved;
            }
            else if (current->getAverage() > baseline->getAverage())
            {
                comparison.state = HotspotComparisonState::Regressed;
            }

            return comparison;
        }
    } // namespace

    Duration HotspotComparison::getAverageDelta() const noexcept
    {
        const auto baselineAverage = baseline ? baseline->getAverage() : Duration{};
        const auto currentAverage = current ? current->getAverage() : Duration{};
        return currentAverage - baselineAverage;
    }

    Duration HotspotComparison::getTotalDelta() const noexcept
    {
        const auto baselineTotal = baseline ? baseline->total : Duration{};
        const auto currentTotal = current ? current->total : Duration{};
        return currentTotal - baselineTotal;
    }

    std::int64_t HotspotComparison::getCallDelta() const noexcept
    {
        const auto baselineCalls = baseline ? static_cast<std::int64_t>(baseline->calls) : 0;
        const auto currentCalls = current ? static_cast<std::int64_t>(current->calls) : 0;
        return currentCalls - baselineCalls;
    }

    double HotspotComparison::getAverageChangePercent() const noexcept
    {
        const auto baselineAverage = baseline ? baseline->getAverage() : Duration{};
        const auto currentAverage = current ? current->getAverage() : Duration{};
        if (baselineAverage == Duration{})
        {
            return currentAverage == Duration{} ? 0.0 : std::numeric_limits<double>::infinity();
        }

        return std::chrono::duration<double>{ currentAverage - baselineAverage }.count()
               / std::chrono::duration<double>{ baselineAverage }.count() * 100.0;
    }

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

        _hotspots = buildHotspots(_perThreadSamples);
        _duration = last - first;
        _pointCount = points.size();
        _threadCount = _perThreadSamples.size();
    }

    std::vector<HotspotComparison> Report::compareTo(const Report& baseline) const
    {
        std::map<Transition, const Hotspot*, TransitionLess> baselineHotspots;
        std::map<Transition, const Hotspot*, TransitionLess> currentHotspots;
        for (const auto& hotspot : baseline._hotspots)
        {
            baselineHotspots.emplace(transitionOf(hotspot), &hotspot);
        }
        for (const auto& hotspot : _hotspots)
        {
            currentHotspots.emplace(transitionOf(hotspot), &hotspot);
        }

        std::vector<HotspotComparison> comparisons;
        comparisons.reserve(baselineHotspots.size() + currentHotspots.size());
        for (const auto& [transition, baselineHotspot] : baselineHotspots)
        {
            const auto current = currentHotspots.find(transition);
            comparisons.push_back(makeComparison(
                baselineHotspot, current == currentHotspots.end() ? nullptr : current->second));
        }
        for (const auto& [transition, currentHotspot] : currentHotspots)
        {
            if (!baselineHotspots.contains(transition))
            {
                comparisons.push_back(makeComparison(nullptr, currentHotspot));
            }
        }
        return comparisons;
    }

    bool Report::hasImprovementsComparedTo(const Report& baseline) const
    {
        const auto comparisons = compareTo(baseline);
        return std::ranges::any_of(
            comparisons, [](const HotspotComparison& comparison)
            { return comparison.state == HotspotComparisonState::Improved; });
    }

    bool Report::hasRegressionsComparedTo(const Report& baseline) const
    {
        const auto comparisons = compareTo(baseline);
        return std::ranges::any_of(
            comparisons, [](const HotspotComparison& comparison)
            { return comparison.state == HotspotComparisonState::Regressed; });
    }

    void Report::fullClear(bool isIgnoreRawSamples)
    {
        if (!isIgnoreRawSamples)
        {
            _perThreadSamples.clear();
        }

        _hotspots.clear();
        _duration = {};
        _pointCount = {};
        _threadCount = {};
    }

} // namespace Foundation::Latency
