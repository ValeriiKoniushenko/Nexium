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

#include "NxWorld/Framework/Input/InputActionTypes.h"
#include "glm/vec2.hpp"

#include <vector>

namespace NX
{
    struct BindingState
    {
        void beginFrame(const InputBinding& binding);
        void onButton(const InputBinding& binding, InputState state, InputModifier mods);
        void addVector(glm::vec2 delta);
        void setAxis(float normalizedValue);

        [[nodiscard]] glm::vec2 contribution(const InputBinding& binding) const;

        bool isHeld = false;
        bool wasPressedThisFrame = false;
        bool firedThisFrame = false;
        glm::vec2 value{ 0.f };
    };

    struct ActionState
    {
        ActionState() = default;
        explicit ActionState(const InputAction& action);

        [[nodiscard]] bool fits(const InputAction& action) const noexcept;
        void beginFrame(const InputAction& action);
        void endFrame(const InputAction& action);

        std::vector<BindingState> bindings;
        glm::vec2 value{ 0.f };
        bool isActive = false;
        bool wasTriggered = false;
    };
} // namespace NX
