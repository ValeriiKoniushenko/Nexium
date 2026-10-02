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

namespace Foundation::Latency
{
    Report::Report(const std::vector<Sample>& samples)
    {
        setSamples(samples);
    }

    Report::Report(const Collector& collector)
        : Report(gCollector.getSamples())
    {
    }

    void Report::setSamples(const std::vector<Sample>& samples)
    {
        _rawSamples = samples;
    }

    void Report::processData()
    {
        fullClear(true);

        int i = 123;
    }

    void Report::fullClear(bool isIgnoreRawSamples /*  = true */)
    {
        if (!isIgnoreRawSamples)
        {
            _rawSamples.clear();
        }

        _gaps.clear();
        _gapSummaries.clear();
        _duration = {};
        _pointCount = {};
        _threadCount = {};
    }

} // namespace Foundation::Latency
