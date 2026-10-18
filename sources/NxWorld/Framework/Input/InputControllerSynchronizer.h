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
#include "InputDeviceState.h"
#include "InputMap.h"
#include "InputRoutingState.h"

#include <functional>

namespace NX
{
    class InputController;

    /// Synchronizes a registered controller with the current context stack.
    /// Does not own the controller or its routing state.
    class InputControllerSynchronizer
    {
    public:
        using ContextClaimsInput = std::function<bool(InputContext, const KeyInputEvent&)>;

        void synchronizeChannels(InputController& controller, InputRoutingState& state,
                                 const InputContextStack& contexts,
                                 const InputDeviceState& devices) const;

        void cancelBlockedBindings(InputController& controller, InputRoutingState& state,
                                   const InputContextStack& contexts,
                                   const InputDeviceState& devices,
                                   const ContextClaimsInput& contextClaimsInput) const;

    private:
        [[nodiscard]] bool isBindingBlocked(const InputController& controller,
                                            const InputBinding& binding,
                                            const InputContextStack& contexts,
                                            const InputDeviceState& devices,
                                            const ContextClaimsInput& contextClaimsInput) const;
    };
} // namespace NX
