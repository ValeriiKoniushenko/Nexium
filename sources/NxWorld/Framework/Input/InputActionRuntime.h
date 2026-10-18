// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "InputMap.h"

#include <functional>
#include <unordered_map>

namespace NX
{
    /// Interprets configured triggers without owning bindings, callbacks, or contexts.
    class InputActionRuntime
    {
    public:
        [[nodiscard]] bool isActive(const Core::StringAtom& action) const;
        [[nodiscard]] InputModifier getModifiers(const Core::StringAtom& action) const;
        [[nodiscard]] glm::vec2 getValue(const Core::StringAtom& action) const;

        [[nodiscard]] std::optional<InputActionEvent> activate(const InputBinding& binding,
                                                               const KeyInputEvent& event);
        [[nodiscard]] std::vector<InputActionEvent> release(const KeyInputEvent& event);
        void beginFrame();
        void cancel(InputChannel channels = InputChannel::All);
        void cancelAction(const Core::StringAtom& action);
        void cancelMatching(const std::function<bool(const InputBinding&)>& predicate);

    private:
        struct ActiveBinding
        {
            InputBinding binding;
            InputModifier modifiers = InputModifier::None;
            glm::vec2 value{};
            glm::vec2 position{};
        };

        struct ActionState
        {
            bool active = false;
            InputModifier modifiers = InputModifier::None;
            glm::vec2 value{};
        };

        void rebuildStates();
        void addState(const ActiveBinding& binding);

        std::vector<ActiveBinding> _activeBindings;
        std::vector<ActiveBinding> _transientBindings;
        std::unordered_map<Core::StringAtom, ActionState> _states;
    };
} // namespace NX
