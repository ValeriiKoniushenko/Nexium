// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputController.h"

#include "InputSystem.h"

#include <algorithm>
#include <ranges>

namespace NX
{
    ECS_IMPL(InputController);

    InputController::InputController(const InputController& other)
        : BaseComponent(other),
          _inputContext(other._inputContext),
          _bindings(other._bindings)
    {
        invalidate();
    }

    InputController::~InputController()
    {
        GetInputSystem().unregisterController(this);
    }

    InputController::Ptr InputController::Create(const Core::StringAtom& name, InputContext context)
    {
        Ptr controller = new InputController(name, context);
        controller->initialize();
        return controller;
    }

    void to_json(nlohmann::json& json, const InputController::Binding& binding)
    {
        json = nlohmann::json{
            { "action", binding.action },
            { "triggerKey",
              std::string{ R<Platform::Keyboard::Key>::ToString(static_cast<Platform::Keyboard::Key>(binding.chord.triggerKey.getIndex())) } },
            { "trigger", binding.trigger == InputState::Repeated
                             ? "WhileHeld"
                             : (binding.trigger == InputState::Released ? "OnRelease" : "OnPress") }
        };
        auto& keys = json["requiredKeys"] = nlohmann::json::array();
        for (const auto key : binding.chord.requiredKeys)
        {
            keys.push_back(std::string{ R<Platform::Keyboard::Key>::ToString(static_cast<Platform::Keyboard::Key>(key.getIndex())) });
        }
    }

    void from_json(const nlohmann::json& json, InputController::Binding& binding)
    {
        binding = {};
        if (json.contains("action"))
        {
            binding.action = Core::StringAtom::Intern(json.at("action").get<Core::StringAtom>());
        }

        const auto* const keyField = json.contains("triggerKey") ? "triggerKey" : "key";
        if (json.contains(keyField))
        {
            if (json.at(keyField).is_string())
            {
                const auto key = R<Platform::Keyboard::Key>::FromString(json.at(keyField).get<std::string>());
                binding.chord.triggerKey = InputButton(static_cast<std::uint16_t>(key.value_or(Platform::Keyboard::Key::None)));
            }
            else if (json.at(keyField).is_number_integer())
            {
                binding.chord.triggerKey = static_cast<InputButton>(json.at(keyField).get<int>());
            }
        }
        for (const auto& value : json.value("requiredKeys", nlohmann::json::array()))
        {
            if (value.is_string())
            {
                if (const auto key = R<Platform::Keyboard::Key>::FromString(value.get<std::string>()))
                {
                    binding.chord.requiredKeys.push_back(InputButton(static_cast<std::uint16_t>(*key)));
                }
            }
            else if (value.is_number_integer())
            {
                binding.chord.requiredKeys.push_back(static_cast<InputButton>(value.get<int>()));
            }
        }

        // Compatibility with assets saved before modifiers became regular chord keys.
        const auto oldModifiers = json.value("requiredModifiers", std::uint8_t{});
        const auto appendOldModifier
            = [&binding, oldModifiers](InputModifier modifier, InputButton key)
        {
            if ((oldModifiers & static_cast<std::uint8_t>(modifier)) != 0
                && std::ranges::find(binding.chord.requiredKeys, key)
                       == binding.chord.requiredKeys.end())
            {
                binding.chord.requiredKeys.push_back(key);
            }
        };
        appendOldModifier(InputModifier::Control,
                          static_cast<int>(Platform::Keyboard::Key::Left_Control));
        appendOldModifier(InputModifier::Shift,
                          static_cast<int>(Platform::Keyboard::Key::Left_Shift));
        appendOldModifier(InputModifier::Alt, static_cast<int>(Platform::Keyboard::Key::Left_Alt));

        appendOldModifier(InputModifier::Super,
                          static_cast<int>(Platform::Keyboard::Key::Left_Super));

        const auto trigger = json.value("trigger", std::string{ "OnPress" });
        if (trigger == "WhileHeld")
        {
            binding.trigger = InputState::Repeated;
        }
        else if (trigger == "OnRelease")
        {
            binding.trigger = InputState::Released;
        }
        else
        {
            binding.trigger = InputState::Pressed;
        }
    }

    bool InputController::bind(const Core::StringAtom& action, const KeyChord& chord,
                               InputState trigger)
    {
        if (action.isEmpty() || chord.triggerKey == NX::InputButton(0))
        {
            return false;
        }

        const auto duplicate = std::ranges::find_if(_bindings, [&action](const Binding& binding)
                                                    { return binding.action == action; });
        if (duplicate != _bindings.end())
        {
            _actionStates.insert_or_assign(action, false);
            _actionModifiers.erase(action);
            _transientActions.erase(action);
            _activeChords.erase(action);
            duplicate->chord = chord;
            duplicate->trigger = trigger;
            return false;
        }

        _bindings.push_back({ .action = action, .chord = chord, .trigger = trigger });
        _actionStates.insert_or_assign(action, false);
        return true;
    }

    bool InputController::bind(const Core::StringAtom& action, KeyChord chord,
                               ActionCallback callback, InputState trigger)
    {
        if (action.isEmpty() || chord.triggerKey.getIndex() == static_cast<int>(0))
        {
            return false;
        }
        const auto inserted = bind(action, chord, trigger);
        if (!action.isEmpty() && callback)
        {
            _actionCallbacks.insert_or_assign(action, std::move(callback));
        }
        return inserted;
    }

    bool InputController::unbind(const Core::StringAtom& action)
    {
        const auto oldSize = _bindings.size();
        std::erase_if(_bindings,
                      [&action](const Binding& binding) { return binding.action == action; });

        _actionStates.erase(action);
        _actionModifiers.erase(action);
        _transientActions.erase(action);
        _activeChords.erase(action);
        _actionCallbacks.erase(action);

        return oldSize != _bindings.size();
    }

    void InputController::clearBindings()
    {
        releaseAllActions();
        _bindings.clear();
        _actionStates.clear();
        _actionModifiers.clear();
        _transientActions.clear();
        _actionCallbacks.clear();
    }

    void InputController::setBindings(const std::vector<Binding>& bindings)
    {
        const auto updated = bindings;
        auto callbacks = std::move(_actionCallbacks);
        clearBindings();
        for (const auto& binding : updated)
        {
            _bindings.push_back(binding);
            if (!binding.action.isEmpty())
            {
                _actionStates.insert_or_assign(binding.action, false);
                if (const auto callback = callbacks.find(binding.action);
                    callback != callbacks.end())
                {
                    _actionCallbacks.emplace(binding.action, std::move(callback->second));
                }
            }
        }
    }
    bool InputController::isActionPressed(const Core::StringAtom& action) const
    {
        const auto it = _actionStates.find(action);
        return it != _actionStates.end() && it->second;
    }
    InputModifier InputController::getActionModifiers(const Core::StringAtom& action) const
    {
        const auto it = _actionModifiers.find(action);
        return it != _actionModifiers.end() ? it->second : InputModifier::None;
    }
    Tag InputController::getTags() const
    {
        return BaseComponent::getTags() | Tag_InputController;
    }
    void InputController::onInitialize()
    {
        BaseComponent::onInitialize();
        GetInputSystem().registerController(this);
    }

    void InputController::onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs)
    {
        BaseComponent::onPostDeserialize(obj, logs);
        setBindings(_bindings);
    }

    void InputController::handleRoutedEvent(const InputEvent& event)
    {
        if (event.state == InputState::Released)
        {
            handleReleasedEvent(event);
            return;
        }

        if (!isEnabled())
        {
            return;
        }

        if (event.state == InputState::Repeated)
        {
            return;
        }

        handlePressedEvent(event);
    }
    void InputController::handleReleasedEvent(const InputEvent& event)
    {
        for (auto it = _activeChords.begin(); it != _activeChords.end();)
        {
            if (!it->second.contains(event.button))
            {
                ++it;
                continue;
            }

            const auto action = it->first;
            _activeChords.erase(it);
            releaseBinding(action, event);
            it = _activeChords.begin();
        }
    }
    void InputController::handlePressedEvent(const InputEvent& event)
    {
        const auto* binding = findBestBinding(event);
        if (!binding)
        {
            return;
        }

        activateBinding(*binding, event);
    }
    const InputController::Binding* InputController::findBestBinding(const InputEvent& event) const
    {
        const Binding* best = nullptr;
        for (const auto& candidate : _bindings)
        {
            if (!candidate.chord.matches(event.button, event.allPressedButtons))
            {
                continue;
            }

            if (!best || candidate.chord.requiredKeys.size() > best->chord.requiredKeys.size())
            {
                best = &candidate;
            }
        }
        return best;
    }
    void InputController::activateBinding(const Binding& binding, const InputEvent& event)
    {
        _activeChords.insert_or_assign(binding.action, binding.chord);
        _actionModifiers.insert_or_assign(binding.action, event.mods);

        if (binding.trigger == InputState::Released)
        {
            _actionStates.insert_or_assign(binding.action, false);
            return;
        }

        _actionStates.insert_or_assign(binding.action, true);
        if (binding.trigger == InputState::Pressed)
        {
            _transientActions.insert(binding.action);
        }

        const InputActionEvent actionEvent{ .action = binding.action, .state = event.state };
        if (const auto callback = _actionCallbacks.find(binding.action);
            callback != _actionCallbacks.end())
        {
            const auto invoke = callback->second;
            invoke(actionEvent);
        }
        onAction->trigger(actionEvent);
    }
    void InputController::releaseBinding(const Core::StringAtom& action, const InputEvent& event)
    {
        const auto binding = std::ranges::find_if(_bindings, [&action](const Binding& value)
                                                  { return value.action == action; });
        if (binding == _bindings.end())
        {
            return;
        }

        if (binding->trigger == InputState::Pressed)
        {
            _actionStates.insert_or_assign(action, false);
            _actionModifiers.insert_or_assign(action, InputModifier::None);
            return;
        }

        if (binding->trigger == InputState::Released)
        {
            _actionStates.insert_or_assign(action, true);
            _transientActions.insert(action);
            const InputActionEvent actionEvent{ .action = action, .state = event.state };
            if (const auto callback = _actionCallbacks.find(action);
                callback != _actionCallbacks.end())
            {
                const auto invoke = callback->second;
                invoke(actionEvent);
            }
            onAction->trigger(actionEvent);
        }
    }
    void InputController::beginInputFrame()
    {
        for (const auto& action : _transientActions)
        {
            _actionStates.insert_or_assign(action, false);
            _actionModifiers.insert_or_assign(action, InputModifier::None);
        }
        _transientActions.clear();
    }
    void InputController::releaseAllActions()
    {
        for (auto& [action, pressed] : _actionStates)
        {
            pressed = false;
            _actionModifiers.insert_or_assign(action, InputModifier::None);
        }
        _activeChords.clear();
        _transientActions.clear();
    }
} // namespace NX
