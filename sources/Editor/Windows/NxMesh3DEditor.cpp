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

#include "NxMesh3DEditor.h"

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/Combo.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/List.h"
#include "Editor/GuiComponents/Separator.h"
#include "Editor/GuiComponents/Spacer.h"
#include "GameplaySystem/Framework/GameInstance.h"

#include <iterator>

using namespace Core::Gui;

namespace Core
{
    ECS_COMPONENT_IMPL(NxMesh3DEditorEWC)

    void NxMesh3DEditorEWC::onInitialize()
    {
        NxActorBasedEditorEWC::onInitialize();

        constexpr float defaultLabelWidth = 140.0f;
        constexpr float defaultModifierWidth = 300.0f;

        auto shaderDataProvider = [](std::size_t inputIndex, StringAtom& out) -> const void*
        {
            auto it = GetShaderManager().getShaderMetas().begin();
            std::advance(it, inputIndex);
            if (it == GetShaderManager().getShaderMetas().end())
            {
                DEBUG_ASSERT(false, "Internal error, can't get specified shader");
                return nullptr;
            }

            out = it->first;
            return nullptr;
        };
        auto shaderSizeProvider = []
        {
            return GetShaderManager().getShaderMetas().size();
        };

        {
            auto* h = _layout.addChildComponent<HorizontalLayout>();
            h->addChildComponent<Label>("Logical path")->setWidth(defaultLabelWidth);
            _logicalPathInput = h->addChildComponent<TextInput>();
            _logicalPathInput->setFlex(Flex::FlexWidth);
            _logicalPathInput->setReadOnly(true);
        }

        _layout.addChildComponent<Separator>();

        {
            auto* h = _layout.addChildComponent<HorizontalLayout>();
            h->addChildComponent<Label>("Model path")->setWidth(defaultLabelWidth);
            _modelInput = h->addChildComponent<TextInput>();
            _modelInput->setFlex(Flex::FlexWidth);
            _modelInput->onInput.subscribe(
                [this](auto)
                {
                    _isModified = true;
                });
        }

        {
            auto* h = _layout.addChildComponent<HorizontalLayout>();
            h->setHorizontalAlign(Align::SpaceBetween);
            h->addChildComponent<Label>("Main shader")->setWidth(defaultLabelWidth);
            _mainShaderCombo = h->addChildComponent<ComboModelBased>();
            _mainShaderCombo->setWidth(defaultModifierWidth);
            _mainShaderCombo->setDataProvider(shaderDataProvider);
            _mainShaderCombo->setSizeProvider(shaderSizeProvider);
            _mainShaderCombo->onSelect.subscribe(
                [this](auto)
                {
                    _isModified = true;
                });
        }
        {
            auto* h = _layout.addChildComponent<HorizontalLayout>();
            h->setHorizontalAlign(Align::SpaceBetween);
            h->addChildComponent<Label>("Outline shader")->setWidth(defaultLabelWidth);
            _outlineShaderCombo = h->addChildComponent<ComboModelBased>();
            _outlineShaderCombo->setWidth(defaultModifierWidth);
            _outlineShaderCombo->setDataProvider(shaderDataProvider);
            _outlineShaderCombo->setSizeProvider(shaderSizeProvider);
            _outlineShaderCombo->onSelect.subscribe(
                [this](auto)
                {
                    _isModified = true;
                });
        }
        {
            auto* h = _layout.addChildComponent<HorizontalLayout>();
            h->setHorizontalAlign(Align::SpaceBetween);
            h->addChildComponent<Label>("On load scale")->setWidth(defaultLabelWidth);
            _onLoadScale = h->addChildComponent<FloatInput>();
            _onLoadScale->setWidth(defaultModifierWidth);
            _onLoadScale->setMin(0.0f);
            _onLoadScale->setStep(0.1f);
            _onLoadScale->onInput.subscribe(
                [this](auto)
                {
                    _isModified = true;
                });
        }

        {
            auto* h = _layout.addChildComponent<HorizontalLayout>();
            h->setHorizontalAlign(Align::SpaceBetween);
            h->setVerticalAlign(Align::Top);
            h->addChildComponent<Label>("Post loading flags")->setWidth(defaultLabelWidth);
            _postProcessArray = h->addChildComponent<AssimpPostProcessArray>();
            _postProcessArray->setWidth(defaultModifierWidth);
            _postProcessArray->setFlex(Flex::Fixed);
            _postProcessArray->onChange.subscribe(
                [this]()
                {
                    _isModified = true;
                });
            _postProcessArray->onSave.subscribe(
                [this](const std::vector<aiPostProcessSteps>& data)
                {
                    _postProcessFlags = 0;
                    for (auto el : data)
                    {
                        _postProcessFlags |= el;
                    }
                });
        }

        fetchFromAssetsManager();
    }

    void NxMesh3DEditorEWC::onDraw()
    {
        NxActorBasedEditorEWC::onDraw();

        _layout.tick(GetWorld().timeDelta);
    }

    void NxMesh3DEditorEWC::fetchFromAssetsManager()
    {
        if (!_targetAsset.isValid())
        {
            return;
        }

        _logicalPathInput->setInputtedData(_targetAsset->getLogicPath().toStdString());
        _modelInput->setInputtedData(_targetAsset->getPathToMode().generic_string());
        _mainShaderCombo->setCurrentIndex(convertShaderNameToIndex(_targetAsset->getMainShader()));
        _outlineShaderCombo->setCurrentIndex(
            convertShaderNameToIndex(_targetAsset->getOutlineShader()));
        _onLoadScale->setInputtedData(_targetAsset->getOnLoadScale());

        _postProcessFlags = _targetAsset->getAssimpPostProcessFlags();
        _postProcessArray->clearData();
        for (std::size_t i = 0; i < sizeof(int) * 8; ++i)
        {
            const auto flag = static_cast<aiPostProcessSteps>(
                (1 << i) & _targetAsset->getAssimpPostProcessFlags());
            if (flag != 0)
            {
                _postProcessArray->add(flag);
            }
        }

        _isModified = false;
    }

    std::size_t NxMesh3DEditorEWC::convertShaderNameToIndex(const StringAtom& shaderName) const
    {
        const auto it = GetShaderManager().getShaderMetas().find(shaderName);
        return std::distance(GetShaderManager().getShaderMetas().begin(), it);
    }

    StringAtom NxMesh3DEditorEWC::convertIndexToShaderName(std::size_t index) const
    {
        auto it = GetShaderManager().getShaderMetas().begin();
        std::advance(it, index);
        DEBUG_ASSERT(it != GetShaderManager().getShaderMetas().end());
        return it->first;
    }

    void NxMesh3DEditorEWC::onDiscardChanges()
    {
        NxActorBasedEditorEWC::onDiscardChanges();

        _targetAsset = GetAssetsManager().getMesh3D(StringAtom::Intern(_filePath.generic_string()));
        // _targetActor = _targetAsset->;
        fetchFromAssetsManager();
    }

    void NxMesh3DEditorEWC::onSave()
    {
        NxActorBasedEditorEWC::onSave();

        _targetAsset->setPathToModel(_modelInput->getInputtedData().c_str());
        _targetAsset->setMainShader(convertIndexToShaderName(_mainShaderCombo->getCurrentIndex()));
        _targetAsset->setOutlineShader(
            convertIndexToShaderName(_outlineShaderCombo->getCurrentIndex()));
        _targetAsset->setOnLoadScale(_onLoadScale->getInputtedData());
        _targetAsset->setAssimpPostProcessFlags(_postProcessFlags);
        _targetAsset->writeToFile();
    }

    void NxMesh3DEditorEWC::onOpenFromPath(const std::filesystem::path& path)
    {
        _targetAsset = GetAssetsManager().getMesh3D(StringAtom::Intern(path.generic_string()));
        _targetActor = _targetAsset.get();
        fetchFromAssetsManager();
    }

} // namespace Core