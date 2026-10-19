// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputActionRuntime.h"

#include <algorithm>
#include <ranges>

namespace
{
    bool IsSameBinding(const NX::InputBinding& lhs, const NX::InputBinding& rhs)
    {
        return lhs.action == rhs.action && lhs.trigger == rhs.trigger && lhs.source == rhs.source
               && lhs.chord.triggerKey == rhs.chord.triggerKey
               && lhs.chord.requiredKeys == rhs.chord.requiredKeys;
    }

    NX::InputChannel GetBindingChannel(const NX::InputBinding& binding)
    {
        return binding.source == NX::InputEventType::Button && !binding.chord.triggerKey.isMouse()
                   ? NX::InputChannel::Keyboard
                   : NX::InputChannel::Pointer;
    }
} // namespace

namespace NX
{
    bool InputActionRuntime::isActive(const Core::StringAtom& action) const
    {
        const auto found = _states.find(action);
        return found != _states.end() && found->second.active;
    }

    InputModifier InputActionRuntime::getModifiers(const Core::StringAtom& action) const
    {
        const auto found = _states.find(action);
        return found != _states.end() ? found->second.modifiers : InputModifier::None;
    }

    glm::vec2 InputActionRuntime::getValue(const Core::StringAtom& action) const
    {
        const auto found = _states.find(action);
        return found != _states.end() ? found->second.value : glm::vec2{};
    }

    std::optional<InputActionEvent> InputActionRuntime::activate(const InputBinding& binding,
                                                                 const KeyInputEvent& event)
    {
        ActiveBinding active{ .binding = binding,
                              .modifiers = event.modifiers,
                              .value = event.type == InputEventType::Button ? glm::vec2{ 1.f, 0.f }
                                                                            : event.value,
                              .position = event.position };
        if (event.type == InputEventType::Button)
        {
            if (std::ranges::any_of(_activeBindings, [&binding](const ActiveBinding& value)
                                    { return IsSameBinding(value.binding, binding); }))
            {
                return std::nullopt;
            }
            _activeBindings.push_back(active);
            if (binding.trigger == InputActionTrigger::OnRelease)
            {
                return std::nullopt;
            }
        }

        if (binding.trigger == InputActionTrigger::OnPress || event.type != InputEventType::Button)
        {
            _transientBindings.push_back(active);
        }
        rebuildStates();
        return InputActionEvent{ .action = binding.action,
                                 .state = event.state,
                                 .value = active.value,
                                 .position = event.position,
                                 .modifiers = event.modifiers };
    }

    std::vector<InputActionEvent> InputActionRuntime::release(const KeyInputEvent& event)
    {
        std::vector<InputActionEvent> actions;
        for (auto it = _activeBindings.begin(); it != _activeBindings.end();)
        {
            const auto& chord = it->binding.chord;
            const bool triggerHeld
                = std::ranges::find(event.pressedKeys, NormalizeInputButton(chord.triggerKey))
                  != event.pressedKeys.end();
            if (triggerHeld && chord.matches(chord.triggerKey, event.pressedKeys))
            {
                ++it;
                continue;
            }
            auto released = *it;
            it = _activeBindings.erase(it);
            if (released.binding.trigger == InputActionTrigger::OnRelease)
            {
                released.modifiers = event.modifiers;
                released.position = event.position;
                _transientBindings.push_back(released);
                actions.push_back({ .action = released.binding.action,
                                    .state = Platform::Keyboard::KeyState::Released,
                                    .value = released.value,
                                    .position = event.position,
                                    .modifiers = event.modifiers });
            }
        }
        rebuildStates();
        return actions;
    }

    void InputActionRuntime::beginFrame()
    {
        _transientBindings.clear();
        rebuildStates();
    }

    void InputActionRuntime::cancel(InputChannel channels)
    {
        const auto affected = [channels](const ActiveBinding& binding)
        { return HasInputChannel(channels, GetBindingChannel(binding.binding)); };
        std::erase_if(_activeBindings, affected);
        std::erase_if(_transientBindings, affected);
        rebuildStates();
    }

    void InputActionRuntime::cancelAction(const Core::StringAtom& action)
    {
        const auto affected
            = [&action](const ActiveBinding& binding) { return binding.binding.action == action; };
        std::erase_if(_activeBindings, affected);
        std::erase_if(_transientBindings, affected);
        rebuildStates();
    }

    void InputActionRuntime::cancelMatching(
        const std::function<bool(const InputBinding&)>& predicate)
    {
        std::vector<InputBinding> bindingsToCancel;

        const auto collect = [&](const ActiveBinding& active)
        {
            const auto& binding = active.binding;

            if (!predicate(binding))
            {
                return;
            }

            const bool alreadyCollected
                = std::ranges::any_of(bindingsToCancel, [&binding](const InputBinding& value)
                                      { return IsSameBinding(value, binding); });

            if (!alreadyCollected)
            {
                bindingsToCancel.push_back(binding);
            }
        };

        for (const auto& active : _activeBindings)
        {
            collect(active);
        }

        for (const auto& transient : _transientBindings)
        {
            collect(transient);
        }

        if (bindingsToCancel.empty())
        {
            return;
        }

        const auto shouldCancel = [&](const ActiveBinding& active)
        {
            return std::ranges::any_of(bindingsToCancel, [&active](const InputBinding& binding)
                                       { return IsSameBinding(active.binding, binding); });
        };

        std::erase_if(_activeBindings, shouldCancel);
        std::erase_if(_transientBindings, shouldCancel);

        rebuildStates();
    }

    void InputActionRuntime::rebuildStates()
    {
        _states.clear();
        for (const auto& binding : _activeBindings)
        {
            if (binding.binding.trigger == InputActionTrigger::WhileHeld)
            {
                addState(binding);
            }
        }
        for (const auto& binding : _transientBindings)
        {
            addState(binding);
        }
    }

    void InputActionRuntime::addState(const ActiveBinding& binding)
    {
        auto& state = _states[binding.binding.action];
        state.active = true;
        state.modifiers = state.modifiers | binding.modifiers;
        state.value += binding.value;
    }
} // namespace NX
