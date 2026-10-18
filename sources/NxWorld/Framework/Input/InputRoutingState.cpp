// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputRoutingState.h"

#include "NxSubsystems/Input/InputTypes.h"

#include <algorithm>

namespace NX
{
    namespace
    {
        InputChannel ChannelOf(InputButton button)
        {
            return button.isMouse() ? InputChannel::Pointer : InputChannel::Keyboard;
        }

        InputChannel ChannelOf(const KeyInputEvent& event)
        {
            return event.type == InputEventType::Button ? ChannelOf(event.key)
                                                        : InputChannel::Pointer;
        }

        bool Contains(std::span<const InputButton> buttons, InputButton button)
        {
            return std::ranges::any_of(
                buttons, [button](InputButton value)
                { return NormalizeInputButton(value) == NormalizeInputButton(button); });
        }
    } // namespace

    bool InputRoutingState::canReceive(InputChannel channel) const
    {
        return hasInputChannel(_available, channel);
    }

    bool InputRoutingState::acceptsEvent(const KeyInputEvent& event, bool pointerCaptured) const
    {
        return canReceive(ChannelOf(event)) && (!pointerCaptured || hasPointerCapture())
               && !(event.isButtonEventType() && Contains(_blockedButtons, event.key));
    }

    KeyInputEvent InputRoutingState::prepareEvent(const KeyInputEvent& event) const
    {
        auto result = event;

        std::erase_if(result.pressedKeys,
                      [this](InputButton button) { return Contains(_blockedButtons, button); });

        return result;
    }

    void InputRoutingState::updateChannels(InputChannel available, const InputDeviceState& devices)
    {
        const auto changedChannels
            = static_cast<std::uint8_t>(available) ^ static_cast<std::uint8_t>(_available);

        if (changedChannels)
        {
            blockHeldButtons(static_cast<InputChannel>(changedChannels), devices);
        }

        _available = available;

        if (!hasInputChannel(_available, InputChannel::Pointer))
        {
            _pointerCaptures.clear();
        }
    }

    void InputRoutingState::observeDevices(const InputDeviceState& devices)
    {
        const auto released = [&devices](InputButton button)
        { return !Contains(devices.getHeldButtons(), button); };

        std::erase_if(_blockedButtons, released);
        std::erase_if(_pointerCaptures, [&released](const PointerCapture& capture)
                      { return released(capture.button); });
    }

    void InputRoutingState::releaseButton(const KeyInputEvent& event)
    {
        if (!Contains(event.pressedKeys, event.key))
        {
            const auto released = [&event](InputButton button)
            { return NormalizeInputButton(button) == event.key; };

            std::erase_if(_blockedButtons, released);
            std::erase_if(_pointerCaptures, [&released](const PointerCapture& capture)
                          { return released(capture.button); });
        }
    }

    void InputRoutingState::capturePointer(InputButton button, const InputBinding& binding)
    {
        if (std::ranges::none_of(_pointerCaptures, [button](const PointerCapture& capture)
                                 { return capture.button == button; }))
        {
            _pointerCaptures.push_back({ .button = button, .binding = binding });
        }
    }

    void InputRoutingState::cancelMatchingPointerCaptures(
        const std::function<bool(const InputBinding&)>& predicate, const InputDeviceState& devices)
    {
        std::vector<KeyChord> blockedChords;
        for (const auto& capture : _pointerCaptures)
        {
            if (predicate(capture.binding))
            {
                blockedChords.push_back(capture.binding.chord);
            }
        }
        for (const auto& chord : blockedChords)
        {
            blockChord(chord, devices);
        }
    }

    void InputRoutingState::blockChord(const KeyChord& chord, const InputDeviceState& devices)
    {
        for (const auto button : devices.getHeldButtons())
        {
            if (chord.contains(button))
            {
                blockButton(button);
            }
        }

        std::erase_if(_pointerCaptures, [&chord](const PointerCapture& capture)
                      { return chord.contains(capture.button); });
    }

    void InputRoutingState::blockButton(InputButton button)
    {
        if (!button.isNone() && !IsInputModifier(button) && !Contains(_blockedButtons, button))
        {
            _blockedButtons.push_back(button);
        }
    }

    void InputRoutingState::blockHeldButtons(InputChannel channels, const InputDeviceState& devices)
    {
        for (const auto button : devices.getHeldButtons())
        {
            if (hasInputChannel(channels, ChannelOf(button)))
            {
                blockButton(button);
            }
        }
    }

    void InputRoutingState::resetRouting()
    {
        _available = InputChannel{};
        _pointerCaptures.clear();
    }

    void InputRoutingState::reset()
    {
        resetRouting();
        _blockedButtons.clear();
    }
} // namespace NX
