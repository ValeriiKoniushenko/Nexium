// Nexium
// Copyright 2018-2026 Valerii Koniushenko
// Licensed under the Apache License, Version 2.0.

#include "InputRouter.h"

#include "InputController.h"

#include <algorithm>
#include <array>

namespace NX
{
    namespace
    {
        constexpr auto RoutedChannels = std::array{ InputChannel::Keyboard, InputChannel::Pointer };

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

    bool InputRouter::Registration::canReceive(InputChannel channel) const
    {
        return hasInputChannel(_available, channel) && handle.controller->isInputEnabled();
    }

    bool InputRouter::Registration::acceptsEvent(const KeyInputEvent& event,
                                                 bool pointerCaptured) const
    {
        return canReceive(ChannelOf(event)) && (!pointerCaptured || hasPointerCapture())
               && !(event.isButtonEventType() && Contains(_blockedButtons, event.key));
    }

    KeyInputEvent InputRouter::Registration::prepareEvent(const KeyInputEvent& event) const
    {
        auto result = event;
        std::erase_if(result.pressedKeys,
                      [this](InputButton button) { return Contains(_blockedButtons, button); });
        return result;
    }

    void InputRouter::Registration::updateChannels(InputChannel available,
                                                   const InputDeviceState& devices)
    {
        for (const auto channel : RoutedChannels)
        {
            if (hasInputChannel(available, channel) != hasInputChannel(_available, channel))
            {
                // Require a fresh press when a channel becomes available or unavailable.
                blockHeldButtons(channel, devices);
            }
        }
        _available = available;
        if (!hasInputChannel(_available, InputChannel::Pointer))
        {
            _pointerButtons.clear();
        }
    }

    void InputRouter::Registration::observeDevices(const InputDeviceState& devices)
    {
        const auto released = [&devices](InputButton button)
        { return !Contains(devices.getHeldButtons(), button); };
        std::erase_if(_blockedButtons, released);
        std::erase_if(_pointerButtons, released);
    }

    void InputRouter::Registration::releaseButton(const KeyInputEvent& event)
    {
        if (!Contains(event.pressedKeys, event.key))
        {
            const auto released = [&event](InputButton button)
            { return NormalizeInputButton(button) == event.key; };
            std::erase_if(_blockedButtons, released);
            std::erase_if(_pointerButtons, released);
        }
    }

    void InputRouter::Registration::capturePointer(InputButton button)
    {
        if (!Contains(_pointerButtons, button))
        {
            _pointerButtons.push_back(button);
        }
    }

    void InputRouter::Registration::blockButton(InputButton button)
    {
        // Modifiers remain usable with a fresh trigger after a routing change.
        if (!button.isNone() && !IsInputModifier(button) && !Contains(_blockedButtons, button))
        {
            _blockedButtons.push_back(button);
        }
    }

    void InputRouter::Registration::blockHeldButtons(InputChannel channels,
                                                     const InputDeviceState& devices)
    {
        for (const auto button : devices.getHeldButtons())
        {
            if (hasInputChannel(channels, ChannelOf(button)))
            {
                blockButton(button);
            }
        }
    }

    void InputRouter::Registration::blockChord(const KeyChord& chord,
                                               const InputDeviceState& devices)
    {
        for (const auto button : devices.getHeldButtons())
        {
            if (chord.contains(button))
            {
                blockButton(button);
            }
        }
        std::erase_if(_pointerButtons,
                      [&chord](InputButton button) { return chord.contains(button); });
    }

    void InputRouter::Registration::resetRouting()
    {
        _available = InputChannel{};
        _pointerButtons.clear();
    }

    void InputRouter::Registration::reset()
    {
        resetRouting();
        _blockedButtons.clear();
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

    std::vector<InputRouter::Handle> InputRouter::snapshot() const
    {
        std::vector<Handle> result;
        for (const auto& entry : _controllers)
        {
            result.push_back(entry.handle);
        }
        return result;
    }

    void InputRouter::synchronize(Handle handle, const InputContextStack& contexts,
                                  const InputDeviceState& devices)
    {
        if (auto* entry = find(handle))
        {
            synchronizeChannels(*entry, contexts, devices);
            cancelBlockedBindings(*entry, contexts, devices);
        }
    }

    void InputRouter::synchronizeChannels(Registration& registration,
                                          const InputContextStack& contexts,
                                          const InputDeviceState& devices)
    {
        auto availableChannels = std::uint8_t{};
        auto canceledChannels = std::uint8_t{};
        auto& controller = *registration.handle.controller;
        const bool inputEnabled = !InputCapture::isActive() && controller.isInputEnabled();
        const auto context = controller.getInputContext();

        for (const auto channel : RoutedChannels)
        {
            const bool available = inputEnabled && contexts.isAvailable(context, channel);
            if (available)
            {
                availableChannels |= static_cast<std::uint8_t>(channel);
            }
            else
            {
                canceledChannels |= static_cast<std::uint8_t>(channel);
            }
        }

        registration.updateChannels(static_cast<InputChannel>(availableChannels), devices);
        if (canceledChannels)
        {
            controller.cancelInput(static_cast<InputChannel>(canceledChannels));
        }
    }

    void InputRouter::cancelBlockedBindings(Registration& registration,
                                            const InputContextStack& contexts,
                                            const InputDeviceState& devices)
    {
        registration.handle.controller->cancelMatchingBindings(
            [&](const InputBinding& binding)
            {
                if (!isBindingBlocked(registration.handle, binding, contexts, devices))
                {
                    return false;
                }
                registration.blockChord(binding.chord, devices);
                return true;
            });
    }

    bool InputRouter::isBindingBlocked(Handle handle, const InputBinding& binding,
                                       const InputContextStack& contexts,
                                       const InputDeviceState& devices) const
    {
        const KeyInputEvent probe{ .key = binding.chord.triggerKey,
                                   .state = Platform::Keyboard::KeyState::Pressed,
                                   .pressedKeys = devices.getChordKeys(),
                                   .type = binding.source };
        const auto channel = ChannelOf(probe);
        for (const auto& layer : contexts.getLayers())
        {
            if (!hasInputChannel(layer.channels, channel))
            {
                continue;
            }
            if (layer.context == handle.controller->getInputContext())
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
            if (const auto pointer = controller.resolveBinding(pointerProbe))
            {
                input.capturePointer = pointer->chord.contains(event.key);
            }
        }
        return input;
    }

    void InputRouter::synchronize(const InputContextStack& contexts,
                                  const InputDeviceState& devices)
    {
        observeDevices(devices);
        for (const Handle handle : snapshot())
        {
            synchronize(handle, contexts, devices);
        }
    }

    /// @brief Removes released buttons from input quarantine and pointer capture.
    void InputRouter::observeDevices(const InputDeviceState& devices)
    {
        for (auto& entry : _controllers)
        {
            entry.observeDevices(devices);
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
            it->resetRouting();
            synchronize(handle, contexts, devices);
        }
    }

    void InputRouter::beginFrame(const InputContextStack& contexts, const InputDeviceState& devices)
    {
        synchronize(contexts, devices);
        for (const auto handle : snapshot())
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
            routeReleasedEvent(event, snapshot());
            return;
        }

        if ((event.isButtonEventType() && event.state != Platform::Keyboard::KeyState::Pressed)
            || (event.isPointerMoveEventType() && event.value == glm::vec2{}))
        {
            return;
        }

        const InputChannel channel = ChannelOf(event);
        const auto handles = snapshot();
        for (const auto& layer : contexts.getLayers())
        {
            if (!hasInputChannel(layer.channels, channel))
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
                entry->releaseButton(event);
                const auto routed = entry->prepareEvent(event);
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
            if (!entry || !entry->acceptsEvent(event, capturedPointer)
                || handle.controller->getInputContext() != context)
            {
                continue;
            }
            auto routed = entry->prepareEvent(event);
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
                                         InputContext context, InputChannel channel,
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
            if (!entry || !entry->canReceive(channel) || !contexts.isAvailable(context, channel)
                || entry->revision != delivery.revision)
            {
                continue;
            }
            if (delivery.input.capturePointer)
            {
                entry->capturePointer(delivery.event.key);
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
                                   { return entry.hasPointerCapture(); });
    }

    void InputRouter::cancelAll()
    {
        for (const auto handle : snapshot())
        {
            if (auto* entry = find(handle))
            {
                entry->resetRouting();
                handle.controller->cancelInput();
            }
        }
    }

    void InputRouter::reset()
    {
        cancelAll();
        for (auto& entry : _controllers)
        {
            entry.reset();
        }
    }
} // namespace NX
