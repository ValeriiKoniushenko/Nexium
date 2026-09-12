// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseWindow.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "NxSubsystems/Graphics/ShaderProgram.h"

namespace Core
{
    class ShaderProgramMeta;
}

namespace Core
{
    namespace Gui
    {
        template<Utils::IsArithmetic T>
        class NumInput;

        using IntInput = NumInput<int>;

        class TextInput;
        class ComboView;
        class Label;
    } // namespace Gui

    CLASS();
    class ShaderManagerEWC : public BaseFloatEWC
    {
        ECS_DECL(ShaderManagerEWC, Core::BaseFloatEWC);

    public:
        [[nodiscard]] const char* getIcon() override;

    protected:
        void createGui();

        void onInitialize() override;

        void onDraw() override;

        void drawTableWith(
            const char* label,
            const std::unordered_set<ShaderVariable, ShaderVariable::Hasher>& inputData);

        void drawUBOs(const std::unordered_set<ShaderUBO, ShaderUBO::Hasher>& ubos);
        void invalidateShaderCache();

        void openEditor(const std::filesystem::path& path);

        void openEditor(const std::string& path);

        void recompileSelectedShader();

        void selectShader(const StringAtom& name);

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;

        Gui::VerticalLayout _headLayout;
        Gui::VerticalLayout _selectedShaderLayout;

        // =========== GUI ===========
        // general
        Gui::IntInput* _totalShaders = nullptr;
        Gui::IntInput* _failedShaders = nullptr;
        Gui::TextInput* _validExtensions = nullptr;
        Gui::ComboView* _comboView = nullptr;
        // selected shader
        Gui::TextInput* _shaderName = nullptr;
        Gui::TextInput* _fragPath = nullptr;
        Gui::TextInput* _vertPath = nullptr;
        Gui::Label* _recompileResult = nullptr;

        ShaderProgramMeta* _selectedRawShader = nullptr;

        StringAtom _selectedShader;
        std::size_t _currentItem = 0;
        float _drawDetailsLabelWidth = 140.f;
    };
} // namespace Core

#include "ShaderManager.generated.h" // added by the code generator. Better don't move it.
