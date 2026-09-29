// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
#pragma once

#include "Core/Singleton.h"
#include "NxSubsystems/Input/InputTypes.h"
#include "NxWorld/Framework/InputController.h"

#include <deque>
#include <vector>

namespace Platform
{
    class Window;
}

namespace NX
{

    class InputSystem final : public Core::Singleton<InputSystem>
    {
        SINGLETONS_FRIEND(InputSystem);

    public:
        ~InputSystem() override = default;

        void initialize(Platform::Window& window);

        void processEvents();

        void setActiveContext(InputContext context);

        void resetInput();

        void registerController(InputController* controller);
        void unregisterController(InputController* controller);

    private:
        void pushKeyEvent(Platform::Keyboard::Key key, int scancode,
                          Platform::Keyboard::KeyState state, int mods);

        void dispatch(const KeyInputEvent& event);

        [[nodiscard]] std::vector<InputController*> selectControllers() const;

        [[nodiscard]] std::vector<InputController*>& controllersFor(InputContext context);

        [[nodiscard]] const std::vector<InputController*>& controllersFor(
            InputContext context) const;

        void refreshRoutedControllers();

        std::deque<KeyInputEvent> _events;

        /// Non-owning pointers. Every controller unregisters itself before destruction.
        /// Controllers registered for shortcuts that are active while editor UI owns input.
        std::vector<InputController*> _editorControllers;
        /// Controllers registered for gameplay actions such as player or spectator movement.
        std::vector<InputController*> _gameplayControllers;
        /// Live snapshot of the active context. Context changes rebuild it immediately; events
        /// are dispatched only to these controllers until the context changes again.
        std::vector<InputController*> _routedControllers;
        InputContext _activeContext = InputContext::Editor;

        std::vector<Platform::Keyboard::Key> _pressedKeys;

        Core::DelegateSubscriberPoolGuard _subscriptions;

        bool _initialized = false;
    };

    [[nodiscard]] InputSystem& GetInputSystem();
} // namespace NX
