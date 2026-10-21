// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "InputDeviceState.h"
#include "InputMap.h"

#include <cstdint>
#include <functional>
#include <span>
#include <vector>

namespace NX
{
    class InputRoutingState
    {
    public:
        [[nodiscard]] bool canReceive(std::uint8_t channel) const;
        [[nodiscard]] bool acceptsEvent(const KeyInputEvent& event, bool pointerCaptured) const;
        [[nodiscard]] KeyInputEvent prepareEvent(const KeyInputEvent& event) const;
        [[nodiscard]] bool hasPointerCapture() const noexcept { return !_pointerCaptures.empty(); }

        void updateChannels(std::uint8_t available, const InputDeviceState& devices);
        void observeDevicesAndEraseReleasedButtons(const InputDeviceState& devices);
        void releaseButton(const KeyInputEvent& event);
        void capturePointer(InputButton button, const InputBinding& binding);
        void cancelMatchingPointerCaptures(
            const std::function<bool(const InputBinding&)>& predicate,
            const InputDeviceState& devices);
        void blockChord(const KeyChord& chord, const InputDeviceState& devices);
        void resetRouting();
        void reset();

    private:
        struct PointerCapture
        {
            InputButton button;
            InputBinding binding;
        };

        void blockButton(InputButton button);
        void blockHeldButtons(std::uint8_t channels, const InputDeviceState& devices);

        std::uint8_t _available{};

        // TODO: split it to two+ classes: keyboard, mouse, joystic, etc
        std::vector<InputButton> _blockedButtons;
        std::vector<PointerCapture> _pointerCaptures;
    };
} // namespace NX
