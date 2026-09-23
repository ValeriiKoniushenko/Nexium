// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "GameViewport.h"

#include "Editor/EditorIntegration.h"
#include "Editor/IconsFontAwesome.h"
#include "ImGui/imgui_internal.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/GameInstance.h"
#include "Scenes/SceneTabs/SceneTabs.h"

using namespace NX;

namespace NX
{
    ECS_IMPL(GameViewportEWC);

    const char* GameViewportEWC::getIcon()
    {
        return ICON_FA_VIDEO_CAMERA;
    }

    void GameViewportEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        setComponentName("Viewport"_atom);
        _sceneTabs.initialize(*GetSceneManager());
    }

    void GameViewportEWC::onUpdate()
    {
        BaseFloatEWC::onUpdate();
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows)
            && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            ImGui::FocusWindow(ImGui::GetCurrentWindow());
        }
    }

    void GameViewportEWC::onDraw()
    {
        _blocksPicking = false;
        if (gGameInstance->renderMode != GameInstance::RenderMode::Editor)
        {
            return;
        }

        _sceneTabs.draw(*GetSceneManager());

        // Picking and rendering use the image area below the scene tabs.
        _innerPosition = ImGui::GetCursorScreenPos();
        const auto available = ImGui::GetContentRegionAvail();
        if (available.x < 1.f || available.y < 1.f)
        {
            return;
        }
        const FSize2 imageSize{ available.x, available.y };
        if (_innerSize != imageSize)
        {
            _innerSize = imageSize;
            onSizeChanged->trigger(_size, _innerSize);
        }

        const auto& r = GetEditor()->gameViewport;
        const glm::vec2 renderSize = { static_cast<float>(r.getRenderSize().width),
                                       static_cast<float>(r.getRenderSize().height) };

        const auto p = ImGui::GetCursorScreenPos();

        ImGui::Image(r.getTextureId(), glm::vec2(renderSize.x, renderSize.y), glm::vec2(0.0f, 1.0f),
                     glm::vec2(1.0f, 0.0f));

        const auto& selected = GetObjectSelectorManager()->getSelectedObjects();
        auto* camera = GetWorld()->currentCamera;
        if (selected.size() == 1 && camera && renderSize.x > 0.f && renderSize.y > 0.f)
        {
            if (auto* object = dynamic_cast<SceneObject*>(selected.begin()->second.get());
                object && object != camera && object->isEnabled())
            {
                _gizmo.draw(*object, *camera, p, renderSize);
                _blocksPicking = _gizmo.blocksPicking();
            }
        }

        if (isFocused())
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRect(p, glm::vec2(p.x + renderSize.x, p.y + renderSize.y),
                              IM_COL32(255, 255, 0, 255), // Yellow
                              0.0f,                       // Rounding
                              0,                          // Flags
                              2.0f                        // Border thickness
            );
        }
    }
} // namespace NX
