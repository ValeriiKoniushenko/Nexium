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
#include <flat_map>
#include <list>
#include <source_location>

namespace Foundation::Latency
{

    using TimeT = std::chrono::time_point<std::chrono::steady_clock>;

    struct Sample
    {
        TimeT timestamp;
        const char* description = nullptr;
    };

    struct SourceLocationLess
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
        void add(std::source_location&& source, const char* description = nullptr);

        [[nodiscard]] const auto& getSamples() const noexcept { return _samples; }

    private:
        std::flat_map<std::source_location, std::list<Sample>, SourceLocationLess> _samples;
    };

    extern Collector gCollector;

} // namespace Foundation::Latency

[[maybe_unused]] inline void NX_LATENCY_POINT(const char* description = nullptr)
{
#if defined(NEXIUM_DEBUG)
    Foundation::Latency::gCollector.add(std::source_location::current(), description);
#endif
}