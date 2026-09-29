// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Latency.h"

namespace Foundation::Latency
{
    Collector gCollector; // extern's impl

    void Collector::add(std::source_location&& source, const char* description /*  = nullptr */)
    {
        Sample sample;
        sample.timestamp = std::chrono::steady_clock::now();
        sample.description = description;

        if (auto i = _samples.find(source); i != _samples.end())
        {
            i->second.emplace_back(std::move(sample));
        }
        else
        {
            _samples.emplace(source, std::list{ std::move(sample) });
        }
    }

} // namespace Foundation::Latency