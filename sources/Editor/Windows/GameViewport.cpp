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
#include "ImGui/imgui_internal.h"
#include "Misc/IconsFontAwesome.h"
#include "NxWorld/Framework/GameInstance.h"

namespace Core
{
    ECS_IMPL(GameViewportEWC);

    const char* GameViewportEWC::getIcon()
    {
        return ICON_FA_VIDEO_CAMERA;
    }

    void GameViewportEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();
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
        if (gGameInstance->renderMode != GameInstance::RenderMode::Editor)
        {
            return;
        }

        const auto& r = GetEditor()->gameViewport;
        const glm::vec2 renderSize = { static_cast<float>(r.getRenderSize().width),
                                       static_cast<float>(r.getRenderSize().height) };

        glm::vec2 p = {};

        if (isFocused())
        {
            p = ImGui::GetCursorScreenPos();
        }

        ImGui::Image(r.getTextureId(), glm::vec2(renderSize.x, renderSize.y), glm::vec2(0.0f, 1.0f),
                     glm::vec2(1.0f, 0.0f));

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
} // namespace Core
