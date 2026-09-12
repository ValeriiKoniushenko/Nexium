// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxTextureEditor.h"

#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/Misc.h"
#include "Editor/GuiComponents/VecInput.h"
#include "Misc/IconsFontAwesome.h"
#include "NxWorld/Framework/GameInstance.h"

using namespace Core::Gui;

namespace Core
{
    ECS_IMPL(NxTextureEditorEWC);

    const char* NxTextureEditorEWC::getIcon()
    {
        return ICON_FA_COG;
    }

    void NxTextureEditorEWC::updateGuiBasedOnAsset()
    {
        if (!_targetAsset)
        {
            return;
        }

        _imageSize->input->setInputtedData(_targetAsset->getSize().toGlm());
        _imageChannelType->input->setInputtedData(
            R<Image::Channel>::ToString(_targetAsset->getChannels()).data());
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
            _layout.tick(GetWorld()->getTimeDelta());
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
            _zoom = std::max(0.1f, _zoom + (ImGui::GetIO().MouseWheel * 0.1f));
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
            = GetAssetsManager()->getTexture(StringAtom::Intern(_assetFilePath.generic_string()));
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
        _targetAsset = GetAssetsManager()->getTexture(StringAtom::Intern(path.generic_string()));
        if (_targetAsset)
        {
            _offset = _lastPreviewRegionSize
                      - static_cast<glm::vec2>(_targetAsset->getData().getSize().toGlm());
            _offset /= 2.0f;
        }

        return true;
    }

} // namespace Core
