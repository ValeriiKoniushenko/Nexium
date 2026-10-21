// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputSystem.h"

#include "NxSubsystems/Input/InputTypes.h"

#include <utility>

namespace NX
{
    InputContextGuard::InputContextGuard(InputContextGuard&& other) noexcept
        : _owner(std::exchange(other._owner, nullptr)),
          _id(std::exchange(other._id, 0))
    {
    }

    InputContextGuard& InputContextGuard::operator=(InputContextGuard&& other) noexcept
    {
        if (this != &other)
        {
            reset();
            _owner = std::exchange(other._owner, nullptr);
            _id = std::exchange(other._id, 0);
        }
        return *this;
    }

    InputContextGuard::~InputContextGuard()
    {
        reset();
    }

    void InputContextGuard::reset()
    {
        if (auto* owner = std::exchange(_owner, nullptr))
        {
            owner->popContext(std::exchange(_id, 0));
        }
    }

    void InputSystem::initialize(Platform::Window& window)
    {
        _collector.initialize(window, [this] { resetInput(); });
    }

    void InputSystem::processEvents()
    {
        _router.beginFrame(_contexts, _devices);

        KeyInputEvent raw;

        while (_collector.pollEvent(raw))
        {
            if (raw.type == InputEventType::PointerMove)
            {
                continue;
            }

            if (raw.type == InputEventType::Button)
            {
                InputCapture::updateButton(raw.key, raw.state);
            }

            const KeyInputEvent event = _devices.apply(std::move(raw));

            if (InputCapture::isActive())
            {
                // Captured events bypass route(), which normally observes the device state.
                _router.observeDevicesAndEraseReleasedButtons(_devices);
                _router.cancelAll();
            }
            else
            {
                _router.route(event, _contexts, _devices);
            }
        }
    }

    void InputSystem::setActiveContext(InputContext context)
    {
        if (_contexts.setActiveContext(context))
        {
            _router.synchronize(_contexts, _devices);
        }
    }

    void InputSystem::setGlobalContext(std::optional<InputContext> context)
    {
        if (_contexts.setGlobalContext(context))
        {
            _router.synchronize(_contexts, _devices);
        }
    }

    InputContextGuard InputSystem::pushContext(InputContextLayer layer)
    {
        const auto id = _contexts.push(layer);
        _router.synchronize(_contexts, _devices);
        return { *this, id };
    }

    void InputSystem::popContext(InputContextStack::LayerId id)
    {
        if (_contexts.pop(id))
        {
            _router.synchronize(_contexts, _devices);
        }
    }

    void InputSystem::registerController(InputController* controller)
    {
        _router.registerController(controller);
        _router.refreshController(controller, _contexts, _devices);
    }

    void InputSystem::unregisterController(InputController* controller)
    {
        _router.unregisterController(controller);
    }

    void InputSystem::refreshController(InputController* controller)
    {
        _router.refreshController(controller, _contexts, _devices);
    }

    void InputSystem::resetInput()
    {
        _collector.reset();
        _devices.reset();
        InputCapture::resetButtons();
        _router.reset();
    }

    InputSystem& GetInputSystem()
    {
        return InputSystem::Instance();
    }
} // namespace NX
