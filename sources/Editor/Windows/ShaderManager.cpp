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

#include "Editor/Configs.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/Combo.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/Spacer.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "TextEditor.h"

namespace Core
{
    ECS_COMPONENT_IMPL(ShaderManagerEWC)

    void ShaderManagerEWC::createGui()
    {
        constexpr float defaultLabelWidth = 140.0f;

        // ================ HEAD LAYOUT ======================
        {
            // Count of valid
            {
                auto* l = _headLayout.addChildComponent<HorizontalLayout>();
                l->setFlex(Widget::Flex::FlexWidth);

                auto* label = l->addChildComponent<Label>();
                label->setText("Total shaders");
                label->setWidth(defaultLabelWidth);

                _totalShaders = l->addChildComponent<IntInput>();
                _totalShaders->setDisabled(true);
                _totalShaders->setFlex(Widget::Flex::FlexWidth);
            }

            // Count of invalid
            {
                auto* l = _headLayout.addChildComponent<HorizontalLayout>();
                l->setFlex(Widget::Flex::FlexWidth);

                auto* label = l->addChildComponent<Label>();
                label->setText("With errors");
                label->setWidth(defaultLabelWidth);

                _failedShaders = l->addChildComponent<IntInput>();
                _failedShaders->setDisabled(true);
                _failedShaders->setFlex(Widget::Flex::FlexWidth);
            }
        }

        {
            auto* ext = _headLayout.addChildComponent<HorizontalLayout>();
            auto* label = ext->addChildComponent<Label>();
            label->setText("Valid extensions");
            label->setWidth(defaultLabelWidth);

            _validExtensions = ext->addChildComponent<TextInput>();
            _validExtensions->setReadOnly(true);
            _validExtensions->setFlex(Widget::Flex::FlexWidth);
        }

        {
            auto* ext = _headLayout.addChildComponent<HorizontalLayout>();
            auto* label = ext->addChildComponent<Label>();
            label->setText("Root shader dir");
            label->setWidth(defaultLabelWidth);

            auto* input = ext->addChildComponent<TextInput>();
            input->setReadOnly(true);
            input->setFlex(Widget::Flex::FlexWidth);
            input->setInputtedData(GetShaderManager().getInputDir().generic_string());
        }

        {
            auto* shaderSelect = _headLayout.addChildComponent<HorizontalLayout>();
            auto* label = shaderSelect->addChildComponent<Label>("Shader");
            label->setWidth(defaultLabelWidth);

            _comboView = shaderSelect->addChildComponent<ComboView>();
            _comboView->setFlex(Widget::Flex::FlexWidth);
            _comboView->onSelect.subscribe(
                [this](ComboView*, StringAtom key)
                {
                    selectShader(key);
                });
        }

        // ================ SELECTED SHADER ======================
        {
            auto* name = _selectedShaderLayout.addChildComponent<HorizontalLayout>();
            auto* label = name->addChildComponent<Label>("Shader name");
            label->setWidth(defaultLabelWidth);

            _shaderName = name->addChildComponent<TextInput>();
            _shaderName->setReadOnly(true);
            _shaderName->setFlex(Widget::Flex::FlexWidth);
        }

        {
            auto* path = _selectedShaderLayout.addChildComponent<HorizontalLayout>();
            auto* label = path->addChildComponent<Label>("Path to .vert");
            label->setWidth(defaultLabelWidth);

            _vertPath = path->addChildComponent<TextInput>();
            _vertPath->setReadOnly(true);
            _vertPath->setFlex(Widget::Flex::FlexWidth);

            auto* edit = path->addChildComponent<Button>("Edit");
            edit->onClick.subscribe(
                [this](Button*)
                {
                    if (_vertPath)
                    {
                        openEditor(_vertPath->getInputtedData());
                    }
                });
        }

        {
            auto* path = _selectedShaderLayout.addChildComponent<HorizontalLayout>();
            auto* label = path->addChildComponent<Label>("Path to .frag");
            label->setWidth(defaultLabelWidth);

            _fragPath = path->addChildComponent<TextInput>();
            _fragPath->setReadOnly(true);
            _fragPath->setFlex(Widget::Flex::FlexWidth);

            auto* edit = path->addChildComponent<Button>("Edit");
            edit->onClick.subscribe(
                [this](Button*)
                {
                    if (_fragPath)
                    {
                        openEditor(_fragPath->getInputtedData());
                    }
                });
        }

        _selectedShaderLayout.addChildComponent<Spacer>();

        {
            auto* recompile = _selectedShaderLayout.addChildComponent<Button>("Recompile");
            recompile->setFlex(Widget::Flex::FlexWidth);
            recompile->onClick.subscribe(
                [this](Button*)
                {
                    recompileSelectedShader();
                });

            _recompileResult = _selectedShaderLayout.addChildComponent<Label>();
            _recompileResult->setFlex(Widget::Flex::FlexWidth);
            _recompileResult->setText("");
        }

        _selectedShaderLayout.addChildComponent<Spacer>();
    }

    void ShaderManagerEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        createGui();
        invalidateShaderCache();
        if (_comboView)
        {
            selectShader(_comboView->getSelectedString());
        }
    }

    void ShaderManagerEWC::onDraw()
    {
        const float dt = gGameInstance->world.timeDelta;
        _headLayout.tick(dt);

        ImGui::Dummy({ 0, ImGui::GetStyle().ItemSpacing.y * 2.f });
        if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _selectedShaderLayout.tick(dt);
        }

        if (_selectedRawShader)
        {
            if (ImGui::CollapsingHeader("Uniforms", ImGuiTreeNodeFlags_DefaultOpen))
            {
                drawTableWith("Uniforms", _selectedRawShader->getUniforms());
            }
            ImGui::Dummy({});

            if (ImGui::CollapsingHeader("Inputs", ImGuiTreeNodeFlags_DefaultOpen))
            {
                drawTableWith("Inputs", _selectedRawShader->getInputs());
            }
            ImGui::Dummy({});

            if (ImGui::CollapsingHeader("Outputs", ImGuiTreeNodeFlags_DefaultOpen))
            {
                drawTableWith("Outputs", _selectedRawShader->getOutputs());
            }
        }

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
        if (_comboView)
        {
            std::vector<StringAtom> shaders;
            shaders.reserve(gGameInstance->shaderManager.getShaderMetas().size());
            for (auto&& shader : gGameInstance->shaderManager.getShaderMetas())
            {
                shaders.push_back(shader.first.data());
            }
            std::ranges::sort(shaders);
            _comboView->setData(shaders);
        }

        if (_validExtensions)
        {
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
        }

        if (_totalShaders)
        {
            _totalShaders->setInputtedData(static_cast<int>(GetShaderManager().countOfShaders()));
        }

        if (_failedShaders)
        {
            _failedShaders->setInputtedData(static_cast<int>(GetShaderManager().countOfFailedShaders()));
        }
    }

    void ShaderManagerEWC::openEditor(const std::filesystem::path& path)
    {
        openEditor(path.generic_string());
    }

    void ShaderManagerEWC::openEditor(const std::string& path)
    {
        GetEditor().showWindow<TextEditorEWC>(".*", path.data());
    }

    void ShaderManagerEWC::recompileSelectedShader()
    {
        if (!_shaderName)
        {
            return;
        }

        const auto name = StringAtom(_shaderName->getInputtedData());
        if (name.isEmpty())
        {
            return;
        }

        auto&& metas = GetShaderManager().getShaderMetas();
        if (Verify(metas.contains(name)))
        {
            bool result = metas[name].safeRecreateFromSources();

            if (_recompileResult)
            {
                if (result)
                {
                    _recompileResult->setTextColor(Config::ColorSoftGreen);
                    _recompileResult->setText("Successfully recompiled");
                }
                else
                {
                    _recompileResult->setTextColor(Config::ColorRed);
                    _recompileResult->setText("Recompile failed. Check logs for details.");
                }
            }
        }
    }

    void ShaderManagerEWC::selectShader(const StringAtom& name)
    {
        auto&& metas = GetShaderManager().getShaderMetas();
        if (!Verify(metas.contains(name)))
        {
            errorLog("Selected shader: {} - not found."_f << name);
            return;
        }

        auto&& shader = metas[name];
        if (_shaderName)
        {
            _shaderName->setInputtedData(shader.getShaderName().toStdString());
        }

        if (_fragPath)
        {
            _fragPath->setInputtedData(shader.getFragmentShaderPath().generic_string());
        }

        if (_vertPath)
        {
            _vertPath->setInputtedData(shader.getVertexShaderPath().generic_string());
        }

        _selectedRawShader = &shader;
    }
} // namespace Core
