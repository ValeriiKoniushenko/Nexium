// Nexium
// Copyright 2018-2026 Valerii Koniushenko
// Licensed under the Apache License, Version 2.0.

#pragma once

#include "InputContextStack.h"
#include "InputDeviceState.h"
#include "InputMap.h"

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
        void observeDevices(const InputDeviceState& devices);
        void beginFrame(const InputContextStack& contexts, const InputDeviceState& devices);
        void route(const KeyInputEvent& event, const InputContextStack& contexts,
                   const InputDeviceState& devices);
        void cancelAll();
        void reset();
        [[nodiscard]] bool hasPointerCapture() const;

    private:
        struct Handle
        {
            InputController* controller;
            std::uint64_t id;
        };

        struct Registration
        {
            explicit Registration(Handle registeredHandle)
                : handle(registeredHandle)
            {
            }

            [[nodiscard]] bool canReceive(InputChannel channel) const;
            [[nodiscard]] bool acceptsEvent(const KeyInputEvent& event, bool pointerCaptured) const;
            [[nodiscard]] KeyInputEvent prepareEvent(const KeyInputEvent& event) const;
            [[nodiscard]] bool hasPointerCapture() const { return !_pointerButtons.empty(); }

            void updateChannels(InputChannel available, const InputDeviceState& devices);
            void observeDevices(const InputDeviceState& devices);
            void releaseButton(const KeyInputEvent& event);
            void capturePointer(InputButton button);
            void blockChord(const KeyChord& chord, const InputDeviceState& devices);
            void resetRouting();
            void reset();

            Handle handle;
            std::uint64_t revision = 0;

        private:
            void blockButton(InputButton button);
            void blockHeldButtons(InputChannel channels, const InputDeviceState& devices);

            InputChannel _available{};
            std::vector<InputButton> _blockedButtons;
            std::vector<InputButton> _pointerButtons;
        };

        struct ResolvedInput
        {
            std::optional<InputBinding> binding;
            bool capturePointer = false;

            [[nodiscard]] bool hasMatch() const { return binding.has_value() || capturePointer; }
        };

        struct PendingDelivery
        {
            Handle handle;
            KeyInputEvent event;
            ResolvedInput input;
            std::uint64_t revision = 0;
        };

        [[nodiscard]] Registration* find(Handle handle);
        [[nodiscard]] const Registration* find(Handle handle) const;
        [[nodiscard]] std::vector<Handle> snapshot() const;
        void synchronize(Handle handle, const InputContextStack& contexts,
                         const InputDeviceState& devices);
        void synchronizeChannels(Registration& registration, const InputContextStack& contexts,
                                 const InputDeviceState& devices);
        void cancelBlockedBindings(Registration& registration, const InputContextStack& contexts,
                                   const InputDeviceState& devices);
        [[nodiscard]] bool isBindingBlocked(Handle handle, const InputBinding& binding,
                                            const InputContextStack& contexts,
                                            const InputDeviceState& devices) const;
        [[nodiscard]] bool contextClaimsInput(InputContext context,
                                              const KeyInputEvent& event) const;
        [[nodiscard]] static ResolvedInput resolveInput(const InputController& controller,
                                                        const KeyInputEvent& event);

        /// @brief Releases previously accepted actions across registered controllers.
        void routeReleasedEvent(const KeyInputEvent& event, std::span<const Handle> handles);
        /// @brief Selects all recipients in one context before invoking callbacks.
        [[nodiscard]] std::vector<PendingDelivery> collectDeliveries(
            const KeyInputEvent& event, InputContext context,
            std::span<const Handle> handles) const;
        /// @brief Revalidates selected recipients and delivers their prepared events.
        void dispatchDeliveries(std::span<const PendingDelivery> deliveries, InputContext context,
                                InputChannel channel, const InputContextStack& contexts);

        std::vector<Registration> _controllers;
        std::uint64_t _nextId = 1;
    };
} // namespace NX
