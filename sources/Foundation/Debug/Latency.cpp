// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Latency.h"

#include <algorithm>
#include <map>
#include <mutex>
#include <set>
#include <utility>

namespace Foundation::Latency
{

#if defined(NEXIUM_DEBUG)
    Collector gCollector{ 1024 };
#else
    Collector gCollector;
#endif

    Collector::Collector(std::size_t expectedSamples)
    {
        _samples.reserve(expectedSamples);
    }

    void Collector::add(std::string_view description, std::source_location source)
    {
        Sample sample{ Clock::now(), source, description };

        const std::unique_lock lock{ _mutex };
        _samples.push_back(sample);
    }

    void Collector::clear()
    {
        const std::unique_lock lock{ _mutex };
        _samples.clear();
    }

    std::vector<Sample> Collector::getSamples() const
    {
        std::vector<Sample> out;

        const std::shared_lock lock{ _mutex };
        out = _samples;
        return out;
    }
} // namespace Foundation::Latency
