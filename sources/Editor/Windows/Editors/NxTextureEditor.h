/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "AssetsManager/TextureAsset.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Editor/Windows/Editors/NxEditorBaseEditor.h"

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
        R_FRIEND_DECL(NxTextureEditorEWC, Core::NxEditorBaseEditorEWC);
        ECS_COMPONENT_DECL(NxTextureEditorEWC, NxEditorBaseEditorEWC);

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
