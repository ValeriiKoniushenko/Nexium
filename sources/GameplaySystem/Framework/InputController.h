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

#pragma once

#include "Core/Delegate.h"
#include "ECS/BaseComponent.h"
#include "InputDevices/InputTypes.h"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Core
{
    class InputSystem;

    CLASS();
    class InputController : public BaseComponent
    {
        ECS_DECL(InputController, Core::BaseComponent);

    public:
        using ActionCallback = std::function<void(const InputActionEvent&)>;

        InputController(const StringAtom& name, InputContext context)
            : InputController(name)
        {
            _inputContext = context;
        }
        InputController(const InputController&) = default;
        InputController& operator=(const InputController&) = default;
        InputController(InputController&&) = default;
        InputController& operator=(InputController&&) = default;

        ~InputController() override;

        [[nodiscard]] static Ptr Create(const StringAtom& name, InputContext context);

        struct Binding
        {
            StringAtom action;
            KeyChord chord;
            InputActionTrigger trigger = InputActionTrigger::OnPress;
        };

        bool bind(const StringAtom& action, const KeyChord& chord,
                  InputActionTrigger trigger = InputActionTrigger::OnPress);

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
        bool bind(const StringAtom& action, KeyChord chord, ActionCallback callback,
                  InputActionTrigger trigger = InputActionTrigger::OnPress);

        /**
         * @brief InputController::clearBindings - Removes every binding and resets input state.
         *
         * Releases all active actions and clears the bindings, cached states, modifiers, and
         * transient actions owned by this controller.
         */
        void clearBindings();

        /**
         * @brief InputController::unbind - Removes all bindings and associated state for the
         * specified action.
         *
         * Removes all input bindings associated with the given action and clears
         * its cached state, modifiers, transient state, and active chords.
         *
         * @param action The action to unbind.
         * @return true if at least one binding was removed, false otherwise.
         */
        bool unbind(const StringAtom& action);

        /**
         * @brief InputController::setBindings - Replaces all bindings with a copied collection.
         *
         * Clears the current configuration, copies the supplied bindings, and initializes the
         * cached state of every valid action as not pressed.
         *
         * @param bindings The bindings to copy into the controller.
         */
        void setBindings(const std::vector<Binding>& bindings);

        [[nodiscard]] InputContext getInputContext() const noexcept { return _inputContext; }

        /**
         * @brief InputController::isActionPressed - Checks whether an action is currently active.
         *
         * Reads the cached state produced while processing routed keyboard events.
         *
         * @param action The action whose state should be checked.
         * @return true if the action is active, false if it is inactive or unknown.
         */
        [[nodiscard]] bool isActionPressed(const StringAtom& action) const;

        /**
         * @brief InputController::getActionModifiers - Returns modifiers captured for an action.
         *
         * Reads the modifier flags stored when the action was activated.
         *
         * @param action The action whose modifiers should be returned.
         * @return The captured modifiers, or InputModifier::None if no modifiers are stored.
         */
        [[nodiscard]] InputModifier getActionModifiers(const StringAtom& action) const;

        [[nodiscard]] const std::vector<Binding>& getBindings() const noexcept { return _bindings; }

        [[nodiscard]] Tag getTags() const override;

        Delegate<void(const InputActionEvent&)>::Ptr onAction
            = Delegate<void(const InputActionEvent&)>::Create();

    protected:
        void onInitialize() override;

    private:
        friend class InputSystem;

        /**
         * @brief InputController::handleRoutedEvent - Processes a keyboard event routed to the
         * controller.
         *
         * Always handles releases so cached actions cannot remain active. Press events are
         * processed only while the component is enabled, and repeated events are ignored.
         *
         * @param event The routed keyboard event to process.
         */
        void handleRoutedEvent(const KeyInputEvent& event);
        /**
         * @brief InputController::handleReleasedEvent - Releases actions affected by a key release.
         *
         * Finds every active chord containing the released key, removes it from the active chord
         * map, and applies the binding's release behavior.
         *
         * @param event The keyboard release event to process.
         */
        void handleReleasedEvent(const KeyInputEvent& event);
        /**
         * @brief InputController::handlePressedEvent - Activates the best matching binding.
         *
         * Selects the most specific binding matching the pressed key and current key set, then
         * applies its trigger behavior.
         *
         * @param event The keyboard press event to process.
         */
        void handlePressedEvent(const KeyInputEvent& event);

        /**
         * @brief InputController::findBestBinding - Finds the most specific matching binding.
         *
         * When multiple chords match, prefers the binding with the greatest number of required
         * keys.
         *
         * @param event The keyboard event and pressed-key set to match.
         * @return A pointer to the best binding, or nullptr if no binding matches.
         */
        [[nodiscard]] const Binding* findBestBinding(const KeyInputEvent& event) const;

        /**
         * @brief InputController::activateBinding - Applies the pressed state of a binding.
         *
         * Stores the active chord and modifiers, updates the cached action state according to the
         * trigger type, and emits an action event when appropriate.
         *
         * @param binding The binding being activated.
         * @param event The keyboard event that activated the binding.
         */
        void activateBinding(const Binding& binding, const KeyInputEvent& event);
        /**
         * @brief InputController::releaseBinding - Applies the release behavior of an action.
         *
         * Clears held actions immediately and activates transient OnRelease actions for the current
         * frame.
         *
         * @param action The action associated with the released chord.
         * @param event The keyboard release event that ended the chord.
         */
        void releaseBinding(const StringAtom& action, const KeyInputEvent& event);

        /**
         * @brief InputController::beginInputFrame - Resets transient actions from the previous
         * frame.
         *
         * Clears the cached state and modifiers of OnPress and OnRelease actions after they have
         * been visible for one input frame.
         */
        void beginInputFrame();

        /**
         * @brief InputController::releaseAllActions - Resets all active input state.
         *
         * Marks every known action as released, clears its modifiers, and discards active chords
         * and transient actions. This prevents state from leaking when input routing changes.
         */
        void releaseAllActions();

        InputContext _inputContext = InputContext::Gameplay;

        FIELD();
        std::vector<Binding> _bindings;

        std::unordered_set<StringAtom> _transientActions;

        std::unordered_map<StringAtom, bool> _actionStates;
        std::unordered_map<StringAtom, KeyChord> _activeChords;
        std::unordered_map<StringAtom, InputModifier> _actionModifiers;
        std::unordered_map<StringAtom, ActionCallback> _actionCallbacks;
    };

    /// @brief Serializes an input binding to its asset JSON representation.
    void to_json(nlohmann::json& json, const InputController::Binding& binding);

    /// @brief Deserializes an input binding, including legacy modifier fields.
    void from_json(const nlohmann::json& json, InputController::Binding& binding);
} // namespace Core

#include "InputController.generated.h" // added by the code generator. Better don't move it.
