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

        constexpr float labelWidth = 80.0f;
        // -+- -+- -+- -+- -+- -+- -+- -+- -+- -+- -+-
        _headerLayout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

        _logicalPath = _headerLayout.addChildComponent<LabelRow<TextInput>>("ID", labelWidth);
        _logicalPath->input->setFlex(Flex::FlexWidth);
        _logicalPath->input->setReadOnly(true);

        _assetType = _headerLayout.addChildComponent<LabelRow<TextInput>>("Type", labelWidth);
        _assetType->input->setFlex(Flex::FlexWidth);
        _assetType->input->setReadOnly(true);

        // -+- -+- -+- -+- -+- -+- -+- -+- -+- -+- -+-
        _baseEcsLayout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

        _ecsName = _baseEcsLayout.addChildComponent<LabelRow<TextInput>>("Name", labelWidth);
        _ecsName->input->setFlex(Flex::FlexWidth);

        _ecsType = _baseEcsLayout.addChildComponent<LabelRow<TextInput>>("Type", labelWidth);
        _ecsType->input->setFlex(Flex::FlexWidth);
        _ecsType->input->setReadOnly(true);

        _ecsParent = _baseEcsLayout.addChildComponent<LabelRow<TextInput>>("Parent", labelWidth);
        _ecsParent->input->setFlex(Flex::FlexWidth);
        _ecsParent->input->setReadOnly(true);

        _ecsDisableTicks
            = _baseEcsLayout.addChildComponent<LabelRow<CheckBox>>("No ticks", labelWidth);

        _ecsChildren
            = _baseEcsLayout.addChildComponent<LabelRow<StringArray>>("Children", labelWidth);
        _ecsChildren->input->setReadOnly(true);
    }

    void NxECSBasedEditorEWC::onDrawProperties()
    {
        const auto dt = GetWorld().timeDelta;

        if (CollapsingHeader("Asset data", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _headerLayout.tick(dt);
        }

        if (CollapsingHeader("Base ECS properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _baseEcsLayout.tick(dt);
        }

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

        int id = 0;
        constexpr int baseFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick
                                  | ImGuiTreeNodeFlags_SpanAvailWidth
                                  | ImGuiTreeNodeFlags_OpenOnArrow;

        struct Frame
        {
            BaseComponent* p = nullptr;
            std::size_t index = 0;
        };

        std::stack<Frame> stack;
        auto* asset = _targetAsset->getData().get();
        stack.push({ asset, 0 });

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

            ImGui::PopID();

            if (asset->hasChildren() && stack.top().index < asset->getChildrenCount())
            {
                asset = asset->getChildAt(stack.top().index++).get();
                stack.push({ asset, 0 });
            }
            else
            {
                stack.pop();
                asset = stack.empty() ? nullptr : stack.top().p;
                if (isOpened)
                {
                    ImGui::TreePop();
                }

                if (!stack.empty() && asset != nullptr && asset->hasChildren())
                {
                    stack.top().index++;
                    if (stack.top().index < asset->getChildrenCount())
                    {
                        asset = asset->getChildAt(stack.top().index).get();
                        stack.push({ asset, 0 });
                    }
                }
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

        auto data = _targetAsset->getData();
        _ecsName->input->setInputtedData(data->getComponentName().toStdString());
        _ecsType->input->setInputtedData(data->getComponentType().toStdString());
        if (data->hasParent())
        {
            _ecsParent->input->setInputtedData(data->getParent()->getComponentName().toStdString());
        }
        _ecsDisableTicks->input->setValue(data->getNoTick());
        for (auto&& child : data->getChildren())
        {
            _ecsChildren->input->add("{} [{}]"_f << child->getComponentName()
                                                 << child->getComponentType());
        }

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