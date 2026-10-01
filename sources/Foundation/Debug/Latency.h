// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <chrono>
#include <cstddef>
#include <mutex>
#include <source_location>
#include <string_view>
#include <thread>
#include <tuple>
#include <vector>

namespace Foundation::Latency
{
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = Clock::duration;

    struct Sample final
    {
        TimePoint timestamp;
        std::source_location source;
        std::string_view description;
        std::thread::id threadId = std::this_thread::get_id();
    };

    struct SourceLocationLess final
    {
        bool operator()(const std::source_location& lhs,
                        const std::source_location& rhs) const noexcept
        {
            return std::tuple{ std::string_view{ lhs.file_name() },
                               std::string_view{ lhs.function_name() }, lhs.line(), lhs.column() }
                   < std::tuple{ std::string_view{ rhs.file_name() },
                                 std::string_view{ rhs.function_name() }, rhs.line(),
                                 rhs.column() };
        }
    };

    class Collector final
    {
    public:
        explicit Collector(std::size_t expectedSamples = 0);

        void add(std::string_view description = {},
                 std::source_location source = std::source_location::current());
        void clear();

    private:
        mutable std::mutex _mutex;
        std::vector<Sample> _samples;
    };

    extern Collector gCollector;
} // namespace Foundation::Latency

#if defined(NEXIUM_DEBUG)
    #define NX_LATENCY_POINT(description)                                                          \
        do                                                                                         \
        {                                                                                          \
            static constexpr std::string_view nxLatencyDescription{ description };                 \
            Foundation::Latency::gCollector.add(nxLatencyDescription,                              \
                                                std::source_location::current());                  \
        } while (false)
#else
    #define NX_LATENCY_POINT(description)                                                          \
        do                                                                                         \
        {                                                                                          \
        } while (false)
#endif
