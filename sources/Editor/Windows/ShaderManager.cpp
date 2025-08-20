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

#include "ShaderManager.h"

#include "GameplaySystem/Framework/GameInstance.h"
#include "TextEditor.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(ShaderManagerEWC)

    void ShaderManagerEWC::onInit()
    {
        BaseFloatEWC::onInit();
    }

    void ShaderManagerEWC::onDraw()
    {
        drawList();
        ImGui::SameLine();
        drawDetails();
    }

    void ShaderManagerEWC::drawList()
    {
        std::vector<const char*> names;
        for (auto&& shader : gGameInstance->shaderManager.getShaderMetas())
        {
            names.push_back(shader.first.data());
        }

        if (ImGui::BeginListBox("##ShaderList", ImVec2(100, -FLT_MIN)))
        {
            for (std::size_t i = 0; i < names.size(); i++)
            {
                if (ImGui::Selectable(names[i], _currentItem == i))
                {
                    _currentItem = i;
                }
            }
            ImGui::EndListBox();
        }

        _selectedShader = names.at(_currentItem);
    }

    void ShaderManagerEWC::drawDetails()
    {
        auto&& metas = gGameInstance->shaderManager.getShaderMetas();
        if (!metas.contains(_selectedShader))
        {
            return;
        }
        auto&& shader = metas.at(_selectedShader);
        auto&& style = ImGui::GetStyle();
        const auto availWidth = ImGui::GetContentRegionAvail().x - style.ItemSpacing.x * 2.f;

        if (ImGui::BeginChild("Shader's meta data"))
        {
            ImGui::Dummy({}); // top padding

            LabelAndInputTextRO("Shader name:", shader.getShaderName(), _drawDetailsLabelWidth,
                                availWidth);

            if (ImGui::Button("Reload shader",
                              ImVec2(_drawDetailsLabelWidth - style.FramePadding.x * 2.f, 0.f)))
            {
                shader.safeRecreateFromSources();
            }

            const auto vertPath = shader.getVertexShaderPath().generic_string();
            if (ButtonAndInputTextRO("Edit .vert:", vertPath.data(), _drawDetailsLabelWidth,
                                     availWidth))
            {
                gGameInstance->gameEditor.showWindow<TextEditorEWC>(".*", vertPath.data());
            }

            const auto fragPath = shader.getFragmentShaderPath().generic_string();
            if (ButtonAndInputTextRO("Edit .frag:", fragPath.data(), _drawDetailsLabelWidth,
                                     availWidth))
            {
                gGameInstance->gameEditor.showWindow<TextEditorEWC>(".*", fragPath.data());
            }

            ImGui::Dummy({});

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Uniforms", ImGuiTreeNodeFlags_DefaultOpen))
            {
                drawTableWith("Uniforms", shader.getUniforms());
            }
            ImGui::Dummy({});

            if (ImGui::CollapsingHeader("Inputs"))
            {
                drawTableWith("Inputs", shader.getInputs());
            }
            ImGui::Dummy({});

            if (ImGui::CollapsingHeader("Outputs"))
            {
                drawTableWith("Outputs", shader.getOutputs());
            }

            ImGui::Dummy({}); // bottom padding
        }
        ImGui::EndChild();
    }

    void ShaderManagerEWC::drawTableWith(
        const char* label,
        const std::unordered_set<ShaderVariable, ShaderVariable::Hasher>& inputData)
    {
        if (ImGui::BeginTable(label, 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            int i = 0;
            ImGui::TableSetupColumn("#");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Size");
            ImGui::TableSetupColumn("Location");
            ImGui::TableHeadersRow();

            for (auto&& data : inputData)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", i++);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", data.name.data());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d", data.type);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%d", data.size);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%d", data.location);
            }

            ImGui::EndTable();
        }
    }
} // namespace Core
