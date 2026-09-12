// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Editor/GuiComponents/VerticalLayout.h"
#include "Editor/Windows/Editors/NxEditorBaseEditor.h"
#include "NxSubsystems/AssetsManager/TextureAsset.h"

namespace Core
{
    namespace Gui
    {
        template<typename T>
        class LabelRow;

        template<std::size_t Size, Utils::IsArithmetic Type>
        class VecNumInput;

        class TextInput;
        class CheckBox;

        using Int2Input = VecNumInput<2, int>;

    } // namespace Gui

    CLASS();
    class NxTextureEditorEWC : public NxEditorBaseEditorEWC
    {
        ECS_DECL(NxTextureEditorEWC, Core::NxEditorBaseEditorEWC);

    public:
        [[nodiscard]] const char* getIcon() override;

    protected:
        void updateGuiBasedOnAsset() override;
        void onInitialize() override;
        void onDrawProperties() override;
        void onDrawPreview() override;
        void onDiscardChanges() override;
        void onSave() override;
        bool onOpenFromPath(const std::filesystem::path& path) override;

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;

        Gui::VerticalLayout _layout;

        Gui::LabelRow<Gui::Int2Input>* _imageSize = nullptr;
        Gui::LabelRow<Gui::TextInput>* _imageChannelType = nullptr;
        Gui::LabelRow<Gui::TextInput>* _pathToImage = nullptr;
        Gui::LabelRow<Gui::CheckBox>* _isFlipVertical = nullptr;
        float _zoom = 1.f;
        glm::vec2 _offset = {};

        glm::vec2 _lastPreviewRegionSize = {};

        NXTexture _targetAsset;
    };
} // namespace Core

#include "NxTextureEditor.generated.h" // added by the code generator. Better don't move it.
