// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Platform::Input

#pragma once

#include "GamepadEvents.h"
#include "KeyboardEvents.h"
#include "MouseEvents.h"
#include "WindowEvents.h"

#include <type_traits>
#include <variant>

namespace NX
{

    // tag::input_any_event[]
    using AnyInputEvent
        = std::variant<KeyboardEvent, TextInputEvent, MouseMoveEvent, MouseButtonEvent,
                       MouseScrollEvent, MouseEnterEvent, GamepadButtonEvent, GamepadAxisEvent,
                       GamepadConnectionEvent, WindowResizeEvent, WindowFocusEvent,
                       WindowIconifyEvent, WindowCloseEvent>;

    /// Events are copied into the per-frame queue; keep them small and trivially copyable.
    static_assert(sizeof(AnyInputEvent) <= 32, "Input events must stay small");
    static_assert(std::is_trivially_copyable_v<AnyInputEvent>,
                  "Input events must be trivially copyable");
    // end::input_any_event[]

    namespace Detail
    {
        template<typename T, typename Variant>
        struct IsVariantAlternative : std::false_type
        {
        };

        template<typename T, typename... Ts>
        struct IsVariantAlternative<T, std::variant<Ts...>> :
            std::bool_constant<(std::is_same_v<T, Ts> || ...)>
        {
        };
    } // namespace Detail

    /// Satisfied by every event type that can be stored in AnyInputEvent.
    template<typename T>
    concept InputEventType
        = Detail::IsVariantAlternative<std::remove_cvref_t<T>, AnyInputEvent>::value;

} // namespace NX
