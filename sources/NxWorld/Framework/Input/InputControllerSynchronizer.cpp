// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputControllerSynchronizer.h"

#include "InputController.h"

#include <array>
#include <cstdint>

namespace NX
{
    namespace
    {
        constexpr auto gcPossibleRoutingChannels
            = std::array{ InputChannel_Keyboard, InputChannel_Pointer };

        std::uint8_t ChannelOf(const KeyInputEvent& event)
        {
            if (event.type != InputEventType::Button)
            {
                return InputChannel_Pointer;
            }
            return event.key.isMouse() ? InputChannel_Pointer : InputChannel_Keyboard;
        }
    } // namespace

    void InputControllerSynchronizer::synchronizeChannels(InputController& controller,
                                                          InputRoutingState& state,
                                                          const InputContextStack& contexts,
                                                          const InputDeviceState& devices) const
    {
        std::uint8_t availableChannels = 0;
        const bool enabled = !InputCapture::isActive() && controller.isInputEnabled();
        const auto context = controller.getInputContext();

        for (const auto channel : gcPossibleRoutingChannels)
        {
            if (enabled && contexts.isAvailable(context, channel))
            {
                availableChannels |= channel;
            }
        }

        state.updateChannels(availableChannels, devices);

        const auto canceledChannels = InputChannel_All & ~availableChannels;
        if (canceledChannels)
        {
            controller.cancelInput(canceledChannels);
        }
    }

    bool InputControllerSynchronizer::isBindingBlocked(
        const InputController& controller, const InputBinding& binding,
        const InputContextStack& contexts, const InputDeviceState& devices,
        const ContextClaimsInput& contextClaimsInput) const
    {
        const KeyInputEvent probe{ .key = binding.chord.triggerKey,
                                   .state = Platform::Keyboard::KeyState::Pressed,
                                   .pressedKeys = devices.getChordKeys(),
                                   .type = binding.source };
        const auto channel = ChannelOf(probe);

        for (const auto& layer : contexts.getLayers())
        {
            if ((layer.channels & channel) == 0)
            {
                continue;
            }
            if (layer.context == controller.getInputContext())
            {
                return false;
            }
            if (layer.policy == InputContextPolicy::BlockLower
                || contextClaimsInput(layer.context, probe))
            {
                return true;
            }
        }
        return true;
    }

    void InputControllerSynchronizer::cancelBlockedBindings(
        InputController& controller, InputRoutingState& state, const InputContextStack& contexts,
        const InputDeviceState& devices, const ContextClaimsInput& contextClaimsInput) const
    {
        // Pointer ownership persists even when no movement binding is active this frame.
        state.cancelMatchingPointerCaptures(
            [&](const InputBinding& binding)
            {
                return isBindingBlocked(controller, binding, contexts, devices, contextClaimsInput);
            },
            devices);
        controller.cancelMatchingBindings(
            [&](const InputBinding& binding)
            {
                if (!isBindingBlocked(controller, binding, contexts, devices, contextClaimsInput))
                {
                    return false;
                }
                state.blockChord(binding.chord, devices);
                return true;
            });
    }
} // namespace NX
