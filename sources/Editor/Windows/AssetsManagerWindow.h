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

#pragma once

#include "BaseWindow.h"
#include "Graphics/Texture.h"

namespace Core
{

    class AssetsManagerWindowEWC : public BaseFloatEWC
    {
        ECS_REGISTER_NEW_COMPONENT(AssetsManagerWindowEWC, BaseFloatEWC);

    public:
        enum class NodeType
        {
            Default,
            Code,
            Image,
            Folder
        };

    public:
        // Pre-launch settings TODO: MOVE IT!!!
        std::filesystem::path assetsPath = "assets";

    protected:
        void onInit() override;
        void onDraw() override;
        void onUpdate() override;

    protected:
        struct CacheNode
        {
            NodeType type = NodeType::Default;
            std::filesystem::path path;
            std::vector<CacheNode> children;
        };

    protected:
        std::unordered_map<NodeType, Core::Texture> _nodeTypesData;
        CacheNode _rootCacheNode;
        StringAtom _filterBuf;
        std::filesystem::path _openedPath = assetsPath;
        int _commonTreeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick;
        ImVec2 _thumbnailSize = ImVec2(70, 70);
        bool _renderFilesInTreeView = false;
        float _defaultGap = 4.f;
        float _toolbarToolsWidth = 150.f;

    private:
        void drawExplorerTree();
        void drawExplorer();
        void drawExplorerToolbar();
        void drawOneLevel(CacheNode& node, bool& isSelected);
        bool drawFileThumbnail(ImTextureID texture, const std::filesystem::directory_entry& entry,
                               ImVec2 size);
        void rescanPhysicalDrive(CacheNode& node);
        void refresh();

        [[nodiscard]] NodeType getNodeType(const std::filesystem::directory_entry& entry);
    };
} // namespace Core
