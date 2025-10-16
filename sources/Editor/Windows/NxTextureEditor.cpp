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

#include "NxTextureEditor.h"

#include "GameplaySystem/Framework/GameInstance.h"

using namespace Core::Gui;

namespace Core
{
    ECS_COMPONENT_IMPL(NxTextureEditorEWC)

    void NxTextureEditorEWC::updateGuiBasedOnAsset()
    {
        if (!_targetAsset)
        {
            return;
        }

        _imageSize->input->setInputtedData(_targetAsset->getSize().toGlm());
        _imageChannelType->input->setInputtedData(_targetAsset->getChannels().toStr());
        _pathToImage->input->setInputtedData(_targetAsset->getFilePath().generic_string());
        _isFlipVertical->input->setValue(_targetAsset->isFlipVertically());
    }

    void NxTextureEditorEWC::onInitialize()
    {
        NxEditorBaseEditorEWC::onInitialize();

        setEnablePreview(true);
        _fileFilters.emplace(std::string("*") + NXTexture::AssetT::fileExtension);

        constexpr float defaultLabelWidth = 140.0f;
        constexpr float defaultInputWidth = 200.0f;

        const auto gap = ImGui::GetStyle().WindowPadding.x;

        _layout.setPaddings(glm::vec4(gap));

        ///////////////////////
        _imageSize
            = _layout.addChildComponent<LabelRow<Gui::Int2Input>>("Image size", defaultLabelWidth);
        _imageSize->input->setWidth(defaultInputWidth);
        _imageSize->input->setReadOnly(true);
        _imageSize->input->onInput.subscribe(
            [this](auto)
            {
                makeDirty();
            });

        ///////////////////////
        _imageChannelType
            = _layout.addChildComponent<LabelRow<Gui::TextInput>>("Channels", defaultLabelWidth);
        _imageChannelType->input->setWidth(defaultInputWidth);
        _imageChannelType->input->setReadOnly(true);
        _imageChannelType->input->onInput.subscribe(
            [this](auto)
            {
                makeDirty();
            });

        ///////////////////////
        _pathToImage = _layout.addChildComponent<LabelRow<Gui::TextInput>>("Path to image",
                                                                           defaultLabelWidth);
        _pathToImage->input->setFlex(Flex::FlexWidth);
        _pathToImage->input->onInput.subscribe(
            [this](auto)
            {
                makeDirty();
            });

        ///////////////////////
        _isFlipVertical = _layout.addChildComponent<LabelRow<Gui::CheckBox>>("Flip vertical",
                                                                             defaultLabelWidth);
        _isFlipVertical->input->onChange.subscribe(
            [this](auto)
            {
                makeDirty();
            });

        updateGuiBasedOnAsset();
    }

    void NxTextureEditorEWC::onDrawProperties()
    {
        const auto gap = ImGui::GetStyle().WindowPadding.x;

        if (Gui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen, gap))
        {
            _layout.tick(GetWorld().timeDelta);
        }
    }

    void NxTextureEditorEWC::onDrawPreview()
    {
        NxEditorBaseEditorEWC::onDrawPreview();

        _lastPreviewRegionSize = ImGui::GetContentRegionAvail();

        if (!_targetAsset)
        {
            return;
        }

        if (ImGui::IsWindowHovered() && ImGui::GetIO().MouseWheel != 0.0f)
        {
            _zoom = std::max(0.1f, _zoom + ImGui::GetIO().MouseWheel * 0.1f);
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            const auto drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
            _offset.x += drag.x;
            _offset.y += drag.y;
        }

        auto& image = _targetAsset->getData();

        const auto displaySize
            = glm::vec2(image.getSize().width * _zoom, image.getSize().height * _zoom);
        ImGui::SetCursorPos(ImGui::GetCursorPos() + _offset);

        ImGui::Image(image.getTextureId(), displaySize);
    }

    void NxTextureEditorEWC::onDiscardChanges()
    {
        _targetAsset
            = GetAssetsManager().getTexture(StringAtom::Intern(_assetFilePath.generic_string()));
    }

    void NxTextureEditorEWC::onSave()
    {
        if (!_targetAsset)
        {
            return;
        }

        _targetAsset->setFilePath(_pathToImage->input->getInputtedData());
        _targetAsset->setFlipVertically(_isFlipVertical->input->getValue());
        _targetAsset->writeToFile();
    }

    void NxTextureEditorEWC::onOpenFromPath(const std::filesystem::path& path)
    {
        _targetAsset = GetAssetsManager().getTexture(StringAtom::Intern(path.generic_string()));
        if (_targetAsset)
        {
            _offset = _lastPreviewRegionSize
                      - static_cast<glm::vec2>(_targetAsset->getData().getSize().toGlm());
            _offset /= 2.0f;
        }
    }

} // namespace Core