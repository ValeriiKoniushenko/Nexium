// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseController.h"
#include "InputController.h"

namespace NX
{
    /// Editor controller with its own input component and no gameplay possession.
    // tag::input-editor-api[]
    CLASS();
    class EditorController : public BaseController
    {
        ECS_DECL(EditorController, NX::BaseController);

    public:
        [[nodiscard]] InputController& getInput();
        [[nodiscard]] const InputController& getInput() const;

    protected:
        void onInitialize() override;
    };
    // end::input-editor-api[]
} // namespace NX

#include "EditorController.generated.h" // added by the code generator. Better don't move it.
