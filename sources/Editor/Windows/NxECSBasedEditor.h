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

#include "AssetsManager/ECSAsset.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "InputDevices/InputManager.h"
#include "NxEditorBaseEditor.h"

namespace Core
{

    namespace Gui
    {
        template<typename T>
        class LabelRow;

        class TextInput;

    } // namespace Gui
    class ECSEditorMimeAdapter : public BaseComponent
    {
        ECS_COMPONENT_DECL(ECSEditorMimeAdapter, BaseComponent);

    public:
        void applyAssetRawData(const nlohmann::json& json);
        void draw(float dt);
        [[nodiscard]] virtual bool canWorkWith(BaseComponent* component) const = 0;
        [[nodiscard]] bool isDirectWorkingMatchWith(const BaseComponent* component) const;

        [[nodiscard]] virtual StringAtom getProcessedAssetType() const = 0;

        [[nodiscard]] virtual nlohmann::json packAssetDataFromObject() const { return {}; }

    protected:
        [[nodiscard]] const char* getPrefix() const override { return "ECSEditorMimeAdapter"; }
        virtual void onDraw(float dt) = 0;
        virtual void onApplyAssetData(const nlohmann::json& json) = 0;

        void makeParentDirty();
        [[nodiscard]] BaseComponent* getTargetComponent();
        [[nodiscard]] const BaseComponent* getTargetComponent() const;
        [[nodiscard]] NXECSAsset getTargetAsset();
    };

    class NxECSBasedEditorEWC : public NxEditorBaseEditorEWC
    {
        ECS_COMPONENT_DECL(NxECSBasedEditorEWC, NxEditorBaseEditorEWC);

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

        [[nodiscard]] const char* getPrefix() const override { return "NxECSBasedEditorEWC"; }

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
