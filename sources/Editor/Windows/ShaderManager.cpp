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

            ImGui::Dummy({});ImGui::SameLine(); // left
            LabelAndInputTextRO("Shader name:", shader.getShaderName(), 100.f, availWidth);

            ImGui::Dummy({}); // bottom padding
        }
        ImGui::EndChild();
    }
} // namespace Core
