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

#include "SceneTreeWindow.h"

#include "AssetsExplorer/AssetsManagerWindow.h"
#include "GameplaySystem/Entities/Actor.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "ImGui/imgui.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "Misc/IconsFontAwesome.h"
#include "ModalAssetsSearchPopUp.h"
#include "Scene/Scene.h"

namespace Core
{
    ECS_COMPONENT_IMPL(SceneTreeWindowEWC);
    R_FRIEND_IMPL(SceneTreeWindowEWC);

    //
    //     _____                           _____
    //    /  ___|                         |_   _|
    //    \ `--.   ___   ___  _ __    ___   | |   _ __   ___   ___
    //     `--. \ / __| / _ \| '_ \  / _ \  | |  | '__| / _ \ / _ \
    //    /\__/ /| (__ |  __/| | | ||  __/  | |  | |   |  __/|  __/
    //    \____/  \___| \___||_| |_| \___|  \_/  |_|    \___| \___|
    // ========================================================================

    SceneTreeWindowEWC::SceneTreeWindowEWC(const StringAtom& name)
        : BaseFloatEWC(componentType, name),
          _commonTreeFlags(ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth)
    {
    }

    void SceneTreeWindowEWC::highlightSpecificObject(const SceneObject* obj)
    {
        if (!obj) [[unlikely]]
        {
            return;
        }
        _highlightTracerObject = obj;
    }

    const char* SceneTreeWindowEWC::getIcon()
    {
        return ICON_FA_GLOBE;
    }

    void SceneTreeWindowEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        setScene(&gGameInstance->gameScene);

        _subscriptionPool << gGameInstance->objectSelectorManager.onChange->subscribeAndGetID(
            [this](BaseComponent* comp, bool newValue)
            {
                if (newValue)
                {
                    selectedObject = comp;
                }
                else
                {
                    selectedObject = nullptr;
                }
            });
    }

    void SceneTreeWindowEWC::onDraw()
    {
        std::string sceneName;
        int extraFlag = ImGuiInputTextFlags_ReadOnly;
        if (_scene)
        {
            sceneName = _scene->getSceneName();
            extraFlag = 0;
        }

        processAddNewComponentButton();

        ImGui::SameLine();
        ImGui::Dummy({});
        ImGui::SameLine();
        ImGui::TextUnformatted("Name: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN); // Makes the next widget take full width
        ImGui::InputText("##sceneName", &sceneName, extraFlag);
        ImGui::PopItemWidth();

        if (!_scene)
        {
            return;
        }

        int32_t internalId = 0;
        for (auto&& object : _scene->getObjects())
        {
            if (!drawTreeNode(object.get(), internalId++))
            {
                break;
            }
        }

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)
            && (ImGui::IsMouseClicked(ImGuiMouseButton_Left)
                || ImGui::IsMouseClicked(ImGuiMouseButton_Right)))
        {
            _highlightTracerObject = nullptr;
        }

        _lastSelectedObject = selectedObject;

        if (sceneName != _scene->getSceneName())
        {
            _scene->setSceneName(sceneName.c_str());
        }
    }

    void SceneTreeWindowEWC::onUpdate()
    {
        BaseFloatEWC::onUpdate();
    }

    bool SceneTreeWindowEWC::drawTreeNode(BaseComponent* n, int32_t id,
                                          bool isInSelectedSubtree /* = false*/)
    {
        if (!n)
        {
            return true;
        }

        if (const auto* actor = n->tryCastTo<Actor>())
        {
            // Trying to exclude NOW any post draw actors.
            if (actor->isPostDraw())
            {
                return true;
            }
        }

        int flags = _commonTreeFlags | ImGuiTreeNodeFlags_OpenOnArrow;

        if (selectedObject == n && !isInSelectedSubtree)
        {
            isInSelectedSubtree = true;
        }

        if (isInSelectedSubtree || gGameInstance->objectSelectorManager.isSelected(n))
        {
            flags |= ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_SpanAvailWidth;
        }

        if (!n->hasChildren())
        {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        ImGui::PushID(id);

        ImGui::PushStyleColor(ImGuiCol_Button, glm::vec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, glm::vec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, glm::vec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

        bool const isEnabled = n->isEnabled();
        if (!isEnabled)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        }

        if (ImGui::Button(isEnabled ? ICON_FA_EYE : ICON_FA_EYE_SLASH))
        {
            n->setEnabled(!isEnabled);
        }
        if (!isEnabled)
        {
            ImGui::PopStyleColor(1);
        }
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        if (!isEnabled)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        }

        auto componentName = n->getComponentName();
        if (componentName.isEmpty())
        {
            componentName = "<no name>";
        }

        if (_highlightTracerObject && _highlightTracerObject->IsSelfOrDescendantOf(n))
        {
            ImGui::SetNextItemOpen(true);
        }

        if (_highlightTracerObject == n)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        const bool isOpened = ImGui::TreeNodeEx(componentName.c_str(), flags);
        if (!isEnabled)
        {
            ImGui::PopStyleColor(1);
        }

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("ContextSceneItemMenu");
        }
        else if (ImGui::IsItemClicked() || (ImGui::IsItemFocused() && isHovered()))
        {
            selectedObject = n;

            if (_lastSelectedObject != selectedObject)
            {
                gGameInstance->objectSelectorManager.selectObject(n);
                ImGui::PopID();
                if (isOpened)
                {
                    ImGui::TreePop();
                }
                return false;
            }
        }

        bool invalidate = false;
        bool fullInvalidate = false;
        if (ImGui::BeginPopup("ContextSceneItemMenu"))
        {
            if (auto* camera = n->tryCastTo<BaseCamera>())
            {
                if (ImGui::MenuItem("Set as a main camera"))
                {
                    gGameInstance->currentCamera = camera;
                }
            }
            if (ImGui::MenuItem(ICON_FA_TRASH " Delete"))
            {
                gGameInstance->gameScene.deleteFromScene(n);
                invalidate = true;
            }
            if (ImGui::MenuItem(ICON_FA_CLONE " Duplicate"))
            {
                gGameInstance->gameScene.duplicateSceneObject(n);
                invalidate = fullInvalidate = true;
            }
            const auto* sceneObj = dynamic_cast<SceneObject*>(n);
            if (sceneObj && sceneObj->hasReferencedAsset() && ImGui::MenuItem("Show derived .nx"))
            {
                auto* wnd = gGameInstance->gameEditor.getWindow<AssetsManagerWindowEWC>();
                if (wnd)
                {
                    wnd->requestFocus();
                    auto asset = GetAssetsManager().getEcsAsset(sceneObj->getReferencedAsset());
                    wnd->tryOpenPath(asset->getMeta().pathToSource);
                }
            }

            ImGui::EndPopup();
        }

        if (isOpened)
        {
            if (!invalidate)
            {
                for (auto& child : n->getChildren())
                {
                    if (!drawTreeNode(child.get(), ++id))
                    {
                        ImGui::PopID();
                        ImGui::TreePop();
                        return false;
                    }
                }
            }

            ImGui::TreePop();
        }

        ImGui::PopID();

        return !fullInvalidate;
    }

    void SceneTreeWindowEWC::processAddNewComponentButton()
    {
        if (ImGui::Button(ICON_FA_PLUS, { ImGui::GetFrameHeight(), ImGui::GetFrameHeight() }))
        {
            ModalAssetsSearchPopUpEWC::Open("Choose a scene asset"_dyn);
        }
    }
} // namespace Core