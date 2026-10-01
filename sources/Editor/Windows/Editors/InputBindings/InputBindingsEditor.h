// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BindingsListWidget.h"
#include "Editor/Windows/BaseWindow.h"

namespace NX
{
    CLASS();
    class InputBindingsEditor : public BaseFloatEWC
    {
        ECS_DECL(InputBindingsEditor, NX::BaseFloatEWC);

    public:
    protected:
        void onInitialize() override;

        void onDraw() override;

    private:
        BindingsListWidget _bindingsList;
    };
} // namespace NX

#include "InputBindingsEditor.generated.h" // added by the code generator. Better don't move it.
