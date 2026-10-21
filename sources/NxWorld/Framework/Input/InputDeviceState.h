// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "NxSubsystems/Input/InputTypes.h"

#include <optional>

namespace NX
{
    class InputDeviceState
    {
    public:
        [[nodiscard]] KeyInputEvent apply(KeyInputEvent event);
        [[nodiscard]] const std::vector<InputButton>& getHeldButtons() const noexcept;
        [[nodiscard]] std::vector<InputButton> getChordKeys() const;
        [[nodiscard]] std::uint8_t getModifiers() const noexcept;
        void reset();

    private:
        std::vector<InputButton> _heldButtons;
        std::optional<glm::vec2> _pointerPosition;
    };
} // namespace NX
