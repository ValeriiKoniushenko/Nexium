// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "LatencyTerminalPrinter.h"

#include <algorithm>
#include <chrono>
#include <format>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

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

        struct ThreadHotspot final
        {
            const Sample* from = nullptr;
            const Sample* to = nullptr;
            std::size_t calls = 0;
            Duration total{};
            Duration min{};
            Duration max{};

            [[nodiscard]] Duration average() const noexcept
            {
                return total / static_cast<Duration::rep>(calls);
            }
        };

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

        std::string hotspotName(const ThreadHotspot& hotspot)
        {
            const std::string_view fromMethod = hotspot.from->source.function_name();
            const std::string_view toMethod = hotspot.to->source.function_name();
            if (fromMethod == toMethod)
            {
                return std::format("{} [{} -> {}]", fromMethod, pointName(*hotspot.from),
                                   pointName(*hotspot.to));
            }

            return std::format("{} [{}] -> {} [{}]", fromMethod, pointName(*hotspot.from), toMethod,
                               pointName(*hotspot.to));
        }

        std::vector<ThreadHotspot> buildHotspots(const std::vector<Sample>& samples)
        {
            std::map<Transition, ThreadHotspot, TransitionLess> aggregated;
            for (std::size_t i = 1; i < samples.size(); ++i)
            {
                const auto duration = samples[i].timestamp - samples[i - 1].timestamp;
                const Transition transition{ samples[i - 1].source, samples[i].source };
                auto [it, inserted] = aggregated.try_emplace(
                    transition,
                    ThreadHotspot{ &samples[i - 1], &samples[i], 1, duration, duration, duration });
                if (!inserted)
                {
                    auto& hotspot = it->second;
                    ++hotspot.calls;
                    hotspot.total += duration;
                    hotspot.min = std::min(hotspot.min, duration);
                    hotspot.max = std::max(hotspot.max, duration);
                }
            }

            std::vector<ThreadHotspot> hotspots;
            hotspots.reserve(aggregated.size());
            for (const auto& [transition, hotspot] : aggregated)
            {
                hotspots.push_back(hotspot);
            }
            std::ranges::sort(hotspots, [](const ThreadHotspot& lhs, const ThreadHotspot& rhs)
                              { return lhs.total > rhs.total; });
            return hotspots;
        }
    } // namespace

    void TerminalPrinter::print() const
    {
        print(std::cout);
    }

    void TerminalPrinter::print(std::ostream& output) const
    {
        const auto& perThreadSamples = _report->getThreadSamples();
        std::size_t sampleCount = 0;
        for (const auto& [threadId, samples] : perThreadSamples)
        {
            sampleCount += samples.size();
        }

        if (sampleCount == 0)
        {
            output << "[latency] no samples\n";
            return;
        }

        output << std::format("\nLatency hotspots: {} samples, {} points, {} threads, {}\n",
                              sampleCount, _report->getPointCount(), _report->getThreadCount(),
                              formatDuration(_report->getDuration()));

        using ThreadSamples = std::pair<const std::thread::id, std::vector<Sample>>;
        std::vector<const ThreadSamples*> threads;
        threads.reserve(perThreadSamples.size());
        for (const auto& entry : perThreadSamples)
        {
            threads.push_back(&entry);
        }
        std::ranges::sort(
            threads, [](const ThreadSamples* lhs, const ThreadSamples* rhs)
            { return lhs->second.front().timestamp < rhs->second.front().timestamp; });

        for (std::size_t threadIndex = 0; threadIndex < threads.size(); ++threadIndex)
        {
            const auto hotspots = buildHotspots(threads[threadIndex]->second);
            output << std::format("\nThread T{}\n", threadIndex + 1);
            if (hotspots.empty())
            {
                output << "[latency] no measurable intervals\n";
                continue;
            }

            std::size_t nameWidth = 7;
            for (const auto& hotspot : hotspots)
            {
                nameWidth = std::max(nameWidth, hotspotName(hotspot).size());
            }

            output << std::format("{:<{}}  {:>7}  {:>11}  {:>11}  {:>11}  {:>11}\n", "Hotspot",
                                  nameWidth, "Calls", "Total", "Average", "Min", "Max");
            for (const auto& hotspot : hotspots)
            {
                output << std::format("{:<{}}  {:>7}  {:>11}  {:>11}  {:>11}  {:>11}\n",
                                      hotspotName(hotspot), nameWidth, hotspot.calls,
                                      formatDuration(hotspot.total),
                                      formatDuration(hotspot.average()),
                                      formatDuration(hotspot.min), formatDuration(hotspot.max));
            }
        }
        output << '\n';
    }
} // namespace Foundation::Latency
