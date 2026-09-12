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
#include "Editors/NxEditorBaseEditor.h"
#include "InputDevices/InputManager.h"
#include "NxFundamental/Assets/ECSAsset.h"

namespace Core
{

    namespace Gui
    {
        template<typename T>
        class LabelRow;

        class TextInput;

    } // namespace Gui

    CLASS();
    class ECSEditorMimeAdapter : public BaseComponent
    {
        ECS_DECL(ECSEditorMimeAdapter, Core::BaseComponent);

    public:
        void applyAssetRawData(const nlohmann::json& json);
        void draw(float dt);
        [[nodiscard]] virtual bool canWorkWith(BaseComponent* component) const = 0;
        [[nodiscard]] bool isDirectWorkingMatchWith(const BaseComponent* component) const;

        [[nodiscard]] virtual StringAtom getProcessedAssetType() const = 0;

    protected:
        virtual void onDraw(float dt) = 0;
        virtual void onApplyAssetData(const nlohmann::json& json) = 0;

        void makeParentDirty();
        [[nodiscard]] BaseComponent* getTargetComponent();
        [[nodiscard]] const BaseComponent* getTargetComponent() const;
        [[nodiscard]] NXECSAsset getTargetAsset();
    };

    CLASS();
    class NxECSBasedEditorEWC : public NxEditorBaseEditorEWC
    {
        ECS_DECL(NxECSBasedEditorEWC, Core::NxEditorBaseEditorEWC);

    public:
        [[nodiscard]] BaseComponent* getTargetComponent() noexcept;
        [[nodiscard]] const BaseComponent* getTargetComponent() const noexcept;

        [[nodiscard]] NXECSAsset getTargetAsset();
        [[nodiscard]] const NXECSAsset getTargetAsset() const;

        [[nodiscard]] bool hasTarget() const noexcept { return _targetComponent && _targetAsset; }

    protected:
        void onInitialize() override;
        void onDrawProperties() override;
        void onDrawPreview() override;
        void onDrawTree() override;
        void onDiscardChanges() override;
        void onTreeContextMenuBehavior() override;
        void onSave() override;
        void updateGuiBasedOnAsset() override;
        void onUpdate() override;

        bool onOpenFromPath(const std::filesystem::path& path) override;
        void onClose() override;

        void drawTreeNode(BaseComponent* comp, int& id);
        void disableAllAdapters();

        void reset();
        void setup();

        void removeCurrentComponent(BaseComponent* component);

    protected:
        KeyboardInputManger _keyboardManager;
        DelegateSubscriberPoolGuard _subscriptionPool;

        Gui::VerticalLayout _headerLayout;
        Gui::LabelRow<Gui::TextInput>* _logicalPath = nullptr;
        Gui::LabelRow<Gui::TextInput>* _assetType = nullptr;

        NXECSAsset _targetAsset;
        BaseComponent* _targetComponent = nullptr;
    };

} // namespace Core

#include "NxECSBasedEditor.generated.h" // added by the code generator. Better don't move it.
