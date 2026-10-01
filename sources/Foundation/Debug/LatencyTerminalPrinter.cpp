// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "LatencyTerminalPrinter.h"

#include "Latency.h"

#include <algorithm>
#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <vector>

namespace Foundation::Latency
{
    namespace
    {
        struct Transition final
        {
            std::thread::id threadId;
            std::source_location from;
            std::source_location to;
        };

        struct TransitionLess final
        {
            bool operator()(const Transition& lhs, const Transition& rhs) const noexcept
            {
                if (lhs.threadId != rhs.threadId)
                {
                    return lhs.threadId < rhs.threadId;
                }

                SourceLocationLess less;
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

        constexpr std::size_t timelineWidth = 48;

        std::string formatDuration(Duration duration)
        {
            const auto nanoseconds = std::chrono::duration<double, std::nano>{ duration }.count();
            if (nanoseconds < 1'000.0)
            {
                return std::format("{:.0f} ns", nanoseconds);
            }
            if (nanoseconds < 1'000'000.0)
            {
                return std::format("{:.1f} us", nanoseconds / 1'000.0);
            }
            if (nanoseconds < 1'000'000'000.0)
            {
                return std::format("{:.3f} ms", nanoseconds / 1'000'000.0);
            }
            return std::format("{:.3f} s", nanoseconds / 1'000'000'000.0);
        }

        std::string pointName(const Sample& sample)
        {
            if (!sample.description.empty())
            {
                return std::string{ sample.description };
            }

            std::string_view file = sample.source.file_name();
            if (const auto separator = file.find_last_of("/\\");
                separator != std::string_view::npos)
            {
                file.remove_prefix(separator + 1);
            }
            return std::format("{}:{}", file, sample.source.line());
        }

        std::string makeLane(Duration from, Duration to, Duration total)
        {
            std::string lane(timelineWidth, '.');
            if (total == Duration{})
            {
                lane.front() = '#';
                return lane;
            }

            const auto column = [total](Duration offset)
            {
                const auto ratio = std::chrono::duration<double>{ offset }.count()
                                   / std::chrono::duration<double>{ total }.count();
                return std::min(timelineWidth - 1,
                                static_cast<std::size_t>(ratio * (timelineWidth - 1)));
            };
            const auto first = column(from);
            const auto last = column(to);
            std::fill(lane.begin() + static_cast<std::ptrdiff_t>(first),
                      lane.begin() + static_cast<std::ptrdiff_t>(last + 1), '=');
            lane[last] = first == last ? '#' : '>';
            return lane;
        }

        std::size_t threadIndex(const std::vector<std::thread::id>& threads,
                                std::thread::id threadId)
        {
            return static_cast<std::size_t>(std::ranges::find(threads, threadId) - threads.begin())
                   + 1;
        }
    } // namespace

    void TerminalPrinter::print(const Report& report) const
    {
        print(report, std::cout);
    }

    void TerminalPrinter::print(const Report& report, std::ostream& output) const
    {
        if (report.empty())
        {
            output << "[latency] no samples\n";
            return;
        }

        const auto& samples = report.getSamples();
        const auto start = samples.front().timestamp;
        std::size_t nameWidth = 5;
        for (const auto& sample : samples)
        {
            nameWidth = std::max(nameWidth, pointName(sample).size());
        }

        output << std::format("\nLatency report: {} samples, {} points, {} threads, {}\n\n",
                              samples.size(), report.getPointCount(), report.getThreadCount(),
                              formatDuration(report.getDuration()));
        output << "Timeline\n";
        output << std::format("{:>3}  {:>6}  {:<{}}  {:>11}  {:>11}  {}\n", "#", "Thread", "Point",
                              nameWidth, "At", "Gap", "Timeline");

        std::vector<std::thread::id> threads;
        threads.reserve(report.getThreadCount());
        std::size_t gapIndex = 0;
        for (std::size_t i = 0; i < samples.size(); ++i)
        {
            if (std::ranges::find(threads, samples[i].threadId) == threads.end())
            {
                threads.push_back(samples[i].threadId);
            }

            const auto offset = samples[i].timestamp - start;
            const Gap* gap = nullptr;
            if (gapIndex < report.getGaps().size() && report.getGaps()[gapIndex].toSample == i)
            {
                gap = &report.getGaps()[gapIndex++];
            }
            const auto gapStart = gap ? samples[gap->fromSample].timestamp - start : offset;
            output << std::format("{:>3}  T{:>5}  {:<{}}  {:>11}  {:>11}  {}\n", i + 1,
                                  threadIndex(threads, samples[i].threadId), pointName(samples[i]),
                                  nameWidth, formatDuration(offset),
                                  gap ? formatDuration(gap->duration) : "-",
                                  makeLane(gapStart, offset, report.getDuration()));
        }

        const auto& summaries = report.getGapSummaries();
        if (summaries.empty())
        {
            output << '\n';
            return;
        }

        std::size_t transitionWidth = 10;
        for (const auto& summary : summaries)
        {
            transitionWidth
                = std::max(transitionWidth, pointName(samples[summary.fromSample]).size() + 4
                                                + pointName(samples[summary.toSample]).size());
        }

        output << "\nGap summary\n";
        output << std::format("{:>6}  {:<{}}  {:>6}  {:>11}  {:>11}  {:>11}\n", "Thread",
                              "Transition", transitionWidth, "Count", "Average", "Min", "Max");
        for (const auto& summary : summaries)
        {
            const auto transition = std::format("{} -> {}", pointName(samples[summary.fromSample]),
                                                pointName(samples[summary.toSample]));
            output << std::format("T{:>5}  {:<{}}  {:>6}  {:>11}  {:>11}  {:>11}\n",
                                  threadIndex(threads, samples[summary.fromSample].threadId),
                                  transition, transitionWidth, summary.count,
                                  formatDuration(summary.getAverage()), formatDuration(summary.min),
                                  formatDuration(summary.max));
        }
        output << '\n';
    }

    Report::Report(std::vector<Sample> samples)
        : _samples(std::move(samples))
    {
        std::ranges::stable_sort(_samples, {}, &Sample::timestamp);

        std::set<std::source_location, SourceLocationLess> points;
        std::set<std::thread::id> threads;
        for (const auto& sample : _samples)
        {
            points.emplace(sample.source);
            threads.emplace(sample.threadId);
        }
        _pointCount = points.size();
        _threadCount = threads.size();

        if (_samples.size() < 2)
        {
            return;
        }

        _duration = _samples.back().timestamp - _samples.front().timestamp;
        _gaps.reserve(_samples.size() - _threadCount);
        _gapSummaries.reserve(_samples.size() - 1);
        std::map<std::thread::id, std::size_t> lastSampleByThread;
        std::map<Transition, std::size_t, TransitionLess> summaryIndices;

        for (std::size_t i = 0; i < _samples.size(); ++i)
        {
            const auto lastIt = lastSampleByThread.find(_samples[i].threadId);
            if (lastIt == lastSampleByThread.end())
            {
                lastSampleByThread.emplace(_samples[i].threadId, i);
                continue;
            }

            const auto previous = lastIt->second;
            lastIt->second = i;
            const auto duration = _samples[i].timestamp - _samples[previous].timestamp;
            _gaps.push_back({ previous, i, duration });

            const Transition transition{ _samples[i].threadId, _samples[previous].source,
                                         _samples[i].source };
            const auto [it, inserted]
                = summaryIndices.try_emplace(transition, _gapSummaries.size());
            if (inserted)
            {
                _gapSummaries.push_back({ previous, i, 1, duration, duration, duration });
                continue;
            }

            auto& summary = _gapSummaries[it->second];
            ++summary.count;
            summary.total += duration;
            summary.min = std::min(summary.min, duration);
            summary.max = std::max(summary.max, duration);
        }
    }
} // namespace Foundation::Latency
