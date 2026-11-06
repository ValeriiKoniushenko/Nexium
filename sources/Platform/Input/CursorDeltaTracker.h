// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Platform::Input

#pragma once

#include "glm/glm.hpp"

namespace Platform
{
    class CursorDeltaTracker
    {
    public:
        [[nodiscard]] glm::vec2 update(glm::vec2 position) noexcept
        {
            const glm::vec2 delta = _hasLastPosition ? position - _lastPosition : glm::vec2{ 0.f };
            _lastPosition = position;
            _hasLastPosition = true;
            return delta;
        }

        void reset() noexcept { _hasLastPosition = false; }

        void resetTo(glm::vec2 position) noexcept
        {
            _lastPosition = position;
            _hasLastPosition = true;
        }

    private:
        glm::vec2 _lastPosition{ 0.f };
        bool _hasLastPosition = false;
    };
} // namespace Platform
