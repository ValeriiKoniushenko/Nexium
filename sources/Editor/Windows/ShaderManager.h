// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "BaseWindow.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Graphics/ShaderProgram.h"

namespace Core
{
    class ShaderProgramMeta;
}
namespace Core
{

    class TextInput;
    class ComboView;
    class Label;

    class ShaderManagerEWC : public BaseFloatEWC
    {
        ECS_REGISTER_NEW_COMPONENT(ShaderManagerEWC, BaseFloatEWC);

    public:
    protected:
        void createGui();
        void onInitialize() override;
        void onDraw() override;
        void drawList();
        void drawDetails();
        void drawTableWith(
            const char* label,
            const std::unordered_set<ShaderVariable, ShaderVariable::Hasher>& inputData);

        void invalidateShaderCache();
        void openEditor(const std::filesystem::path& path);
        void openEditor(const std::string& path);
        void recompileSelectedShader();
        void selectShader(const StringAtom& name);

    protected:
        VerticalLayout _headLayout;
        VerticalLayout _selectedShaderLayout;

        // =========== GUI ===========
        // general
        IntInput* _totalShaders = nullptr;
        IntInput* _failedShaders = nullptr;
        TextInput* _validExtensions = nullptr;
        ComboView* _comboView = nullptr;
        // selected shader
        TextInput* _shaderName = nullptr;
        TextInput* _fragPath = nullptr;
        TextInput* _vertPath = nullptr;
        Label* _recompileResult = nullptr;
        ShaderProgramMeta* _selectedRawShader = nullptr;

        StringAtom _selectedShader;
        std::size_t _currentItem = 0;
        float _drawDetailsLabelWidth = 140.f;
    };

} // namespace Core
