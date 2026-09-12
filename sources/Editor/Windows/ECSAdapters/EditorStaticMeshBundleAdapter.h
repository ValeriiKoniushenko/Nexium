// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

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
        ECS_DECL(ECSEditorStaticMeshBundleAdapter, Core::ECSEditorMimeAdapter);

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
