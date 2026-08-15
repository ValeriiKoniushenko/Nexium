/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "InputController.h"

#include "InputDevices/Internal/InputSystem.h"

#include <algorithm>
#include <ranges>

namespace Core
{
    void to_json(nlohmann::json& json, const InputController::Binding& binding)
    {
        json = nlohmann::json{
            { "action", binding.action },
            { "triggerKey", std::string{ R<Keyboard::Key>::ToString(binding.chord.triggerKey) } },
            { "trigger",
              binding.trigger == InputActionTrigger::WhileHeld
                  ? "WhileHeld"
                  : (binding.trigger == InputActionTrigger::OnRelease ? "OnRelease"
                                                                       : "OnPress") }
        };
        auto& keys = json["requiredKeys"] = nlohmann::json::array();
        for (const auto key : binding.chord.requiredKeys)
            keys.push_back(std::string{ R<Keyboard::Key>::ToString(key) });
    }

    void from_json(const nlohmann::json& json, InputController::Binding& binding)
    {
        binding = {};
        if (json.contains("action"))
            binding.action = StringAtom::Intern(json.at("action").get<StringAtom>());

        const auto keyField = json.contains("triggerKey") ? "triggerKey" : "key";
        if (json.contains(keyField))
        {
            if (json.at(keyField).is_string())
            {
                const auto key = R<Keyboard::Key>::FromString(json.at(keyField).get<std::string>());
                binding.chord.triggerKey = key.value_or(Keyboard::Key::None);
            }
            else if (json.at(keyField).is_number_integer())
            {
                binding.chord.triggerKey = static_cast<Keyboard::Key>(json.at(keyField).get<int>());
            }
        }
        for (const auto& value : json.value("requiredKeys", nlohmann::json::array()))
        {
            if (value.is_string())
            {
                if (const auto key = R<Keyboard::Key>::FromString(value.get<std::string>()))
                    binding.chord.requiredKeys.push_back(*key);
            }
            else if (value.is_number_integer())
                binding.chord.requiredKeys.push_back(static_cast<Keyboard::Key>(value.get<int>()));
        }

        // Compatibility with assets saved before modifiers became regular chord keys.
        const auto oldModifiers = json.value("requiredModifiers", std::uint8_t{});
        const auto appendOldModifier = [&binding, oldModifiers](InputModifier modifier,
                                                                Keyboard::Key key)
        {
            if ((oldModifiers & static_cast<std::uint8_t>(modifier)) != 0
                && std::ranges::find(binding.chord.requiredKeys, key)
                       == binding.chord.requiredKeys.end())
                binding.chord.requiredKeys.push_back(key);
        };
        appendOldModifier(InputModifier::Control, Keyboard::Key::Left_Control);
        appendOldModifier(InputModifier::Shift, Keyboard::Key::Left_Shift);
        appendOldModifier(InputModifier::Alt, Keyboard::Key::Left_Alt);
        appendOldModifier(InputModifier::Super, Keyboard::Key::Left_Super);
        const auto trigger = json.value("trigger", std::string{ "OnPress" });
        if (trigger == "WhileHeld")
            binding.trigger = InputActionTrigger::WhileHeld;
        else if (trigger == "OnRelease")
            binding.trigger = InputActionTrigger::OnRelease;
        else
            binding.trigger = InputActionTrigger::OnPress;
    }

    ECS_IMPL(InputController);

    bool InputController::bind(const StringAtom& action, KeyChord chord,
                               InputActionTrigger trigger)
    {
        if (action.isEmpty() || chord.triggerKey == Keyboard::Key::None)
        {
            return false;
        }

        const auto duplicate = std::ranges::find_if(_bindings, [&action](const Binding& binding)
                                                    { return binding.action == action; });
        if (duplicate != _bindings.end())
        {
            duplicate->chord = chord;
            duplicate->trigger = trigger;
            return false;
        }

        _bindings.push_back({ .action = action, .chord = chord, .trigger = trigger });
        _actionStates.insert_or_assign(action, false);
        return true;
    }

    bool InputController::unbind(const StringAtom& action)
    {
        const auto oldSize = _bindings.size();
        std::erase_if(_bindings,
                      [&action](const Binding& binding) { return binding.action == action; });
        _actionStates.erase(action);
        _actionModifiers.erase(action);
        _transientActions.erase(action);
        _activeChords.erase(action);
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
        clearBindings();
        for (const auto& binding : bindings)
        {
            _bindings.push_back(binding);
            if (!binding.action.isEmpty())
                _actionStates.insert_or_assign(binding.action, false);
        }
    }

    void InputController::setBindings(std::vector<Binding>&& bindings)
    {
        setBindings(static_cast<const std::vector<Binding>&>(bindings));
    }

    void InputController::setInputActive(bool active)
    {
        if (_inputActive == active)
        {
            return;
        }

        _inputActive = active;
        refreshActiveState();
    }

    bool InputController::isInputActive() const
    {
        return isEnabled() && _inputActive
               && (!_activationPredicate || std::invoke(_activationPredicate));
    }

    bool InputController::isActionPressed(const StringAtom& action) const
    {
        const auto it = _actionStates.find(action);
        return it != _actionStates.end() && it->second;
    }

    InputModifier InputController::getActionModifiers(const StringAtom& action) const
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
        _wasEffectivelyActive = isInputActive();
        Internal::InputSystem::Instance().registerController(this);
    }

    InputResult InputController::handleRoutedEvent(const KeyInputEvent& event)
    {
        if (event.state == Keyboard::KeyState::Released)
        {
            bool released = false;
            for (auto it = _activeChords.begin(); it != _activeChords.end();)
            {
                if (!it->second.contains(event.key))
                {
                    ++it;
                    continue;
                }
                const auto action = it->first;
                it = _activeChords.erase(it);
                const auto binding = std::ranges::find_if(
                    _bindings, [&action](const Binding& value) { return value.action == action; });
                if (binding != _bindings.end())
                {
                    if (binding->trigger == InputActionTrigger::WhileHeld)
                    {
                        _actionStates.insert_or_assign(action, false);
                        _actionModifiers.insert_or_assign(action, InputModifier::None);
                    }
                    else if (binding->trigger == InputActionTrigger::OnRelease)
                    {
                        _actionStates.insert_or_assign(action, true);
                        _transientActions.insert(action);
                        onAction->trigger(
                            InputActionEvent{ .action = action, .state = event.state });
                    }
                }
                released = true;
            }
            return released ? InputResult::Consumed : InputResult::Ignored;
        }

        if (!isInputActive())
        {
            return InputResult::Ignored;
        }

        if (event.state == Keyboard::KeyState::Repeated)
        {
            const auto active = std::ranges::find_if(_activeChords, [&event](const auto& pair)
                                                     { return pair.second.triggerKey == event.key; });
            if (active == _activeChords.end())
            {
                return InputResult::Ignored;
            }

            return InputResult::Consumed;
        }

        auto binding = _bindings.end();
        for (auto it = _bindings.begin(); it != _bindings.end(); ++it)
        {
            if (!it->chord.matches(event.key, event.pressedKeys))
                continue;
            if (binding == _bindings.end()
                || it->chord.requiredKeys.size() > binding->chord.requiredKeys.size())
                binding = it;
        }
        if (binding == _bindings.end())
        {
            return _blockingPolicy == InputBlockingPolicy::BlockKeyboard ? InputResult::Consumed
                                                                         : InputResult::Ignored;
        }

        _activeChords.insert_or_assign(binding->action, binding->chord);
        _actionModifiers.insert_or_assign(binding->action, event.modifiers);
        if (binding->trigger == InputActionTrigger::WhileHeld)
        {
            _actionStates.insert_or_assign(binding->action, true);
            onAction->trigger(
                InputActionEvent{ .action = binding->action, .state = event.state });
        }
        else if (binding->trigger == InputActionTrigger::OnPress)
        {
            _actionStates.insert_or_assign(binding->action, true);
            _transientActions.insert(binding->action);
            onAction->trigger(InputActionEvent{ .action = binding->action, .state = event.state });
        }
        else
        {
            _actionStates.insert_or_assign(binding->action, false);
        }
        return InputResult::Consumed;
    }

    std::optional<std::size_t> InputController::getMatchingSpecificity(
        const KeyInputEvent& event) const
    {
        std::optional<std::size_t> result;
        for (const auto& binding : _bindings)
        {
            if (binding.chord.matches(event.key, event.pressedKeys))
                result = std::max(result.value_or(0), binding.chord.requiredKeys.size());
        }
        return result;
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

    void InputController::refreshActiveState()
    {
        const bool active = isInputActive();
        if (_wasEffectivelyActive && !active)
        {
            releaseAllActions();
        }
        else if (!_wasEffectivelyActive && active)
        {
            Internal::InputSystem::Instance().activateController(this);
        }
        _wasEffectivelyActive = active;
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
} // namespace Core
