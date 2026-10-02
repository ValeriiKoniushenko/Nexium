// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Latency.h"

#include <iosfwd>
#include <thread>
#include <unordered_map>

namespace Foundation::Latency
{

    class Report final
    {
    public:
        Report() = default;
        explicit Report(const std::vector<Sample>& samples);
        explicit Report(const Collector& collector);

        void setSamples(const std::vector<Sample>& samples);
        [[nodiscard]] Duration getDuration() const noexcept { return _duration; }
        [[nodiscard]] std::size_t getPointCount() const noexcept { return _pointCount; }
        [[nodiscard]] std::size_t getThreadCount() const noexcept { return _threadCount; }
        [[nodiscard]] const auto& getThreadSamples() const noexcept { return _perThreadSamples; }

        void fullClear(bool isIgnoreRawSamples = true);

    private:
        std::unordered_map<std::thread::id, std::vector<Sample>> _perThreadSamples;

        Duration _duration{};
        std::size_t _pointCount = 0;
        std::size_t _threadCount = 0;
    };

    class BasePrinter
    {
    public:
        explicit BasePrinter(const Report& report)
            : _report{ &report }
        {
        }
        BasePrinter(const BasePrinter&) = delete;
        BasePrinter(BasePrinter&&) = delete;
        BasePrinter& operator=(const BasePrinter&) = delete;
        BasePrinter& operator=(BasePrinter&&) = delete;
        virtual ~BasePrinter() = default;

        virtual void print() const = 0;

    protected:
        const Report* _report = nullptr;
    };
} // namespace Foundation::Latency
