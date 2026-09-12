// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "EditorMenuBarWindow.h"

#include "AssetsExplorer/AssetsManagerWindow.h"
#include "ECSAdapters/EditorStaticMeshBundleAdapter.h"
#include "Editor/EditorIntegration.h"
#include "Editor/Windows/EditorSettings.h"
#include "Editor/Windows/GameViewport.h"
#include "Editor/Windows/LogsWindow.h"
#include "Editor/Windows/ObjectPropertiesWindow.h"
#include "Editor/Windows/SceneTreeWindow.h"
#include "Editors/TextEditor.h"
#include "ImageViewer.h"
#include "Misc/IconsFontAwesome.h"
#include "NxWorld/Framework/GameInstance.h"
#include "ShaderManager.h"
#include "WorldSettings.h"

namespace
{
    template<Core::IsEditorWindowComponent T>
    void WindowMenuItem()
    {
        if (ImGui::MenuItem(GetEditor()->getWindow<T>()->getComponentName().c_str()))
        {
            GetEditor()->showWindow<T>();
        }
    }
} // namespace

namespace Core
{
    ECS_IMPL(EditorMenuBarWindowEWC);

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
            if (ImGui::MenuItem(ICON_FA_FLOPPY_O " Save all"))
            {
                gGameInstance->saveAllToCache();
            }
            if (ImGui::MenuItem(ICON_FA_COG " Settings"))
            {
                GetEditor()->showWindow<EditorSettingsEWC>();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Deselect"))
            {
                GetObjectSelectorManager()->deselectAllAndClear();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows"))
        {
            WindowMenuItem<WorldSettingsEWC>();
            WindowMenuItem<GameViewportEWC>();
            WindowMenuItem<LogsWindowEWC>();
            WindowMenuItem<ObjectPropertiesWindowEWC>();
            WindowMenuItem<SceneTreeWindowEWC>();
            WindowMenuItem<AssetsManagerWindowEWC>();
            WindowMenuItem<ShaderManagerEWC>();
            ImGui::Separator();
            WindowMenuItem<TextEditorEWC>();
            WindowMenuItem<NxECSBasedEditorEWC>();
            WindowMenuItem<ImageViewerEWC>();

            ImGui::EndMenu();
        }

        ImGui::SameLine(0, 0);

        const auto& style = ImGui::GetStyle();

        const float offset = ImGui::GetWindowWidth() - (_fpsTextSize - style.ItemSpacing.x);
        ImGui::SetCursorPosX(offset);
        ImGui::TextUnformatted(_cachedFpsText.c_str());
        ImGui::SameLine(0, 0);

        /*
        offset -= style.ItemSpacing.x * 2.f;
        offset -= _simulationButton - style.ItemSpacing.x;
        ImGui::SetCursorPosX(offset);

        if (ToggleButton(ICON_FA_PLAY_CIRCLE, GetEditor()->getIsRunSimulation(),
                         BaseEWC::ColorSoftGreen, BaseEWC::ColorRed))
        {
            GetEditor()->toggleSimulation();
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
