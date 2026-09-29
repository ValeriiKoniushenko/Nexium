// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Delegate.h"
#include "NxFundamental/ECS/BaseComponent.h"
#include "NxSubsystems/Input/InputTypes.h"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace NX
{
    class InputSystem;

    CLASS();
    class InputController : public BaseComponent
    {
        ECS_DECL(InputController, NX::BaseComponent);

    public:
        using ActionCallback = std::function<void(const InputActionEvent&)>;

        InputController(const Core::StringAtom& name, InputContext context)
            : InputController(name)
        {
            _inputContext = context;
        }
        InputController(const InputController& other);
        InputController& operator=(const InputController&) = delete;
        InputController(InputController&&) = delete;
        InputController& operator=(InputController&&) = delete;

        ~InputController() override;

        [[nodiscard]] static Ptr Create(const Core::StringAtom& name, InputContext context);

        struct Binding
        {
            Core::StringAtom action;
            KeyChord chord;
            InputActionTrigger trigger = InputActionTrigger::OnPress;
        };

        bool bind(const Core::StringAtom& action, const KeyChord& chord,
                  InputActionTrigger trigger = InputActionTrigger::OnPress);

        bool bind(const Core::StringAtom& action, KeyChord chord, ActionCallback callback,
                  InputActionTrigger trigger = InputActionTrigger::OnPress);

        void clearBindings();

        bool unbind(const Core::StringAtom& action);

        void setBindings(const std::vector<Binding>& bindings);

        [[nodiscard]] InputContext getInputContext() const noexcept { return _inputContext; }

        [[nodiscard]] bool isActionPressed(const Core::StringAtom& action) const;

        [[nodiscard]] InputModifier getActionModifiers(const Core::StringAtom& action) const;

        [[nodiscard]] const std::vector<Binding>& getBindings() const noexcept { return _bindings; }

        [[nodiscard]] Tag getTags() const override;

        void onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs) override;

        Core::Delegate<void(const InputActionEvent&)>::Ptr onAction
            = Core::Delegate<void(const InputActionEvent&)>::Create();

    protected:
        void onInitialize() override;

    private:
        friend class InputSystem;

        void handleRoutedEvent(const KeyInputEvent& event);
        void handleReleasedEvent(const KeyInputEvent& event);
        void handlePressedEvent(const KeyInputEvent& event);

        [[nodiscard]] const Binding* findBestBinding(const KeyInputEvent& event) const;

        void activateBinding(const Binding& binding, const KeyInputEvent& event);
        void releaseBinding(const Core::StringAtom& action, const KeyInputEvent& event);

        void beginInputFrame();

        void releaseAllActions();

    private:
        InputContext _inputContext = InputContext::Gameplay;

        FIELD();
        std::vector<Binding> _bindings;

        std::unordered_set<Core::StringAtom> _transientActions;

        std::unordered_map<Core::StringAtom, bool> _actionStates;
        std::unordered_map<Core::StringAtom, KeyChord> _activeChords;
        std::unordered_map<Core::StringAtom, InputModifier> _actionModifiers;
        std::unordered_map<Core::StringAtom, ActionCallback> _actionCallbacks;
    };

    /// @brief Serializes an input binding to its asset JSON representation.
    void to_json(nlohmann::json& json, const InputController::Binding& binding);

    /// @brief Deserializes an input binding, including legacy modifier fields.
    void from_json(const nlohmann::json& json, InputController::Binding& binding);
} // namespace NX

#include "InputController.generated.h" // added by the code generator. Better don't move it.
