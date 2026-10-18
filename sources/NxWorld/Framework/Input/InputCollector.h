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
#include "NxSubsystems/Input/InputTypes.h"

#include <deque>
#include <functional>

namespace Platform
{
    class Window;
}

namespace NX
{
    class InputCollector
    {
    public:
        InputCollector() = default;
        void initialize(Platform::Window& window, std::function<void()> onFocusLost);
        void pushKeyEvent(Platform::Keyboard::Key key, int scancode,
                          Platform::Keyboard::KeyState state, int mods);

        void pushMouseEvent(Platform::Mouse::Key button, Platform::Mouse::State state,
                            Platform::Mouse::Mod mods);

        void pushPointerEvent(glm::vec2 position);
        void pushWheelEvent(glm::vec2 offset);

        [[nodiscard]] bool pollEvent(KeyInputEvent& event);
        void reset();

    private:
        static InputModifier ConvertModifiers(int mods);

    private:
        std::deque<KeyInputEvent> _events;
        Core::DelegateSubscriberPoolGuard _subscriptions;
        bool _initialized = false;
    };
} // namespace NX
