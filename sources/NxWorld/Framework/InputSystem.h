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
#include "NxWorld/Framework/Input/InputActionTypes.h"
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

        /// Discards every pending event and resynchronizes with the window's focus and iconified
        /// state. It does not reset controllers.
        void resetInput();

        /// False while the window is unfocused or minimized: new presses, mouse movement, scroll,
        /// and gamepad input are then ignored. Releases are still delivered.
        [[nodiscard]] bool isWindowActive() const noexcept
        {
            return _isWindowFocused && !_isWindowIconified;
        }

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
        void syncWindowState();
        void setWindowState(bool focused, bool iconified);
        void routeButton(InputSource source, std::uint16_t code, InputState state,
                         InputModifier mods);

        /// Offers an event to the controllers from the highest priority down. `deliver` returns
        /// whether a controller has a binding for the event. If `consumable`, the first enabled,
        /// consuming controller that does keeps every lower priority from seeing the event;
        /// controllers of the same priority still all receive it.
        template<typename Deliver>
        void route(bool consumable, Deliver&& deliver);

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

        /// Events queued for the next processEvents() call.
        std::vector<AnyInputEvent> _events;
        /// Events being handled by the current processEvents() call. Swapped with _events so
        /// both buffers keep their capacity and steady-state frames do not allocate.
        std::vector<AnyInputEvent> _processing;

        std::vector<InputController*> _controllers;
        /// _controllers by descending priority, rebuilt every processEvents().
        std::vector<InputController*> _ordered;
        /// Snapshot of _controllers walked while callbacks run. Kept as a member so it keeps
        /// its capacity between frames.
        std::vector<InputController*> _dispatching;

        Core::DelegateSubscriberPoolGuard _subscriptions;

        Platform::Window* _window = nullptr;
        bool _isWindowFocused = true;
        bool _isWindowIconified = false;

        bool _initialized = false;
    };

    [[nodiscard]] InputSystem& GetInputSystem();
} // namespace NX
