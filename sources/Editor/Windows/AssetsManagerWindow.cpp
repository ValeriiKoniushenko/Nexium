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

#include "AssetsManagerWindow.h"

#include "GameplaySystem/Framework/GameInstance.h"
#include "Misc/IconsFontAwesome.h"
#include "TextEditor.h"

#include <format>

namespace
{

    [[maybe_unused]] bool IsSubpath(const std::filesystem::path& original,
                                    const std::filesystem::path& sub)
    {
        const auto rel = std::filesystem::relative(original, sub);
        return !rel.empty() && rel.native()[0] != '.';
    }

} // namespace

namespace Core
{
    ECS_COMPONENT_IMPL(AssetsManagerWindowEWC)

    void AssetsManagerWindowEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        const static std::unordered_map<NodeType, std::filesystem::path> paths
            = { { NodeType::Default, "assets/images/document.png" },
                { NodeType::Code, "assets/images/code_document.png" },
                { NodeType::Image, "assets/images/image_document.png" },
                { NodeType::Folder, "assets/images/folder.png" } };

        for (auto&& [type, path] : paths)
        {
            Texture tmp;
            if (tmp.loadFromFile(path, false))
            {
                _nodeTypesData.emplace(type, tmp);
            }
        }

        _filterBuf.resize(1024);

        auto& style = ImGui::GetStyle();
        _defaultGap = style.ItemSpacing.x;

        refresh();
    }

    void AssetsManagerWindowEWC::onDraw()
    {
        drawExplorerTree();
        ImGui::SameLine();
        drawExplorer();
    }

    void AssetsManagerWindowEWC::onUpdate()
    {
        BaseFloatEWC::onUpdate();
    }

    void AssetsManagerWindowEWC::drawExplorerTree()
    {
        if (ImGui::BeginChild("Explorer tree", glm::vec2(200.0f, 0), ImGuiChildFlags_ResizeX))
        {
            ImGui::Dummy({}); // extra padding
            bool isSelected = false;
            drawOneLevel(_rootCacheNode, isSelected);
            ImGui::Dummy({}); // extra padding
        }
        ImGui::EndChild();
    }

    void AssetsManagerWindowEWC::drawExplorer()
    {
        if (ImGui::BeginChild("Explorer"))
        {
            drawExplorerToolbar();

            auto& style = ImGui::GetStyle();
            const float oneThumbnailWidth
                = _thumbnailSize.x + (style.ItemSpacing.x * 2.f) + style.ItemSpacing.x;
            const int maxCountPerWidth
                = static_cast<int>(ImGui::GetContentRegionAvail().x / oneThumbnailWidth);
            if (ImGui::BeginPopupContextWindow("ExplorerContextMenu",
                                               ImGuiPopupFlags_MouseButtonRight))
            {
                if (ImGui::MenuItem("Refresh"))
                {
                    refresh();
                }
                ImGui::EndPopup();
            }

            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

            int i = 1;
            for (auto&& entry : std::filesystem::directory_iterator(_openedPath))
            {
                const auto& path = entry.path();
                const auto fileFormat = getNodeType(entry);

                if (!_filterBuf.isEmpty() && _filterBuf[0] != '\0' && !std::isspace(_filterBuf[0]))
                {
                    if (!StringAtom(path.filename().generic_string()).regexFind(_filterBuf))
                    {
                        continue;
                    }
                }

                drawFileThumbnail(_nodeTypesData[fileFormat].getTextureId(), entry, _thumbnailSize);
                if (maxCountPerWidth != 0 && i % maxCountPerWidth != 0)
                {
                    ImGui::SameLine();
                }
                ++i;
            }

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
    }

    void AssetsManagerWindowEWC::drawOneLevel(CacheNode& rootNode, bool& isSelected)
    {
        for (auto&& node : rootNode.children)
        {
            int flags = _commonTreeFlags;
            std::string filename = node.path.filename().generic_string();

            if (node.type != NodeType::Folder)
            {
                if (!_renderFilesInTreeView)
                {
                    continue;
                }
                const auto* icon = ICON_FA_FILE;
                if (node.type == NodeType::Code)
                {
                    icon = ICON_FA_FILE_CODE_O;
                }
                else if (node.type == NodeType::Image)
                {
                    icon = ICON_FA_FILE_IMAGE_O;
                }
                filename = icon + (" " + filename);
                flags |= ImGuiTreeNodeFlags_Leaf;
            }
            else
            {
                filename = ICON_FA_FOLDER + (" " + filename);

                auto atLeastOneFolder = std::ranges::any_of(node.children,
                                                            [](const CacheNode& n)
                                                            {
                                                                return n.type == NodeType::Folder;
                                                            });

                if (!atLeastOneFolder)
                {
                    flags |= ImGuiTreeNodeFlags_Leaf;
                }
            }

            // bool isSub = IsSubpath(_openedPath, node.path);
            // ImGui::SetNextItemOpen(isSub);
            const bool isOpened = ImGui::TreeNodeEx(filename.c_str(), flags);
            if (node.type == NodeType::Folder && ImGui::IsItemHovered()
                && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                _openedPath = node.path;
                isSelected = true;
            }
            if (isOpened)
            {
                if (node.type == NodeType::Folder)
                {
                    drawOneLevel(node, isSelected);
                }

                ImGui::TreePop();
            }
        }
    }

    AssetsManagerWindowEWC::NodeType AssetsManagerWindowEWC::getNodeType(
        const std::filesystem::directory_entry& entry)
    {
        if (entry.is_directory())
        {
            return NodeType::Folder;
        }

        if (!entry.is_regular_file())
        {
            return NodeType::Default;
        }

        auto ext = entry.path().extension().generic_string();
        // clang-format off
        if (ext == ".cpp"   ||
            ext == ".cc"    ||
            ext == ".cxx"   ||
            ext == ".C"     ||
            ext == ".c"     ||
            ext == ".hpp"   ||
            ext == ".hh"    ||
            ext == ".hxx"   ||
            ext == ".H"     ||
            ext == ".h"     ||
            ext == ".inl"   ||
            ext == ".ipp"   ||
            ext == ".pch"   ||
            ext == ".gch"   ||
            ext == ".lib"   ||
            ext == ".so"    ||
            ext == ".dll"   ||
            ext == ".cs"    ||
            ext == ".py"    ||
            ext == ".pyc"   ||
            ext == ".pyo"   ||
            ext == ".whl"   ||
            ext == ".pyi"   ||
            ext == ".vert"  ||
            ext == ".vs"    ||
            ext == ".frag"  ||
            ext == ".fs"    ||
            ext == ".geom"  ||
            ext == ".gs"    ||
            ext == ".tesc"  ||
            ext == ".tese"  ||
            ext == ".comp"  ||
            ext == ".glsl"  ||
            ext == ".hlsl"  ||
            ext == ".fx"    ||
            ext == ".metal" ||
            ext == ".spv"   ||
            ext == ".cmake" ||
            ext == ".pro"   ||
            ext == ".pri"   ||
            ext == ".json"  ||
            ext == ".xml"   ||
            ext == ".yml"   ||
            ext == ".yaml"  ||
            ext == ".ini"   ||
            ext == ".toml") return NodeType::Code;

        if (ext == ".jpg"   ||
            ext == ".jpeg"  ||
            ext == ".png"   ||
            ext == ".bmp"   ||
            ext == ".tga"   ||
            ext == ".psd"   ||
            ext == ".gif"   ||
            ext == ".hdr"   ||
            ext == ".pic") return NodeType::Image;
        // clang-format on

        return NodeType::Default;
    }

    bool AssetsManagerWindowEWC::drawFileThumbnail(ImTextureID texture,
                                                   const std::filesystem::directory_entry& entry,
                                                   glm::vec2 size)
    {
        auto path = entry.path();
        auto filename = path.filename().generic_string();
        const auto& originalFileName = filename;

        bool clicked = false;

        ImGui::PushStyleColor(ImGuiCol_Button, glm::vec4(0, 0, 0, 0));
        ImGui::BeginGroup();

        if (ImGui::ImageButton(filename.data(), texture, size))
        {
            clicked = true;
        }
        ImGui::PopStyleColor();

        // Adding button's padding
        size.x += ImGui::GetStyle().ItemSpacing.x * 2.f - 1.f;

        std::string value = filename;
        if (InputText(filename.data(), value, size.x, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (value != originalFileName)
            {
                std::error_code ec;

                auto newPath = path.parent_path() / value;
                std::filesystem::rename(path, newPath, ec);
                if (ec)
                {
                    errorLog("Can't rename file from: '{}' to '{}'. Reason: {}"_f
                             << filename << value << ec.message());
                }
                else
                {
                    path = newPath;
                }
            }
        }

        ImGui::EndGroup();

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
        {
            ImGui::BeginTooltip();

            std::error_code ec;
            const auto lastWrite = std::filesystem::last_write_time(path, ec);
            if (ec)
            {
                errorLog("Some error of std::filesystem::last_write_time: {}"_f << ec.message());
            }

            ImGui::Text("Name:      %s", originalFileName.data());
            ImGui::Text("Modified:  %s", std::format("{}", lastWrite).data());
            ImGui::Text("Location:  %s", path.generic_string().data());

            if (entry.is_regular_file())
            {
                const auto fileSize = static_cast<uint32_t>(std::filesystem::file_size(path));
                ImGui::Text("File size: %d", fileSize);
            }
            ImGui::EndTooltip();

            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (entry.is_directory())
                {
                    _openedPath = path;
                }
                else if (entry.is_regular_file() && getNodeType(entry) == NodeType::Code)
                {
                    gGameInstance->gameEditor.showWindow<TextEditorEWC>(
                        ".*", path.generic_string().data());
                }
            }
        }

        return clicked;
    }

    void AssetsManagerWindowEWC::rescanPhysicalDrive(CacheNode& node)
    {
        for (auto&& entry : std::filesystem::directory_iterator(node.path))
        {
            if (!entry.is_regular_file() && !entry.is_directory())
            {
                continue;
            }

            const auto nodeType = getNodeType(entry);

            CacheNode tmp;
            tmp.path = entry.path();
            tmp.type = nodeType;
            node.children.push_back(std::move(tmp));

            if (nodeType == NodeType::Folder)
            {
                rescanPhysicalDrive(node.children.back());
            }
        }
    }

    void AssetsManagerWindowEWC::refresh()
    {
        _rootCacheNode = {};

        _rootCacheNode.path = assetsPath;
        rescanPhysicalDrive(_rootCacheNode);
    }

    void AssetsManagerWindowEWC::drawExplorerToolbar()
    {
        float const rowHeight = ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("ExplorerTopBar", glm::vec2(0, rowHeight)))
        {
            ImGui::Dummy(glm::vec2(0, 0));

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, glm::vec2(0, 0));

            // =============== Input ====================
            ImGui::Dummy(glm::vec2(_defaultGap, 0));
            ImGui::SameLine();
            float const width = ImGui::GetContentRegionAvail().x - _defaultGap;
            ImGui::SetNextItemWidth(width);
            ImGui::InputTextWithHint("##ExplorerFilter", "Regex filter...", _filterBuf.data(),
                                     _filterBuf.size() + 1);
            ImGui::SameLine(0, _defaultGap * 3.f);

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
    }

} // namespace Core
