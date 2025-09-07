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

#include "Editor/GuiComponents/Combo.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/Spacer.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "TextEditor.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(ShaderManagerEWC)

    void ShaderManagerEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        constexpr float defaultLabelWidth = 140.0f;

        // ================ HEAD LAYOUT ======================
        {
            // Count of valid
            {
                auto* l = _headLayout.addChildComponent<HorizontalLayout>();
                l->setFlex(Widget::Flex::FlexWidth);

                auto* label = l->addChildComponent<Label>();
                label->setText("Total shaders:");
                label->setWidth(defaultLabelWidth);

                _totalShaders = l->addChildComponent<IntInput>();
                _totalShaders->setIsDisabled(true);
                _totalShaders->setFlex(Widget::Flex::FlexWidth);
            }

            // Count of invalid
            {
                auto* l = _headLayout.addChildComponent<HorizontalLayout>();
                l->setFlex(Widget::Flex::FlexWidth);

                auto* label = l->addChildComponent<Label>();
                label->setText("With errors:");
                label->setWidth(defaultLabelWidth);

                _failedShaders = l->addChildComponent<IntInput>();
                _failedShaders->setIsDisabled(true);
                _failedShaders->setFlex(Widget::Flex::FlexWidth);
            }
        }

        {
            auto* ext = _headLayout.addChildComponent<HorizontalLayout>();
            auto* label = ext->addChildComponent<Label>();
            label->setText("Valid extensions:");
            label->setWidth(defaultLabelWidth);

            _validExtensions = ext->addChildComponent<TextInput>();
            _validExtensions->setReadOnly(true);
            _validExtensions->setFlex(Widget::Flex::FlexWidth);
        }

        {
            auto* ext = _headLayout.addChildComponent<HorizontalLayout>();
            auto* label = ext->addChildComponent<Label>();
            label->setText("Root shader dir:");
            label->setWidth(defaultLabelWidth);

            auto* input = ext->addChildComponent<TextInput>();
            input->setReadOnly(true);
            input->setFlex(Widget::Flex::FlexWidth);
            input->setInputtedData(GetShaderManager().getInputDir().generic_string());
        }

        {
            auto* shaderSelect = _headLayout.addChildComponent<HorizontalLayout>();
            auto* label = shaderSelect->addChildComponent<Label>("Shader:");
            label->setWidth(defaultLabelWidth);

            auto* combo = shaderSelect->addChildComponent<ComboModelBased>();
            combo->setFlex(Widget::Flex::FlexWidth);
            combo->setDataProvider(
                [this](std::size_t i, StringAtom& label) -> void*
                {
                    label = _cachedShader.at(i);
                    return nullptr;
                });
            combo->setSizeProvider(
                [this]() -> std::size_t
                {
                    return _cachedShader.size();
                });
        }

        // ================ SHADER SELECT ======================

        invalidateShaderCache();
    }

    void ShaderManagerEWC::onDraw()
    {
        const float dt = gGameInstance->world.timeDelta;
        _headLayout.tick(dt);

        if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _generalLayout.tick(dt);
        }

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

        if (ImGui::BeginListBox("##ShaderList", glm::vec2(100, -FLT_MIN)))
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
        const auto availWidth = ImGui::GetContentRegionAvail().x - (style.ItemSpacing.x * 2.f);

        if (ImGui::BeginChild("Shader's meta data"))
        {
            ImGui::Dummy({}); // top padding

            LabelAndInputTextRO("Shader name:", shader.getShaderName(), _drawDetailsLabelWidth,
                                availWidth);

            if (ImGui::Button(
                    "Reload shader",
                    glm::vec2(_drawDetailsLabelWidth - (style.FramePadding.x * 2.f), 0.f)))
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
            ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Location", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

            for (auto&& data : inputData)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", i++);

                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(data.name.data());

                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(glTypeToString(data.type).data());

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%d", data.size);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%d", data.location);
            }

            ImGui::EndTable();
        }
    }

    void ShaderManagerEWC::invalidateShaderCache()
    {
        _cachedShader.clear();
        _cachedShader.reserve(gGameInstance->shaderManager.getShaderMetas().size());
        for (auto&& shader : gGameInstance->shaderManager.getShaderMetas())
        {
            _cachedShader.push_back(shader.first.data());
        }
        std::ranges::sort(_cachedShader);

        std::string extensions;
        for (auto&& extension : GetShaderManager().getSuitableFragFileExtensions())
        {
            extensions += extension;
            extensions.push_back(' ');
        }
        for (auto&& extension : GetShaderManager().getSuitableVertFileExtensions())
        {
            extensions += extension;
            extensions.push_back(' ');
        }

        while (!extensions.empty() && extensions.back() == ' ')
        {
            extensions.pop_back();
        }
        _validExtensions->setInputtedData(extensions);

        _totalShaders->setInputtedData(GetShaderManager().countOfShaders());
        _failedShaders->setInputtedData(GetShaderManager().countOfFailedShaders());
    }
} // namespace Core
