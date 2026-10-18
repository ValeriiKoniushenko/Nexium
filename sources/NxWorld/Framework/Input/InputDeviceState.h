// Nexium
// Copyright 2018-2026 Valerii Koniushenko
// Licensed under the Apache License, Version 2.0.

#pragma once

#include "NxSubsystems/Input/InputTypes.h"

#include <optional>

namespace NX
{
    class InputDeviceState
    {
    public:
        [[nodiscard]] KeyInputEvent apply(KeyInputEvent event);
        [[nodiscard]] const std::vector<InputButton>& getHeldButtons() const noexcept
        {
            return _heldButtons;
        }
        [[nodiscard]] std::vector<InputButton> getChordKeys() const;
        [[nodiscard]] InputModifier getModifiers() const noexcept;
        void reset();

    private:
        std::vector<InputButton> _heldButtons;
        std::optional<glm::vec2> _pointerPosition;
    };
} // namespace NX
