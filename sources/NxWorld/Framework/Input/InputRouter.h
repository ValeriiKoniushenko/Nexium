// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "InputContextStack.h"
#include "InputControllerSynchronizer.h"
#include "InputDeviceState.h"
#include "InputMap.h"
#include "InputRoutingState.h"

#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace NX
{
    class InputController;

    class InputRouter
    {
    public:
        void registerController(InputController* controller);
        void unregisterController(InputController* controller);
        void refreshController(InputController* controller, const InputContextStack& contexts,
                               const InputDeviceState& devices);
        void synchronize(const InputContextStack& contexts, const InputDeviceState& devices);
        void observeDevicesAndEraseReleasedButtons(const InputDeviceState& devices);
        void beginFrame(const InputContextStack& contexts, const InputDeviceState& devices);
        void route(const KeyInputEvent& event, const InputContextStack& contexts,
                   const InputDeviceState& devices);
        void cancelAll();
        void reset();
        [[nodiscard]] bool hasPointerCapture() const;

    private:
        struct Handle
        {
            InputController* controller = nullptr;
            std::uint64_t id = 0;
        };

        struct Registration
        {
            explicit Registration(Handle registeredHandle);

            Handle handle;
            std::uint64_t revision = 0;
            InputRoutingState state;
        };

        struct ResolvedInput
        {
            std::optional<InputBinding> binding;
            std::optional<InputBinding> pointerBinding{};
            [[nodiscard]] bool hasMatch() const;
        };

        struct PendingDelivery
        {
            Handle handle;
            KeyInputEvent event;
            ResolvedInput input;
            std::uint64_t revision = 0;
        };

        [[nodiscard]] static bool canReceive(const Registration& registration,
                                             std::uint8_t channel);
        [[nodiscard]] static bool acceptsEvent(const Registration& registration,
                                               const KeyInputEvent& event, bool pointerCaptured);
        [[nodiscard]] Registration* find(Handle handle);
        [[nodiscard]] const Registration* find(Handle handle) const;
        [[nodiscard]] std::vector<Handle> inputControllerSnapshot() const;
        void synchronize(Handle handle, const InputContextStack& contexts,
                         const InputDeviceState& devices);
        [[nodiscard]] bool contextClaimsInput(InputContext context,
                                              const KeyInputEvent& event) const;
        [[nodiscard]] static ResolvedInput resolveInput(const InputController& controller,
                                                        const KeyInputEvent& event);
        void routeReleasedEvent(const KeyInputEvent& event, std::span<const Handle> handles);
        [[nodiscard]] std::vector<PendingDelivery> collectDeliveries(
            const KeyInputEvent& event, InputContext context,
            std::span<const Handle> handles) const;
        void dispatchDeliveries(std::span<const PendingDelivery> deliveries, InputContext context,
                                std::uint8_t channel, const InputContextStack& contexts);

        InputControllerSynchronizer _synchronizer;
        std::vector<Registration> _controllers;
        std::uint64_t _nextId = 1;
    };
} // namespace NX
