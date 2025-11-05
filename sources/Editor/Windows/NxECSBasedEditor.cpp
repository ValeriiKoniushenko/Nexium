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

#include "BaseComponentAdapter.h"
#include "Editor/GuiComponents/Array.h"
#include "Editor/GuiComponents/Combo.h"
#include "EditorActorAdapter.h"
#include "EditorStaticMeshBundleAdapter.h"

using namespace Core::Gui;

namespace Core
{

    ECS_COMPONENT_IMPL(NxECSBasedEditorEWC);
    ECS_COMPONENT_IMPL(ECSEditorMimeAdapter);

    void ECSEditorMimeAdapter::applyAssetRawData(const nlohmann::json& json)
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

    BaseComponent* ECSEditorMimeAdapter::getTargetComponent()
    {
        if (auto* parent = getParentAs<NxECSBasedEditorEWC>())
        {
            return parent->getTargetComponent();
        }
        Assert(false);
        return nullptr;
    }

    const BaseComponent* ECSEditorMimeAdapter::getTargetComponent() const
    {
        if (auto* parent = getParentAs<NxECSBasedEditorEWC>())
        {
            return parent->getTargetComponent();
        }
        Assert(false);
        return nullptr;
    }

    BaseComponent* NxECSBasedEditorEWC::getTargetComponent() noexcept
    {
        return _targetComponent;
    }

    const BaseComponent* NxECSBasedEditorEWC::getTargetComponent() const noexcept
    {
        return _targetComponent;
    }

    void NxECSBasedEditorEWC::onInitialize()
    {
        NxEditorBaseEditorEWC::onInitialize();

        addUniqueTypeChildComponent<ECSBaseComponentAdapter>();
        addUniqueTypeChildComponent<ECSEditorActorAdapter>();
        addUniqueTypeChildComponent<ECSEditorStaticMeshBundleAdapter>();

        setEnablePreview(true);
        setEnableTree(true);

        constexpr float labelWidth = 120.f;
        // -+- -+- -+- -+- -+- -+- -+- -+- -+- -+- -+-
        _headerLayout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

        _logicalPath = _headerLayout.addChildComponent<LabelRow<TextInput>>("ID", labelWidth);
        _logicalPath->input->setFlex(Flex::FlexWidth);
        _logicalPath->input->setReadOnly(true);

        _assetType = _headerLayout.addChildComponent<LabelRow<TextInput>>("Type", labelWidth);
        _assetType->input->setFlex(Flex::FlexWidth);
        _assetType->input->setReadOnly(true);
    }

    void NxECSBasedEditorEWC::onDrawProperties()
    {
        if (!hasTarget())
        {
            return;
        }

        const auto dt = GetWorld().timeDelta;

        if (_targetComponent == _targetAsset->getData().get())
        {
            if (CollapsingHeader("Asset data", ImGuiTreeNodeFlags_DefaultOpen))
            {
                _headerLayout.tick(dt);
            }
        }

        for (auto&& child : _children)
        {
            if (child->isEnabled())
            {
                if (auto* typeAdapter = child->tryCastTo<ECSEditorMimeAdapter>())
                {
                    typeAdapter->draw(dt);
                }
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

        if (!hasTarget())
        {
            return;
        }

        drawTreeNode(_targetAsset->getData().get(), 0);
    }

    void NxECSBasedEditorEWC::onDiscardChanges()
    {
    }

    void NxECSBasedEditorEWC::onSave()
    {
        if (!hasTarget())
        {
            return;
        }
    }

    void NxECSBasedEditorEWC::updateGuiBasedOnAsset()
    {
        if (!hasTarget())
        {
            return;
        }

        _logicalPath->input->setInputtedData(_targetAsset->getLogicPath().toStdString());
        _assetType->input->setInputtedData(_targetAsset->getType().toStdString());

        disableAllAdapters();

        for (auto& child : _children)
        {
            if (auto* adapter = child->tryCastTo<ECSEditorMimeAdapter>())
            {
                if (adapter->canWorkWith(_targetComponent))
                {
                    adapter->enable();
                    adapter->applyAssetRawData(_targetAsset->getAssetData());
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

        _targetComponent = _targetAsset->getData().get();

        setup();
        return true;
    }

    void NxECSBasedEditorEWC::onClose()
    {
        NxEditorBaseEditorEWC::onClose();
        reset();
    }

    void NxECSBasedEditorEWC::drawTreeNode(BaseComponent* comp, int id)
    {
        if (!comp)
        {
            return;
        }

        int flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth
                    | ImGuiTreeNodeFlags_OpenOnArrow;

        if (!comp->hasParent())
        {
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        if (!comp->hasChildren())
        {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        if (comp == _targetComponent)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        const bool isDisabled = !comp->isEnabled();
        if (isDisabled)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.6f);
        }

        ImGui::PushID(id);
        const bool isOpened = ImGui::TreeNodeEx(comp->getComponentName().c_str(), flags);

        if (isDisabled)
        {
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }

        if (ImGui::IsItemClicked() || (ImGui::IsItemFocused() && isHovered()))
        {
            _targetComponent = comp;
            updateGuiBasedOnAsset();
        }

        if (isOpened)
        {
            for (auto&& child : comp->getChildren())
            {
                drawTreeNode(child.get(), id + 1);
            }

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void NxECSBasedEditorEWC::disableAllAdapters()
    {
        for (auto&& child : _children)
        {
            if (child->tryCastTo<ECSEditorMimeAdapter>())
            {
                child->disable();
            }
        }
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