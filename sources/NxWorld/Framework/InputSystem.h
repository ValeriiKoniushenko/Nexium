// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::World

#pragma once

#include "Core/Delegate.h"
#include "Core/Singleton.h"
#include "NxSubsystems/Input/InputTypes.h"
#include "Platform/Input/Events/AnyInputEvent.h"

#include <vector>

namespace Platform
{
    class Window;
}

namespace NX
{
    class InputController;

    class InputSystem final : public Core::Singleton<InputSystem>
    {
        SINGLETONS_FRIEND(InputSystem);

    public:
        ~InputSystem() override = default;

        void initialize(Platform::Window& window);

        void processEvents();

        [[nodiscard]] std::size_t getPendingEventCount() const noexcept { return _events.size(); }
        [[nodiscard]] const std::vector<AnyInputEvent>& getPendingEvents() const noexcept
        {
            return _events;
        }

        void resetInput();

        void registerController(InputController* controller);
        void unregisterController(InputController* controller);
        [[nodiscard]] bool isRegistered(const InputController* controller) const;

        template<InputEventType T>
        void queueEvent(T&& event)
        {
            _events.emplace_back(std::in_place_type<std::remove_cvref_t<T>>,
                                 std::forward<T>(event));
        }

    private:
        void handle(const KeyboardEvent& event);
        void handle(const TextInputEvent& event);
        void handle(const MouseMoveEvent& event);
        void handle(const MouseButtonEvent& event);
        void handle(const MouseScrollEvent& event);
        void handle(const MouseEnterEvent& event);
        void handle(const GamepadButtonEvent& event);
        void handle(const GamepadAxisEvent& event);
        void handle(const GamepadConnectionEvent& event);
        void handle(const WindowResizeEvent& event);
        void handle(const WindowFocusEvent& event);
        void handle(const WindowIconifyEvent& event);
        void handle(const WindowCloseEvent& event);

        // [[nodiscard]] std::vector<InputController*> selectControllers();

        // [[nodiscard]] std::vector<InputController*>& controllersFor(InputContext context);

        // void refreshRoutedControllers();

        /// Events queued for the next processEvents() call.
        std::vector<AnyInputEvent> _events;
        /// Events being handled by the current processEvents() call. Swapped with _events so
        /// both buffers keep their capacity and steady-state frames do not allocate.
        std::vector<AnyInputEvent> _processing;

        // /// Non-owning pointers. Every controller unregisters itself before destruction.
        // /// Controllers registered for shortcuts that are active while editor UI owns input.
        // std::vector<InputController*> _editorControllers;
        // /// Controllers registered for gameplay actions such as player or spectator movement.
        // std::vector<InputController*> _gameplayControllers;
        // /// Live snapshot of the active context. Context changes rebuild it immediately; events
        // /// are dispatched only to these controllers until the context changes again.
        // std::vector<InputController*> _routedControllers;
        // InputContext _activeContext = InputContext::Editor;

        std::vector<InputButton> _pressedKeys;

        std::vector<InputController*> _controllers;

        Core::DelegateSubscriberPoolGuard _subscriptions;

        bool _initialized = false;
    };

    [[nodiscard]] InputSystem& GetInputSystem();
} // namespace NX
