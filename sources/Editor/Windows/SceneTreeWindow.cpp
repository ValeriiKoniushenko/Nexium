// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "SceneTreeWindow.h"

#include "AssetsExplorer/AssetsManagerWindow.h"
#include "Editor/EditorIntegration.h"
#include "ImGui/imgui.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "Misc/IconsFontAwesome.h"
#include "ModalAssetsSearchPopUp.h"
#include "NxWorld/Entities/Actor.h"
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Scene/Scene.h"

namespace Core
{
    ECS_IMPL(SceneTreeWindowEWC);

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

        _subscriptionPool << GetObjectSelectorManager()->onChange->subscribeAndGetID(
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
        for (auto object : _scene->getObjects())
        {
            drawTreeNode(object.get(), internalId++);
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

    void SceneTreeWindowEWC::drawTreeNode(BaseComponent* n, int32_t id,
                                          bool isInSelectedSubtree /* = false*/)
    {
        if (!n)
        {
            return;
        }

        if (const auto* actor = n->tryCastTo<Actor>())
        {
            // Trying to exclude NOW any post draw actors.
            if (actor->isPostDraw())
            {
                return;
            }
        }

        int flags = _commonTreeFlags | ImGuiTreeNodeFlags_OpenOnArrow;

        if (selectedObject == n && !isInSelectedSubtree)
        {
            isInSelectedSubtree = true;
        }

        if (isInSelectedSubtree || GetObjectSelectorManager()->isSelected(n))
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
        else if (ImGui::IsItemFocused() && isHovered())
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Delete))
            {
                gGameInstance->gameScene.deleteFromSceneOrFromObject(n);
            }
            else if (ImGui::IsItemClicked())
            {
                selectedObject = n;
                if (_lastSelectedObject != selectedObject)
                {
                    GetObjectSelectorManager()->selectSingleObject(n);
                }
            }
        }

        if (ImGui::BeginPopup("ContextSceneItemMenu"))
        {
            if (ImGui::MenuItem(ICON_FA_TRASH " Delete"))
            {
                gGameInstance->gameScene.deleteFromSceneOrFromObject(n);
            }
            if (ImGui::MenuItem(ICON_FA_CLONE " Duplicate"))
            {
                gGameInstance->gameScene.duplicateSceneObject(n);
            }

            if (auto* camera = n->tryCastTo<BaseCamera>())
            {
                if (ImGui::MenuItem("Set as a main camera"))
                {
                    GetWorld()->currentCamera = camera;
                }
            }

            const auto* sceneObj = dynamic_cast<SceneObject*>(n);
            if (sceneObj && sceneObj->hasReferencedAsset() && ImGui::MenuItem("Show derived .nx"))
            {
                auto* wnd = GetEditor()->getWindow<AssetsManagerWindowEWC>();
                if (wnd)
                {
                    wnd->requestFocus();
                    auto asset = GetAssetsManager()->getEcsAsset(sceneObj->getReferencedAsset());
                    wnd->tryOpenPath(asset->getMeta().pathToSource);
                }
            }

            ImGui::EndPopup();
        }

        if (isOpened)
        {
            for (auto& child : n->getChildren())
            {
                drawTreeNode(child.get(), ++id);
            }

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void SceneTreeWindowEWC::processAddNewComponentButton()
    {
        if (ImGui::Button(ICON_FA_PLUS, { ImGui::GetFrameHeight(), ImGui::GetFrameHeight() }))
        {
            ModalAssetsSearchPopUpEWC::Open("Choose a scene asset"_dyn);
        }
    }
} // namespace Core
