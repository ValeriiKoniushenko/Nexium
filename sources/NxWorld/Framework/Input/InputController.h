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
#include "InputActionRuntime.h"
#include "NxFundamental/ECS/BaseComponent.h"

#include <functional>
#include <unordered_map>

namespace NX
{
    class InputSystem;
    class InputRouter;
    class InputControllerSynchronizer;

    CLASS();
    class InputController : public BaseComponent
    {
        ECS_DECL(InputController, NX::BaseComponent);

    public:
        // tag::input-controller-api[]
        using ActionCallback = std::function<void(const InputActionEvent&)>;
        using Binding = InputBinding;

        FIELD();
        float speed = 600.f;

        FIELD();
        float mouseSensitivity = 0.5f;

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

        bool bind(const Core::StringAtom& action, const KeyChord& chord,
                  InputActionTrigger trigger = InputActionTrigger::OnPress);
        bool bind(const Core::StringAtom& action, KeyChord chord, ActionCallback callback,
                  InputActionTrigger trigger = InputActionTrigger::OnPress);
        bool bind(Binding binding);

        // Callback registration is independent of the editable bindings.
        // An empty callback unregisters the handler.
        void setActionCallback(const Core::StringAtom& action, ActionCallback callback);
        void clearBindings();
        bool unbind(const Core::StringAtom& action);
        void setBindings(const std::vector<Binding>& bindings);

        void setInputMap(const InputMap& map) { setBindings(map.getBindings()); }
        [[nodiscard]] InputMap getInputMap() const { return InputMap(_bindings); }

        void setInputContext(InputContext context);
        [[nodiscard]] InputContext getInputContext() const noexcept { return _inputContext; }
        void setInputEnabled(bool enabled);
        [[nodiscard]] bool isInputEnabled() const noexcept;

        [[nodiscard]] bool isActive(const Core::StringAtom& action) const;
        [[nodiscard]] bool isActionPressed(const Core::StringAtom& action) const
        {
            return isActive(action);
        }
        [[nodiscard]] std::uint8_t getActionModifiers(const Core::StringAtom& action) const;
        [[nodiscard]] glm::vec2 getActionValue(const Core::StringAtom& action) const;
        [[nodiscard]] const std::vector<Binding>& getBindings() const noexcept { return _bindings; }

        [[nodiscard]] Tag getTags() const override;
        void onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs) override;

        Core::Delegate<void(const InputActionEvent&)>::Ptr onAction
            = Core::Delegate<void(const InputActionEvent&)>::Create();
        // end::input-controller-api[]

    protected:
        void onInitialize() override;
        void onDeinitialize() override;

    private:
        friend class InputSystem;
        friend class InputRouter;
        friend class InputControllerSynchronizer;

        [[nodiscard]] std::optional<Binding> resolveBinding(const KeyInputEvent& event) const;
        void handleRoutedEvent(const KeyInputEvent& event);
        void handleRoutedEvent(const KeyInputEvent& event, const Binding& binding);
        void handleReleasedEvent(const KeyInputEvent& event);
        void beginInputFrame();
        void cancelInput(std::uint8_t channels = InputChannel_All);
        void cancelMatchingBindings(const std::function<bool(const Binding&)>& predicate);
        void releaseAllActions() { cancelInput(); }
        void emitAction(const InputActionEvent& event);

        FIELD();
        InputContext _inputContext = NX::InputContext::Gameplay;

        // Keep the existing asset field name and array representation.
        FIELD();
        std::vector<Binding> _bindings;

        bool _inputEnabled = true;
        InputActionRuntime _runtime;
        std::uint64_t _runtimeRevision = 0;
        std::unordered_map<Core::StringAtom, ActionCallback> _actionCallbacks;
    };
} // namespace NX

#include "InputController.generated.h" // added by the code generator. Better don't move it.