// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/Input/InputActionTypes.h"

namespace
{
    constexpr auto MatchedModifiers = NX::InputModifier::Shift | NX::InputModifier::Control
                                      | NX::InputModifier::Alt | NX::InputModifier::Super;
} // namespace

namespace NX
{
    InputModifier InputBinding::getModifiers() const noexcept
    {
        auto result = InputModifier::None;
        if (shift)
        {
            result = result | InputModifier::Shift;
        }
        if (control)
        {
            result = result | InputModifier::Control;
        }
        if (alt)
        {
            result = result | InputModifier::Alt;
        }
        if (super)
        {
            result = result | InputModifier::Super;
        }
        return result;
    }

    bool InputBinding::isButton() const noexcept
    {
        return source == InputSource::Key || source == InputSource::MouseButton
               || source == InputSource::GamepadButton;
    }

    bool InputBinding::isPerFrame() const noexcept
    {
        return source == InputSource::MouseDelta || source == InputSource::MouseScroll;
    }

    bool InputBinding::matches(InputSource eventSource, std::uint16_t eventCode) const noexcept
    {
        // Mouse delta and scroll have no button code, so only the source has to match.
        return source == eventSource && (isPerFrame() || code == eventCode);
    }

    bool InputBinding::acceptsModifiers(InputModifier mods) const noexcept
    {
        // A binding without modifiers works with any held modifiers (e.g. Shift + W while
        // sprinting); a binding with modifiers needs an exact match, ignoring Caps/Num Lock.
        const auto required = getModifiers();
        return required == InputModifier::None || (mods & MatchedModifiers) == required;
    }

    glm::vec2 InputBinding::applyValueModifiers(glm::vec2 value) const noexcept
    {
        if (negate)
        {
            value = -value;
        }
        if (swizzleYX)
        {
            value = glm::vec2{ value.y, value.x };
        }
        return value;
    }

    void to_json(nlohmann::json& j, const InputBinding& v)
    {
        j = R<InputBinding>::Serialize(v).getData();
    }

    void from_json(const nlohmann::json& j, InputBinding& v)
    {
        const RResourceStream<RJsonResourceStream> data(j);
        R<InputBinding>::Deserialize(data, v);
    }

    void to_json(nlohmann::json& j, const InputAction& v)
    {
        j = R<InputAction>::Serialize(v).getData();
    }

    void from_json(const nlohmann::json& j, InputAction& v)
    {
        const RResourceStream<RJsonResourceStream> data(j);
        R<InputAction>::Deserialize(data, v);
    }
} // namespace NX
