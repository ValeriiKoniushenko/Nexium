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

#pragma once

#include "../BaseWindow.h"
#include "AssetsManager/AssetsManager.h"
#include "AssetsManager/TextureAsset.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
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
        R_FRIEND_DECL(Core::AssetsManagerWindowEWC, Core::BaseFloatEWC);
        ECS_COMPONENT_DECL(AssetsManagerWindowEWC, BaseFloatEWC);

    public:
        constexpr static std::string_view defaultNewFileName = "NewFile";

    public:
        void tryOpenParentDir();
        void tryOpenPath(const std::filesystem::path& p);

        [[nodiscard]] const char* getIcon() override;

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

        void selectPath(const std::filesystem::path& path, bool additive);
        void toggleSelection(const std::filesystem::path& path);

        void drawExplorerContextMenu();

        void drawExplorerTree();
        void drawExplorer();

        void drawExplorerItems(const std::vector<std::filesystem::directory_entry>& entries,
                               int maxCountPerWidth, const ThumbnailActions& actions);

        void drawOneLevel(CacheNode& rootNode, bool& isSelected);

        void rescanPhysicalDrive(CacheNode& node);

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
