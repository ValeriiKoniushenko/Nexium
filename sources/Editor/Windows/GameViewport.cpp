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

#include "GameViewport.h"

#include "GameplaySystem/Framework/GameInstance.h"
#include "ImGui/imgui_internal.h"
#include "Misc/IconsFontAwesome.h"

namespace Core
{
    ECS_IMPL(GameViewportEWC);

    //
    //     _   _  _                                      _
    //    | | | |(_)                                    | |
    //    | | | | _   ___ __      __ _ __    ___   _ __ | |_
    //    | | | || | / _ \\ \ /\ / /| '_ \  / _ \ | '__|| __|
    //    \ \_/ /| ||  __/ \ V  V / | |_) || (_) || |   | |_
    //     \___/ |_| \___|  \_/\_/  | .__/  \___/ |_|    \__|
    //                              | |
    //                              |_|
    // ========================================================================

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
        if (gGameInstance->renderMode == GameInstance::RenderMode::Editor)
        {
            const auto& r = gGameInstance->gameViewport;
            const glm::vec2 renderSize = { static_cast<float>(r.getRenderSize().width),
                                           static_cast<float>(r.getRenderSize().height) };

            glm::vec2 p = {};

            if (isFocused())
            {
                p = ImGui::GetCursorScreenPos();
            }

            ImGui::Image(r.getTextureId(), glm::vec2(renderSize.x, renderSize.y),
                         glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f));

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
    }
} // namespace Core