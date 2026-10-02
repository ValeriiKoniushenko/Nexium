// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Foundation/Debug/Latency.h"
#include "Foundation/Debug/LatencyTerminalPrinter.h"

#include "gtest/gtest.h"
#include <chrono>
#include <sstream>
#include <vector>

namespace
{
    using namespace std::chrono_literals;
    using Foundation::Latency::Clock;
    using Foundation::Latency::Report;
    using Foundation::Latency::Sample;

    TEST(LatencyReportTests, ReplacesSamplesAndCalculatesOverview)
    {
        const auto start = Clock::now();
        const auto pointA = std::source_location::current();
        const auto pointB = std::source_location::current();
        Report report{ std::vector{ Sample{ start - 100ms, pointA, "old" } } };

        report.setSamples(std::vector{
            Sample{ start + 3ms, pointB, "end" },
            Sample{ start, pointA, "start" },
        });

        EXPECT_EQ(report.getDuration(), 3ms);
        EXPECT_EQ(report.getPointCount(), 2u);
        EXPECT_EQ(report.getThreadCount(), 1u);
    }

    TEST(LatencyTerminalPrinterTests, PrintsAggregatedHotspots)
    {
        const auto start = Clock::now();
        const auto loopStart = std::source_location::current();
        const auto loopEnd = std::source_location::current();
        Report report{ std::vector{
            Sample{ start + 11ms, loopEnd, "loop - end" },
            Sample{ start, loopStart, "loop - start" },
            Sample{ start + 4ms, loopEnd, "loop - end" },
            Sample{ start + 5ms, loopStart, "loop - start" },
        } };
        std::ostringstream output;

        Foundation::Latency::TerminalPrinter{ report }.print(output);

        const auto text = output.str();
        EXPECT_NE(text.find("Latency hotspots"), std::string::npos);
        EXPECT_NE(text.find(loopStart.function_name()), std::string::npos);
        EXPECT_NE(text.find("loop - start -> loop - end"), std::string::npos);
        EXPECT_NE(text.find("10.000 ms"), std::string::npos);
        EXPECT_NE(text.find("5.000 ms"), std::string::npos);
        EXPECT_NE(text.find("4.000 ms"), std::string::npos);
        EXPECT_NE(text.find("6.000 ms"), std::string::npos);
    }
} // namespace
