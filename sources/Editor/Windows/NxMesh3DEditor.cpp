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
        BaseFloatEWC::onInitialize();

        _windowFlags |= ImGuiWindowFlags_MenuBar;

        constexpr float defaultLabelWidth = 140.0f;

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
        }

        {
            auto* h = _layout.addChildComponent<HorizontalLayout>();
            h->setHorizontalAlign(Align::SpaceBetween);
            h->addChildComponent<Label>("Main shader")->setWidth(defaultLabelWidth);
            _mainShaderCombo = h->addChildComponent<ComboModelBased>();
            _mainShaderCombo->setWidth(240.f);
            _mainShaderCombo->setDataProvider(shaderDataProvider);
            _mainShaderCombo->setSizeProvider(shaderSizeProvider);
        }
        {
            auto* h = _layout.addChildComponent<HorizontalLayout>();
            h->setHorizontalAlign(Align::SpaceBetween);
            h->addChildComponent<Label>("Outline shader")->setWidth(defaultLabelWidth);
            _outlineShaderCombo = h->addChildComponent<ComboModelBased>();
            _outlineShaderCombo->setWidth(240.f);
            _outlineShaderCombo->setDataProvider(shaderDataProvider);
            _outlineShaderCombo->setSizeProvider(shaderSizeProvider);
        }
        {
            auto* h = _layout.addChildComponent<HorizontalLayout>();
            h->setHorizontalAlign(Align::SpaceBetween);
            h->addChildComponent<Label>("Scale")->setWidth(defaultLabelWidth);
            _scaleInput = h->addChildComponent<FloatInput>();
            _scaleInput->setWidth(240.f);
            _scaleInput->setMin(0.0f);
            _scaleInput->setStep(0.1f);
        }

        refresh();
    }

    void NxMesh3DEditorEWC::onDraw()
    {
        drawBarMenu();

        _layout.tick(GetWorld().timeDelta);
    }

    void NxMesh3DEditorEWC::drawBarMenu()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem(ICON_FA_FILE_O " Open"))
                {
                    openFromFileSystem();
                }
                if (ImGui::MenuItem(ICON_FA_FLOPPY_O " Save"))
                {
                    save();
                }
                if (ImGui::MenuItem(ICON_FA_POWER_OFF " Exit"))
                {
                    closeWindow();
                }

                ImGui::EndMenu();
            }

            if (!_filePath.empty())
            {
                ImGui::SameLine(ImGui::GetWindowWidth()
                                - (ImGui::CalcTextSize(_filePath.generic_string().c_str()).x)
                                - ImGui::GetStyle().ItemSpacing.x * 3.f);
                ImGui::TextUnformatted(_filePath.generic_string().c_str());
            }

            ImGui::EndMenuBar();
        }
    }

    void NxMesh3DEditorEWC::refresh()
    {
        if (!_targetMesh.isValid())
        {
            setEnabledStatusForAllProps(false);
            return;
        }
        setEnabledStatusForAllProps(true);

        _logicalPathInput->setInputtedData(_targetMesh->getLogicPath().toStdString());
        _modelInput->setInputtedData(_targetMesh->getPathToMode());
        _mainShaderCombo->setCurrentIndex(convertShaderNameToIndex(_targetMesh->getMainShader()));
        _outlineShaderCombo->setCurrentIndex(
            convertShaderNameToIndex(_targetMesh->getOutlineShader()));
        _scaleInput->setInputtedData(_targetMesh->getScale());
    }

    void NxMesh3DEditorEWC::save()
    {
    }

    void NxMesh3DEditorEWC::openFromFileSystem()
    {
        const auto path = AssetsManager::OpenFileSelectionDialog({ "*.nxmesh3d" });
        if (path.isEmpty())
        {
            return;
        }

        _filePath = path.toStdString();
        _targetMesh = GetAssetsManager().getMesh3D(StringAtom::Intern(path));
        refresh();
    }

    void NxMesh3DEditorEWC::setEnabledStatusForAllProps(bool isEnabled)
    {
        _logicalPathInput->disableWidget(!isEnabled);
        _modelInput->disableWidget(!isEnabled);
        _mainShaderCombo->disableWidget(!isEnabled);
        _outlineShaderCombo->disableWidget(!isEnabled);
        _scaleInput->disableWidget(!isEnabled);
    }

    std::size_t NxMesh3DEditorEWC::convertShaderNameToIndex(const StringAtom& shaderName) const
    {
        const auto it = GetShaderManager().getShaderMetas().find(shaderName);
        return std::distance(GetShaderManager().getShaderMetas().begin(), it);
    }

} // namespace Core