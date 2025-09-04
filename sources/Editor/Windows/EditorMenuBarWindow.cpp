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

#include "EditorMenuBarWindow.h"

#include "Editor/Windows/AssetsManagerWindow.h"
#include "Editor/Windows/EditorSettings.h"
#include "Editor/Windows/GameViewport.h"
#include "Editor/Windows/LogsWindow.h"
#include "Editor/Windows/ObjectPropertiesWindow.h"
#include "Editor/Windows/SceneTreeWindow.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Misc/IconsFontAwesome.h"
#include "ShaderManager.h"
#include "TextEditor.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(EditorMenuBarWindowEWC)

    void EditorMenuBarWindowEWC::onInitialize()
    {
        BaseMenuBarEWC::onInitialize();

        _slowUpdater.setRepeatTime(1. / 10.);
        _slowUpdater.setCallback(
            [this](auto)
            {
                _cachedFpsText
                    = _fpsText + StringAtom::MakeFrom(static_cast<int>(ImGui::GetIO().Framerate));
            });

        const auto& style = ImGui::GetStyle();
        _fpsTextSize = ImGui::CalcTextSize(_fpsTextTemplate).x + style.ItemSpacing.x;
        _simulationButton = ImGui::CalcTextSize(ICON_FA_PLAY_CIRCLE).x + style.ItemSpacing.x * 2.f;
    }

    void EditorMenuBarWindowEWC::onDraw()
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open"))
            {
            }
            if (ImGui::MenuItem("Save"))
            {
            }
            if (ImGui::MenuItem("Settings"))
            {
                gGameInstance->gameEditor.showWindow<EditorSettingsEWC>();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows"))
        {
            if (ImGui::MenuItem("Viewport"))
            {
                gGameInstance->gameEditor.showWindow<GameViewportEWC>();
            }
            if (ImGui::MenuItem("Logs"))
            {
                gGameInstance->gameEditor.showWindow<LogsWindowEWC>();
            }
            if (ImGui::MenuItem("Object properties"))
            {
                gGameInstance->gameEditor.showWindow<ObjectPropertiesWindowEWC>();
            }
            if (ImGui::MenuItem("Scene"))
            {
                gGameInstance->gameEditor.showWindow<SceneTreeWindowEWC>();
            }
            if (ImGui::MenuItem("Assets manager"))
            {
                gGameInstance->gameEditor.showWindow<AssetsManagerWindowEWC>();
            }
            if (ImGui::MenuItem("Text editor"))
            {
                gGameInstance->gameEditor.showWindow<TextEditorEWC>();
            }
            if (ImGui::MenuItem("Shader manager"))
            {
                gGameInstance->gameEditor.showWindow<ShaderManagerEWC>();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Deselect"))
            {
                gGameInstance->objectSelectorManager.deselectAllAndClear();
            }
            ImGui::EndMenu();
        }
        ImGui::SameLine(0, 0);

        const auto& style = ImGui::GetStyle();

        float offset = ImGui::GetWindowWidth();
        offset -= _fpsTextSize - style.ItemSpacing.x;
        ImGui::SetCursorPosX(offset);
        ImGui::TextUnformatted(_cachedFpsText.c_str());
        ImGui::SameLine(0, 0);
        offset -= style.ItemSpacing.x * 2.f;

        /*
        offset -= _simulationButton - style.ItemSpacing.x;
        ImGui::SetCursorPosX(offset);

        if (ToggleButton(ICON_FA_PLAY_CIRCLE, gGameInstance->gameEditor.getIsRunSimulation(),
                         BaseEWC::ColorSoftGreen, BaseEWC::ColorRed))
        {
            gGameInstance->gameEditor.toggleSimulation();
        }
        ImGui::SameLine(0,0);
        offset -= style.ItemSpacing.x * 2.f;
        */
    }

    void EditorMenuBarWindowEWC::onUpdate()
    {
        BaseMenuBarEWC::onUpdate();

        _slowUpdater.startOrUpdate();
    }

} // namespace Core