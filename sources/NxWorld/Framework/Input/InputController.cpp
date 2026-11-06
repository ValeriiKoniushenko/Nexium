// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/Input/InputController.h"

#include <algorithm>

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

    ECS_IMPL(NX::InputController);

    bool InputController::addAction(InputAction action)
    {
        if (action.name.isEmpty() || findAction(action.name))
        {
            return false;
        }
        _actions.emplace_back(std::move(action));
        return true;
    }

    bool InputController::removeAction(const Core::StringAtom& name)
    {
        return std::erase_if(_actions,
                             [&](const InputAction& action) { return action.name == name; })
               > 0;
    }

    const InputAction* InputController::findAction(const Core::StringAtom& name) const
    {
        const auto it = std::ranges::find(_actions, name, &InputAction::name);
        return it != _actions.end() ? &*it : nullptr;
    }

    InputAction* InputController::findAction(const Core::StringAtom& name)
    {
        const auto it = std::ranges::find(_actions, name, &InputAction::name);
        return it != _actions.end() ? &*it : nullptr;
    }
} // namespace NX
