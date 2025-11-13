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

#include "EditorStaticMeshBundleAdapter.h"

using namespace Core::Gui;

namespace Core
{
    ECS_COMPONENT_IMPL(ECSEditorStaticMeshBundleAdapter)

    void ECSEditorStaticMeshBundleAdapter::onInitialize()
    {
        ECSEditorMimeAdapter::onInitialize();

        constexpr float defaultLabelWidth = 120.0f;

        auto shaderDataProvider = [](std::size_t inputIndex, StringAtom& out) -> const void*
        {
            auto it = GetShaderManager().getShaderMetas().begin();
            std::advance(it, inputIndex);
            if (it == GetShaderManager().getShaderMetas().end())
            {
                Assert(false, "Internal error, can't get specified shader");
                return nullptr;
            }

            out = it->first;
            return nullptr;
        };
        auto shaderSizeProvider = [] { return GetShaderManager().getShaderMetas().size(); };

        _layout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

        _modelInput
            = _layout.addChildComponent<LabelRow<TextInput>>("Model path", defaultLabelWidth);
        _modelInput->input->setFlex(Flex::FlexWidth);
        _subscriptionPool << _modelInput->input->onInput->subscribeAndGetID([this](auto)
                                                                            { makeParentDirty(); });

        _mainShaderCombo = _layout.addChildComponent<LabelRow<ComboModelBased>>("Main shader",
                                                                                defaultLabelWidth);
        _mainShaderCombo->input->setFlex(Flex::FlexWidth);
        _mainShaderCombo->input->setDataProvider(shaderDataProvider);
        _mainShaderCombo->input->setSizeProvider(shaderSizeProvider);
        _subscriptionPool << _mainShaderCombo->input->onSelect->subscribeAndGetID(
            [this](auto) { makeParentDirty(); });

        _outlineShaderCombo = _layout.addChildComponent<LabelRow<ComboModelBased>>(
            "Outline shader", defaultLabelWidth);
        _outlineShaderCombo->input->setFlex(Flex::FlexWidth);
        _outlineShaderCombo->input->setDataProvider(shaderDataProvider);
        _outlineShaderCombo->input->setSizeProvider(shaderSizeProvider);
        _subscriptionPool << _outlineShaderCombo->input->onSelect->subscribeAndGetID(
            [this](auto) { makeParentDirty(); });

        _onLoadScale
            = _layout.addChildComponent<LabelRow<FloatInput>>("On load scale", defaultLabelWidth);
        _onLoadScale->input->setFlex(Flex::FlexWidth);
        _onLoadScale->input->setMin(0.0f);
        _onLoadScale->input->setStep(0.1f);
        _subscriptionPool << _onLoadScale->input->onInput->subscribeAndGetID(
            [this](auto) { makeParentDirty(); });

        _postProcessArray = _layout.addChildComponent<LabelRow<AssimpPostProcessArray>>(
            "P.Loading flags", defaultLabelWidth);
        _postProcessArray->setVerticalAlign(Align::Top);
        _postProcessArray->input->setFlex(Flex::FlexWidth);
        _subscriptionPool << _postProcessArray->input->onChange->subscribeAndGetID(
            [this]() { makeParentDirty(); });
        _subscriptionPool << _postProcessArray->input->onSave->subscribeAndGetID(
            [this](const std::vector<aiPostProcessSteps>& data)
            {
                _postProcessFlags = 0;
                for (auto el : data)
                {
                    _postProcessFlags |= el;
                }
            });
    }

    void ECSEditorStaticMeshBundleAdapter::onApplyAssetData(const nlohmann::json& json)
    {
        /*auto* comp = getTargetComponent()->tryCastTo<StaticMeshBundle>();
        if (!Verify(comp)) [[unlikely]]
        {
            warnLog("Can't cast component to StaticMeshBundle, but it must be cast!");
            return;
        }*/

        if (_modelInput && json.contains("path"))
        {
            _modelInput->input->setInputtedData(json["path"].get<StringAtom>().toStdString());
        }
        if (_mainShaderCombo && json.contains("mainShader"))
        {
            auto str = json["mainShader"].get<StringAtom>();
            _mainShaderCombo->input->setCurrentIndex(convertShaderNameToIndex(str));
        }
        if (_outlineShaderCombo && json.contains("outlineShader"))
        {
            auto str = json["outlineShader"].get<StringAtom>();
            _outlineShaderCombo->input->setCurrentIndex(convertShaderNameToIndex(str));
        }
        if (_onLoadScale && json.contains("onLoadScale"))
        {
            _onLoadScale->input->setInputtedData(json["onLoadScale"].get<float>());
        }
        if (_postProcessArray && json.contains("assimpPostProcess"))
        {
            _postProcessArray->input->clearData(true);
            for (auto el : json["assimpPostProcess"])
            {
                auto flag = Assimp::aiPostProcessStepsFromString(el.get<StringAtom>());
                if (flag)
                {
                    _postProcessArray->input->add(*flag, true);
                }
            }
        }
    }

    void ECSEditorStaticMeshBundleAdapter::onDraw(float dt)
    {
        if (Gui::CollapsingHeader("Static mesh bundle", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _layout.tick(dt);
        }
    }

    bool ECSEditorStaticMeshBundleAdapter::canWorkWith(BaseComponent* component) const
    {
        return dynamic_cast<StaticMeshBundle*>(component) != nullptr;
    }

    std::size_t ECSEditorStaticMeshBundleAdapter::convertShaderNameToIndex(
        const StringAtom& shaderName) const
    {
        const auto it = GetShaderManager().getShaderMetas().find(shaderName);
        return std::distance(GetShaderManager().getShaderMetas().begin(), it);
    }

    StringAtom ECSEditorStaticMeshBundleAdapter::convertIndexToShaderName(std::size_t index) const
    {
        auto it = GetShaderManager().getShaderMetas().begin();
        std::advance(it, index);
        Assert(it != GetShaderManager().getShaderMetas().end());
        return it->first;
    }
} // namespace Core