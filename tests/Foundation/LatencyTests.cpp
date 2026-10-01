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

    TEST(LatencyReportTests, OrdersSamplesAndBuildsGaps)
    {
        const auto start = Clock::now();
        const auto pointA = std::source_location::current();
        const auto pointB = std::source_location::current();
        const auto pointC = std::source_location::current();

        Report report{ std::vector{
            Sample{ start + 8ms, pointC, "C" },
            Sample{ start, pointA, "A" },
            Sample{ start + 3ms, pointB, "B" },
        } };

        ASSERT_EQ(report.getSamples().size(), 3u);
        EXPECT_EQ(report.getSamples()[0].description, "A");
        EXPECT_EQ(report.getSamples()[1].description, "B");
        EXPECT_EQ(report.getSamples()[2].description, "C");

        ASSERT_EQ(report.getGaps().size(), 2u);
        EXPECT_EQ(report.getGaps()[0].duration, 3ms);
        EXPECT_EQ(report.getGaps()[1].duration, 5ms);
        EXPECT_EQ(report.getDuration(), 8ms);
        EXPECT_EQ(report.getPointCount(), 3u);
    }

    TEST(LatencyReportTests, AggregatesRepeatedTransitions)
    {
        const auto start = Clock::now();
        const auto pointA = std::source_location::current();
        const auto pointB = std::source_location::current();

        Report report{ std::vector{
            Sample{ start, pointA, "A" },
            Sample{ start + 2ms, pointB, "B" },
            Sample{ start + 5ms, pointA, "A" },
            Sample{ start + 11ms, pointB, "B" },
        } };

        ASSERT_EQ(report.getGapSummaries().size(), 2u);
        const auto& aToB = report.getGapSummaries()[0];
        EXPECT_EQ(aToB.count, 2u);
        EXPECT_EQ(aToB.min, 2ms);
        EXPECT_EQ(aToB.max, 6ms);
        EXPECT_EQ(aToB.getAverage(), 4ms);
    }

    TEST(LatencyReportTests, CollectorCanProduceIndependentReports)
    {
        Foundation::Latency::Collector collector;
        collector.add("first");

        const auto report = collector.makeReport();
        ASSERT_EQ(report.getSamples().size(), 1u);
        EXPECT_EQ(report.getSamples().front().description, "first");

        collector.clear();
        EXPECT_TRUE(collector.makeReport().empty());
        EXPECT_FALSE(report.empty());
    }

    TEST(LatencyReportTests, DoesNotCreateGapsAcrossThreads)
    {
        const auto start = Clock::now();
        const auto pointA = std::source_location::current();
        const auto pointB = std::source_location::current();
        const auto otherThread = std::thread::id{};
        const auto currentThread = std::this_thread::get_id();

        Report report{ std::vector{
            Sample{ start, pointA, "A", currentThread },
            Sample{ start + 1ms, pointB, "Other thread", otherThread },
            Sample{ start + 3ms, pointB, "B", currentThread },
        } };

        ASSERT_EQ(report.getGaps().size(), 1u);
        EXPECT_EQ(report.getGaps().front().duration, 3ms);
        EXPECT_EQ(report.getThreadCount(), 2u);
    }

    TEST(LatencyTerminalPrinterTests, PrintsTimelineAndGapSummary)
    {
        const auto start = Clock::now();
        const auto pointA = std::source_location::current();
        const auto pointB = std::source_location::current();
        Report report{ std::vector{
            Sample{ start, pointA, "Load assets" },
            Sample{ start + 2ms, pointB, "Create scene" },
        } };
        std::ostringstream output;

        Foundation::Latency::TerminalPrinter{}.print(report, output);

        const auto text = output.str();
        EXPECT_NE(text.find("Latency report"), std::string::npos);
        EXPECT_NE(text.find("Timeline"), std::string::npos);
        EXPECT_NE(text.find("Load assets -> Create scene"), std::string::npos);
        EXPECT_NE(text.find("2.000 ms"), std::string::npos);
    }
} // namespace
