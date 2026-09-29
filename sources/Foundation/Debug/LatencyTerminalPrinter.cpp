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
        using Ms = std::chrono::duration<double, std::milli>;

        struct Row
        {
            std::string name;
            std::size_t hits = 0;
            TimeT first;
            TimeT last;
            double avg = 0, min = 0, max = 0; // ms, period between consecutive hits
        };

        std::string fmt(double ms)
        {
            if (ms < 1.0)
            {
                return std::format("{:.1f} us", ms * 1000.0);
            }
            if (ms < 1000.0)
            {
                return std::format("{:.3f} ms", ms);
            }
            return std::format("{:.3f} s", ms / 1000.0);
        }

        // Bar covering [from, to] (ms from start) inside a lane of `width` columns.
        std::string lane(double from, double to, double total, std::size_t width)
        {
            auto col = [&](double t)
            { return std::min(width - 1, static_cast<std::size_t>(t / total * width)); };
            const auto a = col(from), b = col(to);

            std::string s;
            for (std::size_t i = 0; i < width; ++i)
            {
                s += (i >= a && i <= b) ? "█" : "·";
            }
            return s;
        }
    } // namespace

    void TerminalPrinter::print()
    {
        auto&& samples = gCollector.getSamples();

        // 1. One row per point
        std::vector<Row> rows;
        for (auto&& [loc, list] : samples)
        {
            if (list.empty())
            {
                continue;
            }

            Row r;
            r.hits = list.size();
            r.first = list.front().timestamp;
            r.last = list.back().timestamp;

            if (list.front().description)
            {
                r.name = list.front().description;
            }
            else
            {
                std::string_view file = loc.file_name();
                file = file.substr(file.find_last_of("/\\") + 1);
                r.name = std::format("{}:{}", file, loc.line());
            }

            if (r.hits > 1)
            {
                double sum = 0;
                r.min = 1e300;
                auto prev = list.front().timestamp;
                for (auto it = std::next(list.begin()); it != list.end(); ++it)
                {
                    const double d = Ms{ it->timestamp - prev }.count();
                    prev = it->timestamp;
                    sum += d;
                    r.min = std::min(r.min, d);
                    r.max = std::max(r.max, d);
                }
                r.avg = sum / double(r.hits - 1);
            }
            rows.push_back(std::move(r));
        }

        if (rows.empty())
        {
            std::cout << "[latency] no samples\n";
            return;
        }

        // 2. Chronological order (the map is ordered by source location, not by time)
        std::ranges::sort(rows, {}, &Row::first);

        const auto t0 = rows.front().first;
        auto tEnd = t0;
        std::size_t nameW = 5;
        for (auto& r : rows)
        {
            tEnd = std::max(tEnd, r.last);
            nameW = std::max(nameW, r.name.size());
        }
        const double total = std::max(Ms{ tEnd - t0 }.count(), 1e-6);
        constexpr std::size_t laneW = 60;

        // 3. Print
        std::cout << std::format("\nTotal: {}   Points: {}\n\n", fmt(total), rows.size());
        std::cout << std::format(
            "{:>2}  {:<{}}  {:>6}  {:>11}  {:>11}  {:>11}  {:>11}  {:>11}  {}\n", "#", "Point",
            nameW, "Hits", "Start", "Δ prev", "Avg", "Min", "Max", "Timeline");

        for (std::size_t i = 0; i < rows.size(); ++i)
        {
            const Row& r = rows[i];
            const double start = Ms{ r.first - t0 }.count();
            const double end = Ms{ r.last - t0 }.count();
            const double delta = i ? Ms{ r.first - rows[i - 1].first }.count() : 0.0;

            // Single hit  -> bar = time since the previous point (the "phase" you waited).
            // Many hits   -> bar = span from the first to the last hit.
            const double from = (r.hits == 1 && i) ? start - delta : start;

            const bool rep = r.hits > 1;
            std::cout << std::format(
                "{:>2}  {:<{}}  {:>6}  {:>11}  {:>11}  {:>11}  {:>11}  {:>11}  {}\n", i + 1, r.name,
                nameW, r.hits, fmt(start), i ? fmt(delta) : "-", rep ? fmt(r.avg) : "-",
                rep ? fmt(r.min) : "-", rep ? fmt(r.max) : "-", lane(from, end, total, laneW));
        }
        std::cout << '\n';
    }

} // namespace Foundation::Latency