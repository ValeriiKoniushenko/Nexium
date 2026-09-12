// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "FPSCounter.h"

namespace Core
{
    void FPSCounter::reset()
    {
        _start = {};
        _frames = 0;
    }

    void FPSCounter::start()
    {
        reset();
        _start = std::chrono::system_clock::now();
    }

    void FPSCounter::newFrameUpdate()
    {
        ++_frames;
    }

    double FPSCounter::getFPS() const
    {
        const auto now = std::chrono::system_clock::now();
        return static_cast<double>(_frames) / std::chrono::duration<double>(now - _start).count();
    }
} // namespace Core