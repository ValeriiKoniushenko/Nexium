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
        _imageChannelType->input->setInputtedData(R::ToString(_targetAsset->getChannels()));
        _pathToImage->input->setInputtedData(_targetAsset->getFilePath().generic_string());
        _isFlipVertical->input->setValue(_targetAsset->isFlipVertically());
    }

    void NxTextureEditorEWC::onInitialize()
    {
        NxEditorBaseEditorEWC::onInitialize();

        setEnablePreview(true);
        _fileFilters.emplace(std::string("*") + NXTexture::AssetT::fileExtension);

        constexpr float defaultLabelWidth = 140.0f;

        const auto gap = ImGui::GetStyle().WindowPadding.x;

        _layout.setPaddings(glm::vec4(gap));

        /////////////////////
        _imageSize
            = _layout.addChildComponent<LabelRow<Gui::Int2Input>>("Image size", defaultLabelWidth);
        _imageSize->input->setReadOnly(true);
        _subscriptionPool << _imageSize->input->onInput->subscribeAndGetID([this](auto)
                                                                           { makeDirty(); });

        ///////////////////////
        _imageChannelType
            = _layout.addChildComponent<LabelRow<Gui::TextInput>>("Channels", defaultLabelWidth);
        _imageChannelType->input->setReadOnly(true);
        _imageChannelType->input->setFlex(Flex::FlexWidth);
        _subscriptionPool << _imageChannelType->input->onInput->subscribeAndGetID([this](auto)
                                                                                  { makeDirty(); });

        ///////////////////////
        _pathToImage = _layout.addChildComponent<LabelRow<Gui::TextInput>>("Path to image",
                                                                           defaultLabelWidth);
        _pathToImage->input->setFlex(Flex::FlexWidth);
        _subscriptionPool << _pathToImage->input->onInput->subscribeAndGetID([this](auto)
                                                                             { makeDirty(); });

        ///////////////////////
        _isFlipVertical = _layout.addChildComponent<LabelRow<Gui::CheckBox>>("Flip vertical",
                                                                             defaultLabelWidth);
        _subscriptionPool << _isFlipVertical->input->onChange->subscribeAndGetID([this](auto)
                                                                                 { makeDirty(); });

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

        const float xy1 = _isFlipVertical->input->getValue();
        const float xy2 = !_isFlipVertical->input->getValue();
        ImGui::Image(image.getTextureId(), displaySize, glm::vec2(xy1), glm::vec2(xy2));
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

    bool NxTextureEditorEWC::onOpenFromPath(const std::filesystem::path& path)
    {
        _targetAsset = GetAssetsManager().getTexture(StringAtom::Intern(path.generic_string()));
        if (_targetAsset)
        {
            _offset = _lastPreviewRegionSize
                      - static_cast<glm::vec2>(_targetAsset->getData().getSize().toGlm());
            _offset /= 2.0f;
        }

        return true;
    }

} // namespace Core