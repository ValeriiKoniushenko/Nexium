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
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Core
{
    namespace Internal
    {
        class InputSystem;
    }

    CLASS();
    /// @brief ECS component that maps routed keyboard chords to named gameplay actions.
    class InputController : public BaseComponent
    {
        ECS_DECL(InputController, Core::BaseComponent);

    public:
        /// @brief Serializable association between an action, its chord and activation mode.
        struct Binding
        {
            /// @brief Semantic action name used by gameplay code.
            StringAtom action;

            /// @brief Keyboard combination that activates the action.
            KeyChord chord;

            /// @brief Defines whether the action pulses or remains active while held.
            InputActionTrigger trigger = InputActionTrigger::OnPress;
        };

        /// @brief Adds a binding or replaces the binding with the same action name.
        bool bind(const StringAtom& action, KeyChord chord,
                  InputActionTrigger trigger = InputActionTrigger::OnPress);

        /// @brief Removes the binding and runtime state associated with an action.
        bool unbind(const StringAtom& action);

        /// @brief Releases active actions and removes every binding.
        void clearBindings();

        /// @brief Replaces all bindings with a copied collection.
        void setBindings(const std::vector<Binding>& bindings);

        /// @brief Replaces all bindings with a movable collection.
        void setBindings(std::vector<Binding>&& bindings);

        /// @brief Explicitly enables or disables input handling for this controller.
        void setInputActive(bool active);

        /// @brief Returns the effective state including ECS enablement and the predicate.
        [[nodiscard]] bool isInputActive() const;

        /// @brief Controls whether unmatched keyboard events continue to lower controllers.
        void setBlockingPolicy(InputBlockingPolicy policy) noexcept { _blockingPolicy = policy; }

        /// @brief Returns the current unmatched-event blocking policy.
        [[nodiscard]] InputBlockingPolicy getBlockingPolicy() const noexcept
        {
            return _blockingPolicy;
        }

        /// @brief Sets a runtime activation condition such as focus or an opened menu.
        void setActivationPredicate(std::function<bool()> predicate)
        {
            _activationPredicate = std::move(predicate);
        }

        /// @brief Returns whether an action is active in the current input frame.
        [[nodiscard]] bool isActionPressed(const StringAtom& action) const;

        /// @brief Returns keyboard modifiers captured when the action was activated.
        [[nodiscard]] InputModifier getActionModifiers(const StringAtom& action) const;

        /// @brief Returns the serialized bindings owned by this controller.
        [[nodiscard]] const std::vector<Binding>& getBindings() const noexcept { return _bindings; }

        /// @brief Adds the InputController tag to the base component tags.
        [[nodiscard]] Tag getTags() const override;

        /// @brief Triggered when an action reaches the phase selected by its binding.
        Delegate<void(const InputActionEvent&)>::Ptr onAction
            = Delegate<void(const InputActionEvent&)>::Create();

    protected:
        /// @brief Registers the initialized component in the engine input router.
        void onInitialize() override;

    private:
        friend class Internal::InputSystem;

        /// @brief Applies a routed key event and reports whether it was consumed.
        [[nodiscard]] InputResult handleRoutedEvent(const KeyInputEvent& event);

        /// @brief Returns the size of the most specific chord matching an event.
        [[nodiscard]] std::optional<std::size_t> getMatchingSpecificity(
            const KeyInputEvent& event) const;

        /// @brief Clears actions that were configured as one-frame pulses.
        void beginInputFrame();

        /// @brief Detects effective activation transitions and updates router order.
        void refreshActiveState();

        /// @brief Clears all active, held and transient action state.
        void releaseAllActions();

        FIELD();
        /// @brief Serializable action bindings configured for this component.
        std::vector<Binding> _bindings;

        /// @brief Chords currently started and waiting for one of their keys to be released.
        std::unordered_map<StringAtom, KeyChord> _activeChords;

        /// @brief Current action values exposed through isActionPressed().
        std::unordered_map<StringAtom, bool> _actionStates;

        /// @brief Keyboard modifiers captured when each action was activated.
        std::unordered_map<StringAtom, InputModifier> _actionModifiers;

        /// @brief One-frame OnPress/OnRelease actions reset on the next input frame.
        std::unordered_set<StringAtom> _transientActions;

        /// @brief Optional runtime condition such as viewport focus or an opened menu.
        std::function<bool()> _activationPredicate;

        /// @brief Internal activation sequence used to route equal shortcuts newest-first.
        std::uint64_t _activationOrder = 0;

        /// @brief Defines whether unmatched keyboard events may reach lower controllers.
        InputBlockingPolicy _blockingPolicy = InputBlockingPolicy::Passthrough;

        /// @brief Explicit user-controlled input activation flag.
        bool _inputActive = true;

        /// @brief Cached effective state used to detect activation and deactivation transitions.
        bool _wasEffectivelyActive = true;
    };

    /// @brief Serializes an input binding to its asset JSON representation.
    void to_json(nlohmann::json& json, const InputController::Binding& binding);

    /// @brief Deserializes an input binding, including legacy modifier fields.
    void from_json(const nlohmann::json& json, InputController::Binding& binding);

} // namespace Core

#include "InputController.generated.h" // added by the code generator. Better don't move it.
