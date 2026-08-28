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
                  : (binding.trigger == InputActionTrigger::OnRelease ? "OnRelease" : "OnPress") }
        };
        auto& keys = json["requiredKeys"] = nlohmann::json::array();
        for (const auto key : binding.chord.requiredKeys)
        {
            keys.push_back(std::string{ R<Keyboard::Key>::ToString(key) });
        }
    }

    void from_json(const nlohmann::json& json, InputController::Binding& binding)
    {
        binding = {};
        if (json.contains("action"))
        {
            binding.action = StringAtom::Intern(json.at("action").get<StringAtom>());
        }

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
                {
                    binding.chord.requiredKeys.push_back(*key);
                }
            }
            else if (value.is_number_integer())
            {
                binding.chord.requiredKeys.push_back(static_cast<Keyboard::Key>(value.get<int>()));
            }
        }

        // Compatibility with assets saved before modifiers became regular chord keys.
        const auto oldModifiers = json.value("requiredModifiers", std::uint8_t{});
        const auto appendOldModifier
            = [&binding, oldModifiers](InputModifier modifier, Keyboard::Key key)
        {
            if ((oldModifiers & static_cast<std::uint8_t>(modifier)) != 0
                && std::ranges::find(binding.chord.requiredKeys, key)
                       == binding.chord.requiredKeys.end())
            {
                binding.chord.requiredKeys.push_back(key);
            }
        };
        appendOldModifier(InputModifier::Control, Keyboard::Key::Left_Control);
        appendOldModifier(InputModifier::Shift, Keyboard::Key::Left_Shift);
        appendOldModifier(InputModifier::Alt, Keyboard::Key::Left_Alt);
        appendOldModifier(InputModifier::Super, Keyboard::Key::Left_Super);
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

    ECS_IMPL(InputController);

    /**
     * @brief InputController::bind - Adds or updates a binding for an input action.
     *
     * Associates an action with a key chord and trigger type. If the action is already bound,
     * its existing binding is updated instead of adding a duplicate.
     *
     * @param action The action to bind.
     * @param chord The key chord that activates the action.
     * @param trigger The condition under which the action is triggered.
     * @return true if a new binding was added, false if the input was invalid or an existing
     * binding was updated.
     */
    bool InputController::bind(const StringAtom& action, KeyChord chord, InputActionTrigger trigger)
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

    /**
     * @brief InputController::unbind - Removes all bindings and associated state for the specified
     * action.
     *
     * Removes all input bindings associated with the given action and clears
     * its cached state, modifiers, transient state, and active chords.
     *
     * @param action The action to unbind.
     * @return true if at least one binding was removed, false otherwise.
     */
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
    /**
     * @brief InputController::clearBindings - Removes every binding and resets input state.
     *
     * Releases all active actions and clears the bindings, cached states, modifiers, and
     * transient actions owned by this controller.
     */
    void InputController::clearBindings()
    {
        releaseAllActions();
        _bindings.clear();
        _actionStates.clear();
        _actionModifiers.clear();
        _transientActions.clear();
    }

    /**
     * @brief InputController::setBindings - Replaces all bindings with a copied collection.
     *
     * Clears the current configuration, copies the supplied bindings, and initializes the cached
     * state of every valid action as not pressed.
     *
     * @param bindings The bindings to copy into the controller.
     */
    void InputController::setBindings(const std::vector<Binding>& bindings)
    {
        clearBindings();
        for (const auto& binding : bindings)
        {
            _bindings.push_back(binding);
            if (!binding.action.isEmpty())
            {
                _actionStates.insert_or_assign(binding.action, false);
            }
        }
    }

    /**
     * @brief InputController::setBindings - Replaces all bindings with a movable collection.
     *
     * Applies the supplied bindings using the same state initialization rules as the copying
     * overload.
     *
     * @param bindings The bindings to apply to the controller.
     */
    void InputController::setBindings(std::vector<Binding>&& bindings)
    {
        setBindings(static_cast<const std::vector<Binding>&>(bindings));
    }

    /**
     * @brief InputController::isActionPressed - Checks whether an action is currently active.
     *
     * Reads the cached state produced while processing routed keyboard events.
     *
     * @param action The action whose state should be checked.
     * @return true if the action is active, false if it is inactive or unknown.
     */
    bool InputController::isActionPressed(const StringAtom& action) const
    {
        const auto it = _actionStates.find(action);
        return it != _actionStates.end() && it->second;
    }

    /**
     * @brief InputController::getActionModifiers - Returns modifiers captured for an action.
     *
     * Reads the modifier flags stored when the action was activated.
     *
     * @param action The action whose modifiers should be returned.
     * @return The captured modifiers, or InputModifier::None if no modifiers are stored.
     */
    InputModifier InputController::getActionModifiers(const StringAtom& action) const
    {
        const auto it = _actionModifiers.find(action);
        return it != _actionModifiers.end() ? it->second : InputModifier::None;
    }

    /**
     * @brief InputController::getTags - Returns the component tags for an input controller.
     *
     * Extends the base component tags with Tag_InputController.
     *
     * @return The combined component tags.
     */
    Tag InputController::getTags() const
    {
        return BaseComponent::getTags() | Tag_InputController;
    }

    /**
     * @brief InputController::onInitialize - Registers the controller with the input system.
     *
     * Initializes the base component and registers this controller in its fixed input context.
     */
    void InputController::onInitialize()
    {
        BaseComponent::onInitialize();
        Internal::InputSystem::Instance().registerController(this);
    }

    /**
     * @brief InputController::handleRoutedEvent - Processes a keyboard event routed to the
     * controller.
     *
     * Always handles releases so cached actions cannot remain active. Press events are processed
     * only while the component is enabled, and repeated events are ignored.
     *
     * @param event The routed keyboard event to process.
     */
    void InputController::handleRoutedEvent(const KeyInputEvent& event)
    {
        if (event.state == Keyboard::KeyState::Released)
        {
            handleReleasedEvent(event);
            return;
        }

        if (!isEnabled())
        {
            return;
        }

        if (event.state == Keyboard::KeyState::Repeated)
        {
            return;
        }

        handlePressedEvent(event);
    }

    /**
     * @brief InputController::handleReleasedEvent - Releases actions affected by a key release.
     *
     * Finds every active chord containing the released key, removes it from the active chord map,
     * and applies the binding's release behavior.
     *
     * @param event The keyboard release event to process.
     */
    void InputController::handleReleasedEvent(const KeyInputEvent& event)
    {
        for (auto it = _activeChords.begin(); it != _activeChords.end();)
        {
            if (!it->second.contains(event.key))
            {
                ++it;
                continue;
            }

            const auto action = it->first;
            it = _activeChords.erase(it);
            releaseBinding(action, event);
        }
    }

    /**
     * @brief InputController::handlePressedEvent - Activates the best matching binding.
     *
     * Selects the most specific binding matching the pressed key and current key set, then applies
     * its trigger behavior.
     *
     * @param event The keyboard press event to process.
     */
    void InputController::handlePressedEvent(const KeyInputEvent& event)
    {
        const auto* binding = findBestBinding(event);
        if (!binding)
        {
            return;
        }

        activateBinding(*binding, event);
    }

    /**
     * @brief InputController::findBestBinding - Finds the most specific matching binding.
     *
     * When multiple chords match, prefers the binding with the greatest number of required keys.
     *
     * @param event The keyboard event and pressed-key set to match.
     * @return A pointer to the best binding, or nullptr if no binding matches.
     */
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

    /**
     * @brief InputController::activateBinding - Applies the pressed state of a binding.
     *
     * Stores the active chord and modifiers, updates the cached action state according to the
     * trigger type, and emits an action event when appropriate.
     *
     * @param binding The binding being activated.
     * @param event The keyboard event that activated the binding.
     */
    void InputController::activateBinding(const Binding& binding, const KeyInputEvent& event)
    {
        _activeChords.insert_or_assign(binding.action, binding.chord);
        _actionModifiers.insert_or_assign(binding.action, event.modifiers);

        if (binding.trigger == InputActionTrigger::OnRelease)
        {
            _actionStates.insert_or_assign(binding.action, false);
            return;
        }

        _actionStates.insert_or_assign(binding.action, true);
        if (binding.trigger == InputActionTrigger::OnPress)
        {
            _transientActions.insert(binding.action);
        }

        onAction->trigger(InputActionEvent{ .action = binding.action, .state = event.state });
    }

    /**
     * @brief InputController::releaseBinding - Applies the release behavior of an action.
     *
     * Clears held actions immediately and activates transient OnRelease actions for the current
     * frame.
     *
     * @param action The action associated with the released chord.
     * @param event The keyboard release event that ended the chord.
     */
    void InputController::releaseBinding(const StringAtom& action, const KeyInputEvent& event)
    {
        const auto binding = std::ranges::find_if(_bindings, [&action](const Binding& value)
                                                  { return value.action == action; });
        if (binding == _bindings.end())
        {
            return;
        }

        if (binding->trigger == InputActionTrigger::WhileHeld)
        {
            _actionStates.insert_or_assign(action, false);
            _actionModifiers.insert_or_assign(action, InputModifier::None);
            return;
        }

        if (binding->trigger == InputActionTrigger::OnRelease)
        {
            _actionStates.insert_or_assign(action, true);
            _transientActions.insert(action);
            onAction->trigger(InputActionEvent{ .action = action, .state = event.state });
        }
    }

    /**
     * @brief InputController::beginInputFrame - Resets transient actions from the previous frame.
     *
     * Clears the cached state and modifiers of OnPress and OnRelease actions after they have been
     * visible for one input frame.
     */
    void InputController::beginInputFrame()
    {
        for (const auto& action : _transientActions)
        {
            _actionStates.insert_or_assign(action, false);
            _actionModifiers.insert_or_assign(action, InputModifier::None);
        }
        _transientActions.clear();
    }

    /**
     * @brief InputController::releaseAllActions - Resets all active input state.
     *
     * Marks every known action as released, clears its modifiers, and discards active chords and
     * transient actions. This prevents state from leaking when input routing changes.
     */
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
