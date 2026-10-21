// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputRouter.h"

#include "InputController.h"

#include <algorithm>

namespace NX
{
    namespace
    {
        std::uint8_t ChannelOf(InputButton button)
        {
            return button.isMouse() ? InputChannel_Pointer : InputChannel_Keyboard;
        }

        std::uint8_t ChannelOf(const KeyInputEvent& event)
        {
            return event.type == InputEventType::Button ? ChannelOf(event.key)
                                                        : std::uint8_t{ InputChannel_Pointer };
        }
    } // namespace

    bool InputRouter::canReceive(const Registration& registration, std::uint8_t channel)
    {
        return registration.handle.controller->isInputEnabled()
               && registration.state.canReceive(channel);
    }

    bool InputRouter::acceptsEvent(const Registration& registration, const KeyInputEvent& event,
                                   bool pointerCaptured)
    {
        return registration.handle.controller->isInputEnabled()
               && registration.state.acceptsEvent(event, pointerCaptured);
    }

    void InputRouter::registerController(InputController* controller)
    {
        if (controller
            && std::ranges::none_of(_controllers, [controller](const Registration& entry)
                                    { return entry.handle.controller == controller; }))
        {
            _controllers.emplace_back(Handle{ controller, _nextId++ });
        }
    }

    void InputRouter::unregisterController(InputController* controller)
    {
        std::erase_if(_controllers, [controller](const Registration& entry)
                      { return entry.handle.controller == controller; });
    }

    InputRouter::Registration* InputRouter::find(Handle handle)
    {
        const auto it = std::ranges::find_if(
            _controllers,
            [handle](const Registration& entry)
            {
                return entry.handle.id == handle.id && entry.handle.controller == handle.controller;
            });
        return it == _controllers.end() ? nullptr : &*it;
    }

    const InputRouter::Registration* InputRouter::find(Handle handle) const
    {
        const auto it = std::ranges::find_if(
            _controllers,
            [handle](const Registration& entry)
            {
                return entry.handle.id == handle.id && entry.handle.controller == handle.controller;
            });
        return it == _controllers.end() ? nullptr : &*it;
    }

    std::vector<InputRouter::Handle> InputRouter::inputControllerSnapshot() const
    {
        std::vector<Handle> result;
        result.reserve(_controllers.size());
        for (const auto& entry : _controllers)
        {
            result.push_back(entry.handle);
        }
        return result;
    }

    void InputRouter::synchronize(Handle handle, const InputContextStack& currentContext,
                                  const InputDeviceState& currentDevices)
    {
        if (auto* entry = find(handle))
        {
            _synchronizer.synchronizeChannels(*handle.controller, entry->state, currentContext,
                                              currentDevices);
        }

        // Re-find the registration: synchronization can invalidate cached addresses.
        if (auto* current = find(handle))
        {
            _synchronizer.cancelBlockedBindings(
                *handle.controller, current->state, currentContext, currentDevices,
                [this](InputContext context, const KeyInputEvent& event)
                { return contextClaimsInput(context, event); });
        }
    }

    bool InputRouter::contextClaimsInput(InputContext context, const KeyInputEvent& event) const
    {
        return std::ranges::any_of(_controllers,
                                   [context, &event](const Registration& entry)
                                   {
                                       const auto& controller = *entry.handle.controller;
                                       return controller.isInputEnabled()
                                              && controller.getInputContext() == context
                                              && resolveInput(controller, event).hasMatch();
                                   });
    }

    InputRouter::ResolvedInput InputRouter::resolveInput(const InputController& controller,
                                                         const KeyInputEvent& event)
    {
        ResolvedInput input{ .binding = controller.resolveBinding(event) };
        if (event.isButtonEventType() && event.key.isMouse())
        {
            auto pointerProbe = event;
            pointerProbe.type = InputEventType::PointerMove;
            if (const auto pointer = controller.resolveBinding(pointerProbe);
                pointer && pointer->chord.contains(event.key))
            {
                input.pointerBinding = pointer;
            }
        }
        return input;
    }

    void InputRouter::synchronize(const InputContextStack& contexts,
                                  const InputDeviceState& devices)
    {
        observeDevicesAndEraseReleasedButtons(devices);
        for (const Handle handle : inputControllerSnapshot())
        {
            synchronize(handle, contexts, devices);
        }
    }

    /// @brief Removes released buttons from input quarantine and pointer capture.
    void InputRouter::observeDevicesAndEraseReleasedButtons(const InputDeviceState& devices)
    {
        for (auto& entry : _controllers)
        {
            entry.state.observeDevicesAndEraseReleasedButtons(devices);
        }
    }

    void InputRouter::refreshController(InputController* controller,
                                        const InputContextStack& contexts,
                                        const InputDeviceState& devices)
    {
        const auto it = std::ranges::find_if(_controllers, [controller](const Registration& entry)
                                             { return entry.handle.controller == controller; });
        if (it != _controllers.end())
        {
            const auto handle = it->handle;
            ++it->revision;
            it->state.resetRouting();
            synchronize(handle, contexts, devices);
        }
    }

    void InputRouter::beginFrame(const InputContextStack& contexts, const InputDeviceState& devices)
    {
        synchronize(contexts, devices);
        for (const auto handle : inputControllerSnapshot())
        {
            if (find(handle))
            {
                handle.controller->beginInputFrame();
            }
        }
    }

    void InputRouter::route(const KeyInputEvent& event, const InputContextStack& contexts,
                            const InputDeviceState& devices)
    {
        synchronize(contexts, devices);
        if (event.isButtonEventType() && event.state == Platform::Keyboard::KeyState::Released)
        {
            routeReleasedEvent(event, inputControllerSnapshot());
            return;
        }

        if ((event.isButtonEventType() && event.state != Platform::Keyboard::KeyState::Pressed)
            || (event.isPointerMoveEventType() && event.value == glm::vec2{}))
        {
            return;
        }

        const std::uint8_t channel = ChannelOf(event);
        const auto handles = inputControllerSnapshot();
        for (const auto& layer : contexts.getLayers())
        {
            if ((layer.channels & channel) == 0)
            {
                continue;
            }
            const auto deliveries = collectDeliveries(event, layer.context, handles);
            if (!deliveries.empty())
            {
                dispatchDeliveries(deliveries, layer.context, channel, contexts);
                return;
            }
            if (layer.policy == InputContextPolicy::BlockLower)
            {
                return;
            }
        }
    }

    void InputRouter::routeReleasedEvent(const KeyInputEvent& event,
                                         std::span<const Handle> handles)
    {
        for (const auto handle : handles)
        {
            if (auto* entry = find(handle))
            {
                entry->state.releaseButton(event);
                const auto routed = entry->state.prepareEvent(event);
                // The runtime only releases bindings which accepted an earlier press.
                handle.controller->handleReleasedEvent(routed);
            }
        }
    }

    std::vector<InputRouter::PendingDelivery> InputRouter::collectDeliveries(
        const KeyInputEvent& event, InputContext context, std::span<const Handle> handles) const
    {
        const bool capturedPointer = event.isPointerMoveEventType() && hasPointerCapture();
        std::vector<PendingDelivery> deliveries;
        for (const auto handle : handles)
        {
            const auto* entry = find(handle);
            if (!entry || !acceptsEvent(*entry, event, capturedPointer)
                || handle.controller->getInputContext() != context)
            {
                continue;
            }
            auto routed = entry->state.prepareEvent(event);
            auto input = resolveInput(*handle.controller, routed);
            if (input.hasMatch())
            {
                deliveries.push_back(
                    { handle, std::move(routed), std::move(input), entry->revision });
            }
        }
        return deliveries;
    }

    void InputRouter::dispatchDeliveries(std::span<const PendingDelivery> deliveries,
                                         InputContext context, std::uint8_t channel,
                                         const InputContextStack& contexts)
    {
        for (const auto& delivery : deliveries)
        {
            if (InputCapture::isActive())
            {
                cancelAll();
                return;
            }
            auto* entry = find(delivery.handle);
            if (!entry || !canReceive(*entry, channel) || !contexts.isAvailable(context, channel)
                || delivery.handle.controller->getInputContext() != context
                || entry->revision != delivery.revision)
            {
                continue;
            }
            if (delivery.input.pointerBinding)
            {
                entry->state.capturePointer(delivery.event.key, *delivery.input.pointerBinding);
            }
            if (delivery.input.binding)
            {
                InputController::Ptr keepAlive = delivery.handle.controller->getHardRefCount()
                                                     ? delivery.handle.controller
                                                     : nullptr;
                delivery.handle.controller->handleRoutedEvent(delivery.event,
                                                              *delivery.input.binding);
            }
        }
    }

    bool InputRouter::hasPointerCapture() const
    {
        return std::ranges::any_of(_controllers, [](const Registration& entry)
                                   { return entry.state.hasPointerCapture(); });
    }

    void InputRouter::cancelAll()
    {
        for (const auto handle : inputControllerSnapshot())
        {
            if (auto* entry = find(handle))
            {
                entry->state.resetRouting();
                handle.controller->cancelInput();
            }
        }
    }

    void InputRouter::reset()
    {
        cancelAll();
        for (auto& entry : _controllers)
        {
            entry.state.reset();
        }
    }

    InputRouter::Registration::Registration(Handle registeredHandle)
        : handle(registeredHandle)
    {
    }

    bool InputRouter::ResolvedInput::hasMatch() const
    {
        return binding.has_value() || pointerBinding.has_value();
    }
} // namespace NX
