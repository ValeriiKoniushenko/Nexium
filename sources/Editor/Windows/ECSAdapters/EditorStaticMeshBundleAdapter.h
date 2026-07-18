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

#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Editor/Windows/NxECSBasedEditor.h"
#include "assimp/postprocess.h"

namespace Core
{
    namespace Gui
    {
        template<class T>
        class LabelRow;

        class TextInput;

        class ListView;
        class CheckBox;

        template<class T, class ArrayCellViewerFunc, class ViewFetchFunc>
        class BaseArray;

        template<Utils::IsArithmetic T>
        class NumInput;

        using FloatInput = NumInput<float>;

        class ComboModelBased;

    } // namespace Gui

    struct _AssimpPostProcessArray_ArrayCellViewerFunc
    {
        Gui::HorizontalLayout::Ptr operator()(aiPostProcessSteps data) const;
    };

    struct _AssimpPostProcessArray_ViewFetchFunc
    {
        aiPostProcessSteps operator()(Gui::HorizontalLayout* layout) const;
    };

    using AssimpPostProcessArray
        = Gui::BaseArray<aiPostProcessSteps, _AssimpPostProcessArray_ArrayCellViewerFunc,
                         _AssimpPostProcessArray_ViewFetchFunc>;

    CLASS();
    class ECSEditorStaticMeshBundleAdapter : public ECSEditorMimeAdapter
    {
        R_FRIEND_DECL(ECSEditorStaticMeshBundleAdapter, Core::ECSEditorMimeAdapter);
        ECS_COMPONENT_DECL(ECSEditorStaticMeshBundleAdapter, ECSEditorMimeAdapter);

    public:
        struct StreamData
        {
            StreamData() = delete;

            static constexpr const char* path = "path";
            static constexpr const char* mainShader = "mainShader";
            static constexpr const char* outlineShader = "outlineShader";
            static constexpr const char* onLoadScale = "onLoadScale";
            static constexpr const char* assimpPostProcess = "assimpPostProcess";
        };

    public:
        [[nodiscard]] bool canWorkWith(BaseComponent* component) const override;
        [[nodiscard]] StringAtom getProcessedAssetType() const override;

    protected:
        [[nodiscard]] std::size_t convertShaderNameToIndex(const StringAtom& shaderName) const;
        [[nodiscard]] StringAtom convertIndexToShaderName(std::size_t index) const;

        void onInitialize() override;
        void onApplyAssetData(const nlohmann::json& json) override;
        void onDraw(float dt) override;

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;
        Gui::VerticalLayout _layout;

        Gui::LabelRow<Gui::TextInput>* _logicalPath = nullptr;

        Gui::LabelRow<Gui::TextInput>* _modelInput = nullptr;
        Gui::LabelRow<Gui::ComboModelBased>* _mainShaderCombo = nullptr;
        Gui::LabelRow<Gui::ComboModelBased>* _outlineShaderCombo = nullptr;
        Gui::LabelRow<Gui::FloatInput>* _onLoadScale = nullptr;
        Gui::LabelRow<AssimpPostProcessArray>* _postProcessArray = nullptr;
        int _postProcessFlags = 0;
    };

} // namespace Core

#include "EditorStaticMeshBundleAdapter.generated.h" // added by the code generator. Better don't move it.
