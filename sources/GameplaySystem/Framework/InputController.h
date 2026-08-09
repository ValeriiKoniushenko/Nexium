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
    class InputController : public BaseComponent
    {
        ECS_DECL(InputController, Core::BaseComponent);

    public:
        struct Binding
        {
            StringAtom action;
            KeyChord chord;
            InputActionTrigger trigger = InputActionTrigger::OnPress;
        };

        bool bind(const StringAtom& action, KeyChord chord,
                  InputActionTrigger trigger = InputActionTrigger::OnPress);
        bool unbind(const StringAtom& action);
        void clearBindings();
        void setBindings(const std::vector<Binding>& bindings);
        void setBindings(std::vector<Binding>&& bindings);

        void setInputActive(bool active);
        [[nodiscard]] bool isInputActive() const;

        void setBlockingPolicy(InputBlockingPolicy policy) noexcept { _blockingPolicy = policy; }
        [[nodiscard]] InputBlockingPolicy getBlockingPolicy() const noexcept
        {
            return _blockingPolicy;
        }

        // Useful for editor focus, menus and other runtime-only activation conditions.
        void setActivationPredicate(std::function<bool()> predicate)
        {
            _activationPredicate = std::move(predicate);
        }

        [[nodiscard]] bool isActionPressed(const StringAtom& action) const;
        [[nodiscard]] InputModifier getActionModifiers(const StringAtom& action) const;
        [[nodiscard]] const std::vector<Binding>& getBindings() const noexcept { return _bindings; }
        [[nodiscard]] Tag getTags() const override;

        Delegate<void(const InputActionEvent&)>::Ptr onAction
            = Delegate<void(const InputActionEvent&)>::Create();

    protected:
        void onInitialize() override;

    private:
        friend class Internal::InputSystem;

        [[nodiscard]] InputResult handleRoutedEvent(const KeyInputEvent& event);
        [[nodiscard]] std::optional<std::size_t> getMatchingSpecificity(
            const KeyInputEvent& event) const;
        void beginInputFrame();
        void refreshActiveState();
        void releaseAllActions();

        FIELD();
        std::vector<Binding> _bindings;
        std::unordered_map<StringAtom, KeyChord> _activeChords;
        std::unordered_map<StringAtom, bool> _actionStates;
        std::unordered_map<StringAtom, InputModifier> _actionModifiers;
        std::unordered_set<StringAtom> _transientActions;
        std::function<bool()> _activationPredicate;
        std::uint64_t _activationOrder = 0;
        InputBlockingPolicy _blockingPolicy = InputBlockingPolicy::Passthrough;
        bool _inputActive = true;
        bool _wasEffectivelyActive = true;
    };

    void to_json(nlohmann::json& json, const InputController::Binding& binding);
    void from_json(const nlohmann::json& json, InputController::Binding& binding);

} // namespace Core

#include "InputController.generated.h" // added by the code generator. Better don't move it.
