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

#include "NxECSBasedEditor.h"

#include "Editor/GuiComponents/Array.h"
#include "Editor/GuiComponents/Combo.h"

using namespace Core::Gui;

namespace Core
{

    ECS_COMPONENT_IMPL(NxECSBasedEditorEWC);
    ECS_COMPONENT_IMPL(ECSEditorMimeAdapter);

    void ECSEditorMimeAdapter::applyAssetData(const nlohmann::json& json)
    {
        if (json.empty())
        {
            globalLog.warnLog("Can't apply asset data. No data.");
            return;
        }

        onApplyAssetData(json);
    }

    void ECSEditorMimeAdapter::draw(float dt)
    {
        onDraw(dt);
    }

    void ECSEditorMimeAdapter::makeParentDirty()
    {
        if (auto* parent = getParentAs<NxECSBasedEditorEWC>())
        {
            parent->makeDirty();
        }
    }

    bool NxECSBasedEditorEWC::hasMimeTypeAdapter(const StringAtom& mimeType) const
    {
        return _mimeTypeAdapters.contains(mimeType);
    }

    void NxECSBasedEditorEWC::onInitialize()
    {
        NxEditorBaseEditorEWC::onInitialize();

        setEnablePreview(true);
        setEnableTree(true);

        _headerLayout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

        constexpr float labelWidth = 60.0f;
        _logicalPath = _headerLayout.addChildComponent<LabelRow<TextInput>>("ID", labelWidth);
        _logicalPath->input->setFlex(Flex::FlexWidth);
        _logicalPath->input->setReadOnly(true);

        _assetType = _headerLayout.addChildComponent<LabelRow<TextInput>>("Type", labelWidth);
        _assetType->input->setFlex(Flex::FlexWidth);
        _assetType->input->setReadOnly(true);
    }

    void NxECSBasedEditorEWC::onDrawProperties()
    {
        const auto dt = GetWorld().timeDelta;
        _headerLayout.tick(dt);
        for (auto&& child : _children)
        {
            if (auto* typeAdapter = dynamic_cast<ECSEditorMimeAdapter*>(child.get()))
            {
                typeAdapter->draw(dt);
            }
        }
    }

    void NxECSBasedEditorEWC::onDrawPreview()
    {
        NxEditorBaseEditorEWC::onDrawPreview();
    }

    void NxECSBasedEditorEWC::onDrawTree()
    {
        NxEditorBaseEditorEWC::onDrawTree();

        if (!_targetAsset)
        {
            return;
        }

        int lastChildIndex = -1;
        int id = 0;
        constexpr int baseFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick
                                  | ImGuiTreeNodeFlags_SpanAvailWidth
                                  | ImGuiTreeNodeFlags_OpenOnArrow;

        std::stack<BaseComponent*> stack;
        auto* asset = _targetAsset->getData().get();
        stack.push(asset);

        while (!stack.empty() && asset != nullptr)
        {
            ImGui::PushID(id++);

            int flags = baseFlags;
            if (!asset->hasChildren())
            {
                flags |= ImGuiTreeNodeFlags_Leaf;
            }

            const bool isOpened = ImGui::TreeNodeEx(asset->getComponentName().c_str(), flags);
            if (ImGui::IsItemClicked() || (ImGui::IsItemFocused() && isHovered()))
            {
            }

            if (isOpened)
            {
                ImGui::TreePop();
            }

            ImGui::PopID();

            if (asset->hasChildren()
                && lastChildIndex < static_cast<int>(asset->getChildrenCount()))
            {
                asset = asset->getChildAt(++lastChildIndex).get();
            }
            else
            {
                stack.pop();
                asset = stack.empty() ? nullptr : stack.top();
                lastChildIndex = -1;
            }
        }
    }

    void NxECSBasedEditorEWC::onDiscardChanges()
    {
    }

    void NxECSBasedEditorEWC::onSave()
    {
        if (!_targetAsset)
        {
            return;
        }
    }

    void NxECSBasedEditorEWC::updateGuiBasedOnAsset()
    {
        if (!_targetAsset)
        {
            return;
        }

        _logicalPath->input->setInputtedData(_targetAsset->getLogicPath().toStdString());
        _assetType->input->setInputtedData(_targetAsset->getType().toStdString());

        if (hasMimeTypeAdapter(_targetAsset->getType()))
        {
            if (auto adapter = trySpawnMimeTypeAdapter(_targetAsset->getType()))
            {
                if (auto* p = attachUniqueChild(adapter)->castTo<ECSEditorMimeAdapter>())
                {
                    p->applyAssetData(_targetAsset->getAssetData());
                }
            }
        }
    }

    bool NxECSBasedEditorEWC::onOpenFromPath(const std::filesystem::path& path)
    {
        _targetAsset = GetAssetsManager().getAssetByPath(path);
        if (!_targetAsset)
        {
            errorLog("Requested asset not found: " + path.generic_string());
            return false;
        }

        setup();
        return true;
    }

    void NxECSBasedEditorEWC::onClose()
    {
        NxEditorBaseEditorEWC::onClose();
        reset();
    }

    ECSEditorMimeAdapter::Ptr NxECSBasedEditorEWC::trySpawnMimeTypeAdapter(
        const StringAtom& mimeType) const
    {
        if (!hasMimeTypeAdapter(mimeType))
        {
            errorLog("Can't spawn editor's mime adapter. Mime type '{}' is not registered."_f
                     << mimeType);
            return nullptr;
        }

        for (auto&& child : _children)
        {
            if (child->getComponentName() == mimeType)
            {
                return child->castTo<ECSEditorMimeAdapter>();
            }
        }

        return _mimeTypeAdapters.at(mimeType)();
    }

    void NxECSBasedEditorEWC::reset()
    {
        removeChildOf<ECSEditorMimeAdapter>();

        _targetAsset.reset();
    }

    void NxECSBasedEditorEWC::setup()
    {
        if (!_targetAsset)
        {
            return;
        }

        if (_targetAsset->getLoadingStatus().cast() != ECSAsset::Status::Loaded)
        {
            Assert(false);
            errorLog("Can't setup editor's tree. Asset '{}' is not loaded properly."_f
                     << _targetAsset->getLogicPath());
            return;
        }

        updateGuiBasedOnAsset();
    }

} // namespace Core