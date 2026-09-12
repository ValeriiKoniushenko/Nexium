// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Editor/Windows/BaseWindow.h"

namespace Core
{
    CLASS();
    class TextEditorEWC : public BaseFloatEWC
    {
        ECS_DECL(TextEditorEWC, Core::BaseFloatEWC);

    public:
        void putArguments(const StringAtom& args) override;

        [[nodiscard]] const char* getIcon() override;

    protected:
        void onInitialize() override;

        void onDraw() override;

        void drawBarMenu();

        void drawEditor();

        void save();

    protected:
        std::filesystem::path _path;
        std::string _fileContent;
        bool _wasEdited = false;
        float _statusMarginRight = 10.f;
    };
} // namespace Core

#include "TextEditor.generated.h" // added by the code generator. Better don't move it.
