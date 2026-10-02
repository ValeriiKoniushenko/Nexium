// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "LatencyExtractor.h"

#include <iosfwd>

namespace Foundation::Latency
{

    class TerminalPrinter final : public BasePrinter
    {
    public:
        explicit TerminalPrinter(const Report& report)
            : BasePrinter{ report }
        {
        }

        void print() const override;
    };

} // namespace Foundation::Latency
