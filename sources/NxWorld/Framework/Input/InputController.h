// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "NxFundamental/ECS/BaseComponent.h"
#include "Platform/Input/Events/InputEvent.h"

namespace NxWorld::Framework::Input
{

    struct InputDevice
    {
        NX::InputDeviceType type;
        NX::InputDeviceId id;
    };

    struct KeyChord
    {
        NX::InputButton triggerKey;
        std::vector<NX::InputButton> keys;
    };

    struct Binding
    {
        NX::StringAtom action;
        KeyChord chord;
        NX::InputState trigger;
    };

    class InputController : public NX::BaseComponent
    {
    public:
        InputController();

    private:
    };

} // namespace NxWorld::Framework::Input