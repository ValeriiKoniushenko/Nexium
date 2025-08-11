// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "SceneTreeWindow.h"

#include "GameplaySystem/Framework/GameInstance.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "Misc/IconsFontAwesome.h"
#include "ObjectPropertiesWindow.h"
#include "Scene/Scene.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(SceneTreeWindowEWC)

    // ========================================================================
    //
    //     _____                           _____
    //    /  ___|                         |_   _|
    //    \ `--.   ___   ___  _ __    ___   | |   _ __   ___   ___
    //     `--. \ / __| / _ \| '_ \  / _ \  | |  | '__| / _ \ / _ \
        //    /\__/ /| (__ |  __/| | | ||  __/  | |  | |   |  __/|  __/
    //    \____/  \___| \___||_| |_| \___|  \_/  |_|    \___| \___|
    // ========================================================================
    void SceneTreeWindowEWC::onInit()
    {
        BaseFloatEWC::onInit();

        setScene(&gameInstance->gameScene);
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

        ImGui::TextUnformatted("Name: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN); // Makes the next widget take full width
        ImGui::InputText("##sceneName", &sceneName, extraFlag);
        ImGui::PopItemWidth();

        if (!_scene)
        {
            return;
        }

        auto& graphicNodes = _scene->getStaticMeshBundles();
        if (ImGui::TreeNodeEx("Graphic nodes", ImGuiTreeNodeFlags_DefaultOpen | _commonTreeFlags))
        {
            int32_t internalId = 0;
            for (auto& node : graphicNodes)
            {
                drawTreeNode(&node, internalId++);
            }
            ImGui::TreePop();
        }

        auto& logicalNodes = _scene->getLogicalComponents();
        if (ImGui::TreeNodeEx("Logical nodes", ImGuiTreeNodeFlags_DefaultOpen | _commonTreeFlags))
        {
            int32_t internalId = 0;
            for (auto& node : logicalNodes)
            {
                drawTreeNode(node.get(), internalId++);
            }
            ImGui::TreePop();
        }

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

        int flags = _commonTreeFlags | ImGuiTreeNodeFlags_OpenOnArrow;

        if (selectedObject == n && !isInSelectedSubtree)
        {
            isInSelectedSubtree = true;
        }

        if (isInSelectedSubtree)
        {
            flags |= ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_SpanAvailWidth;
        }

        if (n->getChildrenCount() == 0)
        {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        ImGui::PushID(id);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

        bool isEnabled = n->isEnabled();
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
        const bool isOpened = ImGui::TreeNodeEx(n->getComponentName().c_str(), flags);
        if (!isEnabled)
        {
            ImGui::PopStyleColor(1);
        }

        if (ImGui::IsItemClicked() || ImGui::IsItemFocused())
        {
            selectedObject = n;
            if (auto* wnd = gameInstance->gameEditor.getWindow<ObjectPropertiesWindowEWC>())
            {
                if (wnd->isEnabled())
                {
                    wnd->setTargetObject(selectedObject);
                }
            }
        }

        if (isOpened)
        {
            for (auto&& child : n->getChildren())
            {
                drawTreeNode(child.get(), ++id);
            }

            ImGui::TreePop();
        }

        ImGui::PopID();
    }
} // namespace Core
