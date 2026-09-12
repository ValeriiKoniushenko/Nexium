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
#include <cstdint>

namespace Core
{
    class FPSCounter
    {
    public:
        void reset();

        void start();

        void newFrameUpdate();

        [[nodiscard]] double getFPS() const;

    private:
        std::chrono::system_clock::time_point _start;
        uint64_t _frames = 0;
    };
} // namespace Core
