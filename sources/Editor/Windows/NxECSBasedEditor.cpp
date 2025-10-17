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

#include "NxECSBasedEditor.h"

namespace Core
{

    ECS_COMPONENT_IMPL(NxECSBasedEditorEWC);

    void NxECSBasedEditorEWC::onInitialize()
    {
        NxEditorBaseEditorEWC::onInitialize();

        constexpr float labelWidth = 60.0f;

        using namespace Gui;

        _headerLayout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

        _logicalPath = _headerLayout.addChildComponent<LabelRow<TextInput>>("ID", labelWidth);
        _logicalPath->input->setFlex(Flex::FlexWidth);
        _logicalPath->input->setReadOnly(true);

        _assetType = _headerLayout.addChildComponent<LabelRow<TextInput>>("Type", labelWidth);
        _assetType->input->setFlex(Flex::FlexWidth);
        _assetType->input->setReadOnly(true);
    }

    void NxECSBasedEditorEWC::onDrawProperties()
    {
        const auto dt = GetWorld().timeDelta;
        _headerLayout.tick(dt);
    }

    void NxECSBasedEditorEWC::onDiscardChanges()
    {
    }

    void NxECSBasedEditorEWC::onSave()
    {
        if (!_targetAsset)
        {
            return;
        }
    }

    void NxECSBasedEditorEWC::updateGuiBasedOnAsset()
    {
        if (!_targetAsset)
        {
            return;
        }

        _logicalPath->input->setInputtedData(_targetAsset->getLogicPath().toStdString());
        _assetType->input->setInputtedData(_targetAsset->getType().toStdString());
    }

    void NxECSBasedEditorEWC::onOpenFromPath(const std::filesystem::path& path)
    {
        _targetAsset = GetAssetsManager().getAssetByPath(path);
        if (!_targetAsset)
        {
            errorLog("Requested asset not found: " + path.generic_string());
        }
    }

    void NxECSBasedEditorEWC::onFinishOpenFromPath(const std::filesystem::path& path)
    {
        NxEditorBaseEditorEWC::onFinishOpenFromPath(path);
        updateGuiBasedOnAsset();
    }

} // namespace Core