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

        bool bind(const StringAtom& action, KeyChord chord, ActionCallback callback,
                  InputActionTrigger trigger = InputActionTrigger::OnPress);

        void clearBindings();
        bool unbind(const StringAtom& action);

        void setBindings(const std::vector<Binding>& bindings);

        [[nodiscard]] InputContext getInputContext() const noexcept { return _inputContext; }

        [[nodiscard]] bool isActionPressed(const StringAtom& action) const;

        [[nodiscard]] InputModifier getActionModifiers(const StringAtom& action) const;

        [[nodiscard]] const std::vector<Binding>& getBindings() const noexcept { return _bindings; }

        [[nodiscard]] Tag getTags() const override;

        Delegate<void(const InputActionEvent&)>::Ptr onAction
            = Delegate<void(const InputActionEvent&)>::Create();

    protected:
        void onInitialize() override;

    private:
        friend class InputSystem;

        void handleRoutedEvent(const KeyInputEvent& event);
        void handleReleasedEvent(const KeyInputEvent& event);
        void handlePressedEvent(const KeyInputEvent& event);

        [[nodiscard]] const Binding* findBestBinding(const KeyInputEvent& event) const;

        void activateBinding(const Binding& binding, const KeyInputEvent& event);
        void releaseBinding(const StringAtom& action, const KeyInputEvent& event);

        void beginInputFrame();

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
