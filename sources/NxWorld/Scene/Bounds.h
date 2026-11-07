// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::World

#pragma once

#include "Core/Rect.h"

namespace NX
{

    class Bounds
    {
    public:
        Bounds() = default;
        Bounds(const Bounds&) = default;
        Bounds(Bounds&&) noexcept = default;
        Bounds& operator=(const Bounds&) = default;
        Bounds& operator=(Bounds&&) noexcept = default;
        virtual ~Bounds() = default;

        [[nodiscard]] virtual Core::FRect getBounds() const = 0;
    };

} // namespace NX