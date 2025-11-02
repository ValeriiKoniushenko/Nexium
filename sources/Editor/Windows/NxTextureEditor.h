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

#pragma once

#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "EditorActorAdapter.h"
#include "Graphics/Texture.h"

namespace Core
{

    class NxTextureEditorEWC : public NxEditorBaseEditorEWC
    {
        ECS_COMPONENT_DECL(NxTextureEditorEWC, NxEditorBaseEditorEWC);

    public:
        [[nodiscard]] const char* getIcon() override { return ICON_FA_COG; }

    protected:
        void updateGuiBasedOnAsset() override;
        void onInitialize() override;
        void onDrawProperties() override;
        void onDrawPreview() override;
        void onDiscardChanges() override;
        void onSave() override;
        bool onOpenFromPath(const std::filesystem::path& path) override;

    protected:
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