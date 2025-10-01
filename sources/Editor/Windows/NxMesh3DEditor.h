/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "AssetsManager/Mesh3DAsset.h"
#include "BaseWindow.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Graphics/Texture.h"

namespace Core
{
    namespace Gui
    {
        class Label;
        class ComboModelBased;
    } // namespace Gui

    class NxMesh3DEditorEWC : public BaseFloatEWC
    {
        ECS_COMPONENT_DECL(NxMesh3DEditorEWC, BaseFloatEWC);

    public:
        [[nodiscard]] const char* getIcon() override { return ICON_FA_COG; }

        void save();
        void openFromFileSystem();
        void discardChanges();

    protected:
        void fetchFromAssetsManager();
        void onInitialize() override;
        void onDraw() override;
        void drawBarMenu();
        void setEnabledStatusForAllProps(bool isEnabled);
        [[nodiscard]] std::size_t convertShaderNameToIndex(const StringAtom& shaderName) const;

    protected:
        Gui::VerticalLayout _layout;
        std::filesystem::path _filePath;

        Gui::TextInput* _logicalPathInput = nullptr;
        Gui::TextInput* _modelInput = nullptr;
        Gui::ComboModelBased* _mainShaderCombo = nullptr;
        Gui::ComboModelBased* _outlineShaderCombo = nullptr;
        Gui::FloatInput* _scaleInput = nullptr;

        bool _isModified = false;

        NXMesh3D _targetMesh;
    };
} // namespace Core