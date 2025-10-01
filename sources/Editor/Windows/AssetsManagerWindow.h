/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "AssetsManager/TextureAsset.h"
#include "BaseWindow.h"
#include "Editor/AssetsManager.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Graphics/Texture.h"
#include "Misc/JsonCacheable.h"

namespace Core
{
    class AssetsManagerWindowEWC : public BaseFloatEWC, public JsonCacheable
    {
        ECS_COMPONENT_DECL(AssetsManagerWindowEWC, BaseFloatEWC);

    public:
        ~AssetsManagerWindowEWC() override;

    public:
        void tryOpenParentDir();
        void tryOpenPath(const std::filesystem::path& p);

        [[nodiscard]] const char* getIcon() override { return ICON_FA_FOLDER; }

    protected:
        [[nodiscard]] StringAtom getCacheHash() const override;
        [[nodiscard]] nlohmann::json toCacheData() const override;
        void fromCacheData(const nlohmann::json& json) override;

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
            EditorAssetsManager::NodeType type = EditorAssetsManager::NodeType::Default;
            std::filesystem::path path;
            std::vector<CacheNode> children;
        };

    protected:
        std::unordered_map<EditorAssetsManager::NodeType, NXTexture> _nodeTypesData;

        Gui::HorizontalLayout _toolbarLayout;
        Gui::Button* _refreshButton = nullptr;
        Gui::Button* _homeButton = nullptr;
        Gui::Button* _backButton = nullptr;
        Gui::TextInput* _pathInput = nullptr;
        Gui::TextInput* _filterInput = nullptr;

        CacheNode _rootCacheNode;
        std::filesystem::path _selectedPath;

        glm::vec2 _thumbnailSize = glm::vec2(70, 70);
        int _commonTreeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick;
        bool _renderFilesInTreeView = false;

    private:
        [[nodiscard]] bool isFiltered(const std::filesystem::path& p) const;

        void drawExplorerTree();

        void drawExplorer();

        void drawOneLevel(CacheNode& rootNode, bool& isSelected);

        void drawFileThumbnail(ImTextureID texture, const std::filesystem::directory_entry& entry,
                               glm::vec2 size);

        void rescanPhysicalDrive(CacheNode& node);

        void refresh();

    private:
        std::filesystem::path _openedPath;
        bool _isCopy = true;
    };
} // namespace Core