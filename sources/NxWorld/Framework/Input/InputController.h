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

#include "NxFundamental/ECS/BaseComponent.h"
#include "NxFundamental/ResourceManagement/JsonAdapter.h"
#include "Platform/Input/Events/InputEvent.h"

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
    enum class ActionValueType : std::uint8_t
    {
        Bool,
        Axis1D,
        Axis2D
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
        NX::ActionValueType type = NX::ActionValueType::Bool;
        FIELD();
        std::vector<NX::InputBinding> bindings;
    };

    void to_json(nlohmann::json& j, const InputAction& v);
    void from_json(const nlohmann::json& j, InputAction& v);

    CLASS();
    class InputController : public NX::BaseComponent
    {
        ECS_DECL(InputController, NX::BaseComponent);

    public:
        bool addAction(InputAction action);
        bool removeAction(const Core::StringAtom& name);

        [[nodiscard]] const InputAction* findAction(const Core::StringAtom& name) const;
        [[nodiscard]] InputAction* findAction(const Core::StringAtom& name);

        [[nodiscard]] const std::vector<InputAction>& getActions() const noexcept
        {
            return _actions;
        }

    private:
        FIELD();
        std::vector<NX::InputAction> _actions;
    };
} // namespace NX

#include "InputController.generated.h" // added by the code generator. Better don't move it.
