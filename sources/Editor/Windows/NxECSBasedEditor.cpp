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

#include "NxECSBasedEditor.h"

#include "ECSAdapters/BaseComponentAdapter.h"
#include "ECSAdapters/EditorActorAdapter.h"
#include "ECSAdapters/EditorInterleavedGraphicsDataAdapter.h"
#include "ECSAdapters/EditorStaticMeshBundleAdapter.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/Misc.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "ImGui/imgui_internal.h"
#include "Misc/IconsFontAwesome.h"
#include "ModalECSSearchPopUp.h"

using namespace Core::Gui;

namespace Core
{

    ECS_COMPONENT_IMPL(NxECSBasedEditorEWC);
    ECS_COMPONENT_IMPL(ECSEditorMimeAdapter);

    void ECSEditorMimeAdapter::applyAssetRawData(const nlohmann::json& json)
    {
        // if (json.empty())
        // {
        //     globalLog.warnLog("Can't apply asset data. No data.");
        //     return;
        // }

        onApplyAssetData(json);
    }

    void ECSEditorMimeAdapter::draw(float dt)
    {
        onDraw(dt);
    }

    bool ECSEditorMimeAdapter::isDirectWorkingMatchWith(const BaseComponent* component) const
    {
        if (component) [[likely]]
        {
            Assert(component->getComponentType().isStatic());
            Assert(getProcessedAssetType().isStatic());
            return component->getComponentType() == getProcessedAssetType();
        }

        return false;
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

    NXAsset ECSEditorMimeAdapter::getTargetAsset()
    {
        if (auto* parent = getParentAs<NxECSBasedEditorEWC>())
        {
            return parent->getTargetAsset();
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

    NXAsset NxECSBasedEditorEWC::getTargetAsset()
    {
        return _targetAsset;
    }

    const NXAsset NxECSBasedEditorEWC::getTargetAsset() const
    {
        return _targetAsset;
    }

    void NxECSBasedEditorEWC::onInitialize()
    {
        NxEditorBaseEditorEWC::onInitialize();

        addUniqueTypeChildComponent<ECSBaseComponentAdapter>();
        addUniqueTypeChildComponent<ECSEditorActorAdapter>();
        addUniqueTypeChildComponent<ECSEditorStaticMeshBundleAdapter>();
        addUniqueTypeChildComponent<ECSEditorInterleavedGraphicsDataAdapter>();

        setEnablePreview(true);
        setEnableTree(true);

        // -+- -+- -+- -+- -+- -+- -+- -+- -+- -+- -+-
        constexpr float labelWidth = 120.f;
        _headerLayout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

        _logicalPath = _headerLayout.addChildComponent<LabelRow<TextInput>>("ID", labelWidth);
        _logicalPath->input->setFlex(Flex::FlexWidth);
        _logicalPath->input->setReadOnly(true);

        _assetType = _headerLayout.addChildComponent<LabelRow<TextInput>>("Type", labelWidth);
        _assetType->input->setFlex(Flex::FlexWidth);
        _assetType->input->setReadOnly(true);

        // -+- -+- -+- -+- -+- -+- -+- -+- -+- -+- -+-
        auto deleteKey
            = _keyboardManager.getOrCreate("Delete selected component", Keyboard::Key::Delete);
        deleteKey->setIsRepeatable(false);
        _subscriptionPool << deleteKey->onPress->subscribeAndGetID(
            [&](auto) { removeCurrentComponent(_targetComponent); });
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

        int id = 0;
        drawTreeNode(_targetAsset->getData().get(), id);
    }

    void NxECSBasedEditorEWC::onDiscardChanges()
    {
    }

    void NxECSBasedEditorEWC::onTreeContextMenuBehavior()
    {
        NxEditorBaseEditorEWC::onTreeContextMenuBehavior();

        if (ImGui::MenuItem(ICON_FA_PLUS " Add new component"))
        {
            ModalECSSearchPopUpEWC::Open("Choose new component",
                                         [this](BaseComponent::Ptr newComp)
                                         {
                                             _targetAsset->getData()->attachChild(newComp);
                                             updateGuiBasedOnAsset();
                                             makeDirty();
                                         });
        }
    }

    void NxECSBasedEditorEWC::onSave()
    {
        if (!hasTarget())
        {
            return;
        }

        nlohmann::json assetData;

#if defined(DEBUG)
        int counter = 0;
        const ECSEditorMimeAdapter* firstAdapter = nullptr;
        for (auto&& child : _children)
        {
            if (const auto* adapter = child->tryCastTo<ECSEditorMimeAdapter>())
            {
                if (adapter->isEnabled()
                    && adapter->isDirectWorkingMatchWith(_targetAsset->getData().get()))
                {
                    if (!firstAdapter)
                    {
                        firstAdapter = adapter;
                    }
                    if (++counter > 1)
                    {
                        if (counter == 2)
                        {
                            errorLog("First asset's adapter (main): '{}'"_f
                                     << firstAdapter->getComponentType());
                        }

                        errorLog(
                            "More than one adapter is enabled as main one for asset '{}'. Second adapter is: '{}'"_f
                            << _targetAsset->getLogicPath() << adapter->getComponentType());
                        Assert(false);
                    }
                }
            }
        }
#endif

        for (auto&& child : _children)
        {
            if (auto* adapter = child->tryCastTo<ECSEditorMimeAdapter>())
            {
                if (adapter->canWorkWith(_targetAsset->getData().get()))
                {
                    assetData = adapter->packAssetDataFromObject();
                    break;
                }
            }
        }

        _targetAsset->syncAssetWithMemory(assetData);
    }

    void NxECSBasedEditorEWC::updateGuiBasedOnAsset()
    {
        if (!hasTarget())
        {
            return;
        }

        ImGui::ClearActiveID();

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

    void NxECSBasedEditorEWC::onUpdate()
    {
        NxEditorBaseEditorEWC::onUpdate();

        _keyboardManager.update();
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

    void NxECSBasedEditorEWC::drawTreeNode(BaseComponent* comp, int& id)
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

        ImGui::PushID(++id);

        StringAtom name = comp->getComponentName();
        if (name.isEmpty())
        {
            name = id == 1 ? "Root" : "<no name>";
        }
        const bool isOpened = ImGui::TreeNodeEx(name.c_str(), flags);

        if (isDisabled)
        {
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }

        if (ImGui::IsItemClicked() || (ImGui::IsItemFocused() && isHovered()))
        {
            const auto* old = _targetComponent;
            _targetComponent = comp;

            if (old != comp)
            {
                updateGuiBasedOnAsset();
            }
        }

        // Controlling over the chosen node
        bool shouldInvalidate = false;
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("ECSBasedEditorTreeContextMenu");
        }

        if (ImGui::BeginPopup("ECSBasedEditorTreeContextMenu"))
        {
            if (ImGui::MenuItem(ICON_FA_PLUS " Add new component"))
            {
                ModalECSSearchPopUpEWC::Open("Choose new component",
                                             [this](BaseComponent::Ptr newComp)
                                             {
                                                 _targetComponent->attachChild(newComp);
                                                 updateGuiBasedOnAsset();
                                                 makeDirty();
                                             });
            }

            if (comp->hasParent() && ImGui::MenuItem(ICON_FA_TRASH " Delete"))
            {
                removeCurrentComponent(comp);
                comp = nullptr;
                shouldInvalidate = true;
            }

            ImGui::EndPopup();
        }

        if (isOpened)
        {
            if (!shouldInvalidate)
            {
                for (auto&& child : comp->getChildren())
                {
                    drawTreeNode(child.get(), id);
                }
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
        _targetAsset.reset();
    }

    void NxECSBasedEditorEWC::setup()
    {
        if (!_targetAsset)
        {
            return;
        }

        if (_targetAsset->getLoadingStatus() != ECSAsset::Status::Loaded)
        {
            Assert(false);
            errorLog("Can't setup editor's tree. Asset '{}' is not loaded properly."_f
                     << _targetAsset->getLogicPath());
            return;
        }

        updateGuiBasedOnAsset();
    }

    void NxECSBasedEditorEWC::removeCurrentComponent(BaseComponent* comp)
    {
        if (!comp || !comp->hasParent())
        {
            return;
        }

        bool resetToParent = false;
        if (_targetComponent == comp)
        {
            resetToParent = true;
        }

        comp->getParent()->removeChild(comp);

        if (resetToParent)
        {
            _targetComponent = _targetAsset->getData().get();
        }

        updateGuiBasedOnAsset();
        makeDirty();
    }

} // namespace Core