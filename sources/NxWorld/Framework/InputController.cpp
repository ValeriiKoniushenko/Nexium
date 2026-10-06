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
#include <optional>
#include <ranges>

namespace
{
    Core::StringAtom MouseButtonName(Platform::Mouse::Key button)
    {
        using Button = Platform::Mouse::Key;
        switch (button)
        {
            case Button::Left:
                return "Left"_atom;
            case Button::Right:
                return "Right"_atom;
            case Button::Middle:
                return "Middle"_atom;
            case Button::_4:
                return "_4"_atom;
            case Button::_5:
                return "_5"_atom;
            case Button::_6:
                return "_6"_atom;
            case Button::_7:
                return "_7"_atom;
            case Button::_8:
                return "_8"_atom;
            default:
                return "None"_atom;
        }
    }

    nlohmann::json SerializeInputButton(NX::InputButton button)
    {
        if (button.isMouse())
        {
            return { { "device", "Mouse" },
                     { "button", MouseButtonName(button.getMouseButton()).toStdString() } };
        }
        return std::string{ R<Platform::Keyboard::Key>::ToString(button.getKeyboardKey()) };
    }

    std::optional<NX::InputButton> DeserializeInputButton(const nlohmann::json& value)
    {
        if (value.is_string())
        {
            if (const auto key = R<Platform::Keyboard::Key>::FromString(value.get<std::string>()))
            {
                return *key;
            }
        }
        else if (value.is_number_integer())
        {
            return static_cast<Platform::Keyboard::Key>(value.get<int>());
        }
        else if (value.is_object() && value.contains("device") && value.at("device").is_string()
                 && value.at("device").get<std::string>() == "Mouse" && value.contains("button"))
        {
            const auto& button = value.at("button");
            if (button.is_string())
            {
                if (const auto key = R<Platform::Mouse::Key>::FromString(button.get<std::string>()))
                {
                    return *key;
                }
            }
        }
        return std::nullopt;
    }
} // namespace

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
        json = nlohmann::json{ { "action", binding.action },
                               { "triggerKey", SerializeInputButton(binding.chord.triggerKey) },
                               { "trigger", binding.trigger == InputActionTrigger::WhileHeld
                                                ? "WhileHeld"
                                                : (binding.trigger == InputActionTrigger::OnRelease
                                                       ? "OnRelease"
                                                       : "OnPress") } };
        auto& keys = json["requiredKeys"] = nlohmann::json::array();
        for (const auto key : binding.chord.requiredKeys)
        {
            keys.push_back(SerializeInputButton(key));
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
            if (const auto button = DeserializeInputButton(json.at(keyField)))
            {
                binding.chord.triggerKey = *button;
            }
        }
        for (const auto& value : json.value("requiredKeys", nlohmann::json::array()))
        {
            if (const auto button = DeserializeInputButton(value))
            {
                binding.chord.requiredKeys.push_back(*button);
            }
        }

        // Compatibility with assets saved before modifiers became regular chord keys.
        const auto oldModifiers = json.value("requiredModifiers", std::uint8_t{});
        const auto appendOldModifier
            = [&binding, oldModifiers](InputModifier modifier, Platform::Keyboard::Key key)
        {
            if ((oldModifiers & static_cast<std::uint8_t>(modifier)) != 0
                && std::ranges::find(binding.chord.requiredKeys, key)
                       == binding.chord.requiredKeys.end())
            {
                binding.chord.requiredKeys.push_back(key);
            }
        };
        appendOldModifier(InputModifier::Control, Platform::Keyboard::Key::Left_Control);
        appendOldModifier(InputModifier::Shift, Platform::Keyboard::Key::Left_Shift);
        appendOldModifier(InputModifier::Alt, Platform::Keyboard::Key::Left_Alt);
        appendOldModifier(InputModifier::Super, Platform::Keyboard::Key::Left_Super);
        const auto trigger = json.value("trigger", std::string{ "OnPress" });
        if (trigger == "WhileHeld")
        {
            binding.trigger = InputActionTrigger::WhileHeld;
        }
        else if (trigger == "OnRelease")
        {
            binding.trigger = InputActionTrigger::OnRelease;
        }
        else
        {
            binding.trigger = InputActionTrigger::OnPress;
        }
    }

    bool InputController::bind(const Core::StringAtom& action, const KeyChord& chord,
                               InputActionTrigger trigger)
    {
        if (action.isEmpty() || chord.triggerKey.isNone())
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
            std::erase_if(_activeBindings,
                          [&action](const Binding& binding) { return binding.action == action; });
            duplicate->chord = chord;
            duplicate->trigger = trigger;
            return false;
        }

        _bindings.push_back({ .action = action, .chord = chord, .trigger = trigger });
        _actionStates.insert_or_assign(action, false);
        return true;
    }

    bool InputController::bind(const Core::StringAtom& action, KeyChord chord,
                               ActionCallback callback, InputActionTrigger trigger)
    {
        if (action.isEmpty() || chord.triggerKey.isNone())
        {
            return false;
        }
        const auto inserted = bind(action, chord, trigger);
        if (!action.isEmpty() && callback)
        {
            setActionCallback(action, std::move(callback));
        }
        return inserted;
    }

    void InputController::setActionCallback(const Core::StringAtom& action, ActionCallback callback)
    {
        if (action.isEmpty())
        {
            return;
        }
        if (callback)
        {
            _actionCallbacks.insert_or_assign(action, std::move(callback));
        }
        else
        {
            _actionCallbacks.erase(action);
        }
    }

    bool InputController::unbind(const Core::StringAtom& action)
    {
        const auto oldSize = _bindings.size();
        std::erase_if(_bindings,
                      [&action](const Binding& binding) { return binding.action == action; });

        _actionStates.erase(action);
        _actionModifiers.erase(action);
        _transientActions.erase(action);
        std::erase_if(_activeBindings,
                      [&action](const Binding& binding) { return binding.action == action; });

        return oldSize != _bindings.size();
    }

    void InputController::clearBindings()
    {
        releaseAllActions();
        _bindings.clear();
        _actionStates.clear();
        _actionModifiers.clear();
        _transientActions.clear();
    }

    void InputController::setBindings(const std::vector<Binding>& bindings)
    {
        const auto updated = bindings;
        clearBindings();
        for (const auto& binding : updated)
        {
            _bindings.push_back(binding);
            if (!binding.action.isEmpty())
            {
                _actionStates.insert_or_assign(binding.action, false);
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

    void InputController::handleRoutedEvent(const KeyInputEvent& event)
    {
        if (event.state == Platform::Keyboard::KeyState::Released)
        {
            handleReleasedEvent(event);
            return;
        }

        if (!isEnabled())
        {
            return;
        }

        if (event.state == Platform::Keyboard::KeyState::Repeated)
        {
            return;
        }

        handlePressedEvent(event);
    }

    void InputController::handleReleasedEvent(const KeyInputEvent& event)
    {
        for (auto it = _activeBindings.begin(); it != _activeBindings.end();)
        {
            if (!it->chord.contains(event.key))
            {
                ++it;
                continue;
            }

            const auto binding = *it;
            _activeBindings.erase(it);
            releaseBinding(binding, event);
            it = _activeBindings.begin();
        }
    }

    void InputController::handlePressedEvent(const KeyInputEvent& event)
    {
        const auto* binding = findBestBinding(event);
        if (!binding)
        {
            return;
        }

        activateBinding(*binding, event);
    }

    const InputController::Binding* InputController::findBestBinding(
        const KeyInputEvent& event) const
    {
        const Binding* best = nullptr;
        for (const auto& candidate : _bindings)
        {
            if (!candidate.chord.matches(event.key, event.pressedKeys))
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

    void InputController::activateBinding(const Binding& binding, const KeyInputEvent& event)
    {
        const auto active = std::ranges::find_if(
            _activeBindings,
            [&binding](const Binding& value)
            {
                return value.action == binding.action && value.trigger == binding.trigger
                       && value.chord.triggerKey == binding.chord.triggerKey
                       && value.chord.requiredKeys == binding.chord.requiredKeys;
            });
        if (active == _activeBindings.end())
        {
            _activeBindings.push_back(binding);
        }
        _actionModifiers.insert_or_assign(binding.action, event.modifiers);

        if (binding.trigger == InputActionTrigger::OnRelease)
        {
            return;
        }

        _actionStates.insert_or_assign(binding.action, true);
        if (binding.trigger == InputActionTrigger::OnPress)
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

    bool InputController::hasHeldBinding(const Core::StringAtom& action) const
    {
        return std::ranges::any_of(_activeBindings,
                                   [&action](const Binding& binding)
                                   {
                                       return binding.action == action
                                              && binding.trigger == InputActionTrigger::WhileHeld;
                                   });
    }

    void InputController::releaseBinding(const Binding& binding, const KeyInputEvent& event)
    {
        const auto& action = binding.action;
        if (binding.trigger == InputActionTrigger::WhileHeld)
        {
            const bool pressed = _transientActions.contains(action) || hasHeldBinding(action);
            _actionStates.insert_or_assign(action, pressed);
            if (!pressed)
            {
                _actionModifiers.insert_or_assign(action, InputModifier::None);
            }
            return;
        }

        if (binding.trigger == InputActionTrigger::OnRelease)
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
            const bool pressed = hasHeldBinding(action);
            _actionStates.insert_or_assign(action, pressed);
            if (!pressed)
            {
                _actionModifiers.insert_or_assign(action, InputModifier::None);
            }
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
        _activeBindings.clear();
        _transientActions.clear();
    }
} // namespace NX
