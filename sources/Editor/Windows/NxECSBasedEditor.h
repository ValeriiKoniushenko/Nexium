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

#include "AssetsManager/ECSAsset.h"
#include "Editor/GuiComponents/Array.h"
#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/Misc.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "NxEditorBaseEditor.h"

namespace Core
{

    class ECSEditorMimeAdapter : public BaseComponent
    {
        ECS_COMPONENT_DECL(ECSEditorMimeAdapter, BaseComponent);

    public:
        void applyAssetData(const nlohmann::json& json);
        void draw(float dt);

    protected:
        virtual void onDraw(float dt) = 0;
        virtual void onApplyAssetData(const nlohmann::json& json) = 0;

        void makeParentDirty();
    };

    class NxECSBasedEditorEWC : public NxEditorBaseEditorEWC
    {
        ECS_COMPONENT_DECL(NxECSBasedEditorEWC, NxEditorBaseEditorEWC);

    public:
        template<typename T>
        void registerMimeTypeAdapter(const StringAtom& mimeType)
        {
            if (hasMimeTypeAdapter(mimeType))
            {
                warnLog(
                    "You're trying to register the adapter for the MIME type '{}' twice. Previous adapter will be owerwritten by new one."_f
                    << mimeType);
            }

            _mimeTypeAdapters[mimeType] = [mimeType]()
            {
                return new T(mimeType);
            };
        }

        [[nodiscard]] bool hasMimeTypeAdapter(const StringAtom& mimeType) const;

    protected:
        void onInitialize() override;
        void onDrawProperties() override;
        void onDrawPreview() override;
        void onDrawTree() override;
        void onDiscardChanges() override;
        void onSave() override;
        void updateGuiBasedOnAsset() override;
        bool onOpenFromPath(const std::filesystem::path& path) override;
        void onClose() override;

        void drawTreeNode(BaseComponent* comp, int id);

        [[nodiscard]] ECSEditorMimeAdapter::Ptr trySpawnMimeTypeAdapter(
            const StringAtom& mimeType) const;

        void reset();
        void setup();

    protected:
        Gui::VerticalLayout _headerLayout;
        Gui::LabelRow<Gui::TextInput>* _logicalPath = nullptr;
        Gui::LabelRow<Gui::TextInput>* _assetType = nullptr;

        Gui::VerticalLayout _baseEcsLayout;
        Gui::LabelRow<Gui::TextInput>* _ecsName = nullptr;
        Gui::LabelRow<Gui::TextInput>* _ecsType = nullptr;
        Gui::LabelRow<Gui::CheckBox>* _ecsDisableTicks = nullptr;
        Gui::LabelRow<Gui::CheckBox>* _ecsEnabledComponent = nullptr;
        Gui::LabelRow<Gui::TextInput>* _ecsParent = nullptr;
        Gui::LabelRow<Gui::StringArray>* _ecsChildren = nullptr;

        std::unordered_map<StringAtom, std::function<ECSEditorMimeAdapter::Ptr()>>
            _mimeTypeAdapters;

        NXAsset _targetAsset;
    };

} // namespace Core