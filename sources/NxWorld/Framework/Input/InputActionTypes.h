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

#include "Core/String.h"
#include "JustReflectMe/Adapter.h"
#include "NxFundamental/ResourceManagement/JsonAdapter.h"
#include "Platform/Input/Events/InputEvent.h"
#include "glm/vec2.hpp"

#include <cstdint>
#include <vector>

namespace NX
{
    ENUM_CLASS();
    enum class InputSource : std::uint8_t
    {
        Key,
        MouseButton,
        MouseDelta,
        MouseScroll,
        GamepadButton,
        GamepadAxis
    };

    ENUM_CLASS();
    enum class InputTrigger : std::uint8_t
    {
        Down,
        Pressed,
        Released
    };

    CLASS();
    struct InputBinding
    {
        R_FRIEND(InputBinding);

        FIELD();
        NX::InputSource source = NX::InputSource::Key;
        FIELD();
        std::uint16_t code = 0;
        FIELD();
        NX::InputTrigger trigger = NX::InputTrigger::Down;
        FIELD();
        bool shift = false;
        FIELD();
        bool control = false;
        FIELD();
        bool alt = false;
        FIELD();
        bool super = false;
        FIELD();
        bool negate = false;
        FIELD();
        bool swizzleYX = false;

        [[nodiscard]] InputModifier getModifiers() const noexcept;
        [[nodiscard]] bool isButton() const noexcept;
        [[nodiscard]] bool isPerFrame() const noexcept;
        [[nodiscard]] bool matches(InputSource eventSource, std::uint16_t eventCode) const noexcept;
        [[nodiscard]] bool acceptsModifiers(InputModifier mods) const noexcept;
        [[nodiscard]] glm::vec2 applyValueModifiers(glm::vec2 value) const noexcept;
    };

    void to_json(nlohmann::json& j, const InputBinding& v);
    void from_json(const nlohmann::json& j, InputBinding& v);

    CLASS();
    struct InputAction
    {
        R_FRIEND(InputAction);

        FIELD();
        Core::StringAtom name;
        FIELD();
        std::vector<NX::InputBinding> bindings;
    };

    void to_json(nlohmann::json& j, const InputAction& v);
    void from_json(const nlohmann::json& j, InputAction& v);
} // namespace NX

#include "InputActionTypes.generated.h" // added by the code generator. Better don't move it.
