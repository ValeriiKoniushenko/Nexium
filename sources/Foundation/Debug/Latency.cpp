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
#include <set>
#include <utility>

namespace Foundation::Latency
{

#if defined(NEXIUM_DEBUG)
    Collector gCollector{ 256 };
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
        const std::scoped_lock lock{ _mutex };
        _samples.push_back(sample);
    }

    void Collector::clear()
    {
        const std::scoped_lock lock{ _mutex };
        _samples.clear();
    }

    Report Collector::makeReport() const
    {
        std::vector<Sample> samples;
        {
            const std::scoped_lock lock{ _mutex };
            samples = _samples;
        }
        return Report{ std::move(samples) };
    }
} // namespace Foundation::Latency
