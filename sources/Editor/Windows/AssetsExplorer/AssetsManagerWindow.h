// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "../BaseWindow.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "NxSubsystems/AssetsManager/AssetsManager.h"
#include "NxSubsystems/AssetsManager/TextureAsset.h"
#include "RenamePopUpWindow.h"
#include "ThumbnailFile.h"

namespace Core
{
    namespace Gui
    {
        template<Utils::IsArithmetic T>
        class NumInput;

        using IntInput = NumInput<int>;

        class TextInput;
        class Button;
    } // namespace Gui

    CLASS();
    class AssetsManagerWindowEWC : public BaseFloatEWC
    {
        ECS_DECL(AssetsManagerWindowEWC, Core::BaseFloatEWC);

    public:
        constexpr static std::string_view defaultNewFileName = "NewFile";

    public:
        void tryOpenParentDir();
        void tryOpenPath(const std::filesystem::path& p);

        [[nodiscard]] const char* getIcon() override;

        [[nodiscard]] std::filesystem::path getOpenedPath() const { return _openedPath; }

    protected:
        void onPreInitialize() override;
        void onInitialize() override;

        void onDraw() override;
        void onUpdate() override;
        void openPath(const std::filesystem::path& path);

        void copyFrom(const std::filesystem::path& path);
        void cutFrom(const std::filesystem::path& path);
        void pasteTo(std::filesystem::path path);
        void deleteAt(const std::filesystem::path& path);
        void createFolder(const std::filesystem::path& path);
        void createFolderAutoName(const std::filesystem::path& basePath);
        void createFile(const std::filesystem::path& path);
        void createFileAutoName(const std::filesystem::path& basePath);

        [[nodiscard]] std::filesystem::path getExclusiveFileName(
            const std::filesystem::path& path) const;

    protected:
        struct CacheNode
        {
            AssetsManager::NodeType type = AssetsManager::NodeType::Default;
            std::filesystem::path path;
            std::vector<CacheNode> children;
        };

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;

        std::unordered_map<AssetsManager::NodeType, NXTexture> _nodeTypesData;

        Gui::HorizontalLayout _toolbarLayout;
        Gui::Button* _refreshButton = nullptr;
        Gui::Button* _homeButton = nullptr;
        Gui::Button* _backButton = nullptr;
        Gui::TextInput* _pathInput = nullptr;
        Gui::TextInput* _filterInput = nullptr;

        CacheNode _rootCacheNode;
        std::filesystem::path _selectedPath;
        std::vector<std::filesystem::path> _selectedPaths;

        glm::vec2 _thumbnailSize = glm::vec2(70, 70);
        int _commonTreeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick;
        bool _renderFilesInTreeView = false;

        void refresh();

    private:
        [[nodiscard]] bool isFiltered(const std::filesystem::path& p) const;
        [[nodiscard]] bool isSelected(const std::filesystem::path& p) const;
        [[nodiscard]] std::vector<std::filesystem::directory_entry> prepareExplorerEntries() const;

        void saveThumbnailSelectionAfterRename(const std::filesystem::path& oldPath,
                                               const std::filesystem::path& newPath);

        ThumbnailActions buildThumbnailActions();

        void deleteSelectedFiles();
        void toggleSelection(const std::filesystem::path& path);
        void selectPath(const std::filesystem::path& path, bool additive);

        void drawExplorerContextMenu();

        void drawExplorerTree();
        void drawExplorer();

        void drawExplorerItems(const std::vector<std::filesystem::directory_entry>& entries,
                               int maxCountPerWidth, const ThumbnailActions& actions);

        void drawOneLevel(CacheNode& rootNode, bool& isSelected);

        void rescanPhysicalDrive(CacheNode& node);

        void drawAssetsContextMenu(const std::filesystem::directory_entry& entry, bool& invalidate,
                                   bool& needOpen);

        static std::filesystem::path Normalize(const std::filesystem::path& p)
        {
            std::error_code ec;

            auto canonical = std::filesystem::weakly_canonical(p, ec);

            if (!ec)
            {
                return canonical;
            }

            return p.lexically_normal();
        }

    private:
        std::filesystem::path _openedPath;
        bool _isCopy = true;
    };
} // namespace Core

#include "AssetsManagerWindow.generated.h"
