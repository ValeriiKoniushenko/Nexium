// Nexium
// Copyright 2018-2026 Valerii Koniushenko
// Licensed under the Apache License, Version 2.0.

#pragma once

#include "Core/Singleton.h"
#include "InputCollector.h"
#include "InputContextStack.h"
#include "InputDeviceState.h"
#include "InputRouter.h"

namespace NX
{
    class InputSystem;

    // tag::input-context-guard-api[]
    class InputContextGuard
    {
    public:
        InputContextGuard() = default;
        InputContextGuard(const InputContextGuard&) = delete;
        InputContextGuard& operator=(const InputContextGuard&) = delete;
        InputContextGuard(InputContextGuard&& other) noexcept;
        InputContextGuard& operator=(InputContextGuard&& other) noexcept;
        ~InputContextGuard();
        void reset();
        // end::input-context-guard-api[]

    private:
        friend class InputSystem;
        InputContextGuard(InputSystem& owner, InputContextStack::LayerId id)
            : _owner(&owner),
              _id(id)
        {
        }
        InputSystem* _owner = nullptr;
        InputContextStack::LayerId _id = 0;
    };

    class InputSystem final : public Core::Singleton<InputSystem>
    {
        SINGLETONS_FRIEND(InputSystem);

    public:
        // tag::input-system-api[]
        ~InputSystem() override = default;
        void initialize(Platform::Window& window);
        void processEvents();
        void setActiveContext(InputContext context);
        void setGlobalContext(std::optional<InputContext> context);
        [[nodiscard]] InputContextGuard pushContext(InputContextLayer layer);
        void resetInput();
        void registerController(InputController* controller);
        void unregisterController(InputController* controller);
        void refreshController(InputController* controller);
        [[nodiscard]] bool hasPointerCapture() const { return _router.hasPointerCapture(); }
        // end::input-system-api[]

    private:
        friend class InputContextGuard;
        void popContext(InputContextStack::LayerId id);

        InputCollector _collector;
        InputDeviceState _devices;
        InputContextStack _contexts;
        InputRouter _router;
    };

    [[nodiscard]] InputSystem& GetInputSystem();
} // namespace NX
