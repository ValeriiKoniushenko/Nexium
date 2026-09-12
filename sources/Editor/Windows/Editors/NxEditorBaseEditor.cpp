// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxEditorBaseEditor.h"

#include "ImGui/imgui.h"
#include "Misc/IconsFontAwesome.h"
#include "NxSubsystems/AssetsManager/AssetsManager.h"

namespace Core
{
    ECS_IMPL(NxEditorBaseEditorEWC);

    const char* NxEditorBaseEditorEWC::getIcon()
    {
        return ICON_FA_COG;
    }

    std::filesystem::path NxEditorBaseEditorEWC::getAssetFilePath() const noexcept
    {
        return _assetFilePath;
    }

    void NxEditorBaseEditorEWC::save()
    {
        onSave();
        _isDirty = false;
    }

    void NxEditorBaseEditorEWC::openFromFileSystem()
    {
        const std::vector filters(_fileFilters.begin(), _fileFilters.end());
        openFromPath(AssetsManager::OpenFileSelectionDialog(filters).toStdStringView());
    }

    void NxEditorBaseEditorEWC::openFromPath(const std::filesystem::path& path)
    {
        if (path.empty())
        {
            return;
        }

        _assetFilePath = path;
        if (!onOpenFromPath(path))
        {
            traceLog(
                "Can't continue to process asset's opening due to some errors. Check logs for "
                "details.");
            _assetFilePath.clear();
            return;
        }

        onFinishOpenFromPath(path);
    }

    void NxEditorBaseEditorEWC::discardChanges()
    {
        if (!_assetFilePath.empty())
        {
            onDiscardChanges();
            _isDirty = false;
            updateGuiBasedOnAsset();
        }
    }

    void NxEditorBaseEditorEWC::putArguments(const StringAtom& args)
    {
        BaseFloatEWC::putArguments(args);
        openFromPath(args.toStdStringView());
    }

    void NxEditorBaseEditorEWC::appendFileFilter(const std::string& filter)
    {
        _fileFilters.emplace(filter);
    }

    void NxEditorBaseEditorEWC::removeFileFilter(const std::string& filter)
    {
        _fileFilters.erase(filter);
    }

    void NxEditorBaseEditorEWC::clearFileFilters()
    {
        _fileFilters.clear();
    }

    const std::set<std::string>& NxEditorBaseEditorEWC::getFileFilters() const noexcept
    {
        return _fileFilters;
    }

    void NxEditorBaseEditorEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        _windowFlags |= ImGuiWindowFlags_MenuBar;
    }

    void NxEditorBaseEditorEWC::onDraw()
    {
        drawMenuBar();

        const float maxWidth = std::min(800.f, ImGui::GetContentRegionAvail().x * 0.7f);

        if (_enablePreview && _lastPropertiesTreeWidth != -1.f
            && _lastPropertiesTreeWidth > maxWidth)
        {
            ImGui::SetNextWindowSize(glm::vec2(maxWidth, 0), ImGuiCond_Always);
        }

        if (ImGui::BeginChild("Properties_and_Tree",
                              glm::vec2(_enablePreview ? _defaultPropertiesWidth : 0, 0),
                              _enablePreview ? ImGuiChildFlags_ResizeX : 0))
        {
            _lastPropertiesTreeWidth = ImGui::GetContentRegionAvail().x;

            if (_enableTree)
            {
                if (ImGui::BeginChild("Tree", glm::vec2(0, 100.f),
                                      ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY))
                {
                    treeContextMenuBehavior();
                    onDrawTree();
                }
                ImGui::EndChild();
            }

            if (ImGui::BeginChild("Properties"))
            {
                onDrawProperties();
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        if (_enablePreview)
        {
            ImGui::SameLine();

            if (ImGui::BeginChild("Preview"))
            {
                onDrawPreview();
            }
            ImGui::EndChild();
        }

        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
        {
            bool const ctrl = ImGui::GetIO().KeyCtrl;
            if (ctrl && ImGui::IsKeyPressed(ImGuiKey_S, false))
            {
                save();
            }
        }
    }

    void NxEditorBaseEditorEWC::treeContextMenuBehavior()
    {
        if (ImGui::BeginPopupContextWindow("ExplorerContextMenu",
                                           ImGuiPopupFlags_MouseButtonRight
                                               | ImGuiPopupFlags_NoOpenOverItems))
        {
            onTreeContextMenuBehavior();
            ImGui::EndPopup();
        }
    }

    void NxEditorBaseEditorEWC::drawMenuBar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem(ICON_FA_FILE_O " Open"))
                {
                    openFromFileSystem();
                }
                if (ImGui::MenuItem(ICON_FA_FLOPPY_O " Save"))
                {
                    save();
                }
                if (ImGui::MenuItem(ICON_FA_TIMES " Discard changes"))
                {
                    discardChanges();
                }
                onDrawBarExtraFileMenu();
                ImGui::Separator();
                if (ImGui::MenuItem(ICON_FA_POWER_OFF " Exit"))
                {
                    closeWindow();
                }

                ImGui::EndMenu();
            }

            if (!_assetFilePath.empty())
            {
                std::string str = (_isDirty ? "Modified" : "No changes");
                str += " | ";
                str += _assetFilePath.generic_string();

                ImGui::SameLine(ImGui::GetWindowWidth() - (ImGui::CalcTextSize(str.c_str()).x)
                                - ImGui::GetStyle().ItemSpacing.x * 3.f);
                ImGui::TextUnformatted(str.c_str());
            }

            ImGui::EndMenuBar();
        }
    }

    void NxEditorBaseEditorEWC::makeDirty()
    {
        _isDirty = true;
    }

    void NxEditorBaseEditorEWC::setEnablePreview(bool enable)
    {
        _enablePreview = enable;
    }

    void NxEditorBaseEditorEWC::setEnableTree(bool enable)
    {
        _enableTree = enable;
    }

} // namespace Core
