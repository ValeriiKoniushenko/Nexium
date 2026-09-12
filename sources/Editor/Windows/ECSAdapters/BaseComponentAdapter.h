// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "../NxECSBasedEditor.h"
#include "Editor/GuiComponents/VerticalLayout.h"

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

        struct _StringArray_ArrayCellViewerFunc;
        struct _StringArray_ViewFetchFunc;

        using StringArray
            = BaseArray<StringAtom, _StringArray_ArrayCellViewerFunc, _StringArray_ViewFetchFunc>;
    } // namespace Gui

    CLASS();
    class ECSBaseComponentAdapter : public ECSEditorMimeAdapter
    {
        ECS_DECL(ECSBaseComponentAdapter, Core::ECSEditorMimeAdapter);

    public:
        [[nodiscard]] bool canWorkWith(BaseComponent* component) const override { return true; }
        [[nodiscard]] StringAtom getProcessedAssetType() const override
        {
            return BaseComponent::componentType;
        }

    protected:
        void onApplyAssetData(const nlohmann::json& json) override;
        void onInitialize() override;
        void onDraw(float dt) override;

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;

        Gui::VerticalLayout _baseEcsLayout;
        Gui::LabelRow<Gui::TextInput>* _ecsName = nullptr;
        Gui::LabelRow<Gui::TextInput>* _ecsType = nullptr;
        Gui::LabelRow<Gui::CheckBox>* _ecsDisableTicks = nullptr;
        Gui::LabelRow<Gui::CheckBox>* _ecsEnabledComponent = nullptr;
        Gui::LabelRow<Gui::TextInput>* _ecsParent = nullptr;
        Gui::LabelRow<Gui::StringArray>* _ecsChildren = nullptr;
    };

} // namespace Core

#include "BaseComponentAdapter.generated.h" // added by the code generator. Better don't move it.
