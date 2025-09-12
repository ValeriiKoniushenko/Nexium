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

#include "Editor/GuiComponents/Spacer.h"
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

    std::string PrettyBytes(uint64_t bytes)
    {
        static const std::array<const char*, 6> suffixes = { "B", "KB", "MB", "GB", "TB", "PB" };

        if (bytes == 0)
        {
            return "0 B";
        }

        int i = 0;
        double count = static_cast<double>(bytes);

        while (count >= 1024.0 && i < static_cast<int>(suffixes.size()) - 1)
        {
            count /= 1024.0;
            ++i;
        }

        std::ostringstream out;
        out << std::fixed << std::setprecision(count < 10 ? 2 : (count < 100 ? 1 : 0)) << count
            << ' ' << suffixes[i];

        return out.str();
    }

} // namespace

namespace Core
{
    namespace Gui
    {
        class Spacer;
    }
    ECS_COMPONENT_IMPL(AssetsManagerWindowEWC)

    void AssetsManagerWindowEWC::tryOpenParentDir()
    {
        if (_openedPath.has_parent_path())
        {
            openPath(_openedPath.parent_path());
        }
    }

    void AssetsManagerWindowEWC::tryOpenPath(const std::filesystem::path& p)
    {
        if (std::filesystem::exists(p))
        {
            openPath(p);
        }
    }

    void AssetsManagerWindowEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        static bool _ = []()
        {
            std::unordered_map<NodeType, std::filesystem::path> paths
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

            return true;
        }();

        const auto gap = ImGui::GetStyle().WindowPadding.x;

        // Structure
        _toolbarLayout.setPaddings(glm::vec4(gap));

        _backButton = _toolbarLayout.addChildComponent<Gui::Button>(ICON_FA_CHEVRON_LEFT);
        _toolbarLayout.addChildComponent<Gui::Spacer>();
        _refreshButton = _toolbarLayout.addChildComponent<Gui::Button>(ICON_FA_REFRESH);
        _pathInput = _toolbarLayout.addChildComponent<Gui::TextInput>();
        _toolbarLayout.addChildComponent<Gui::Spacer>();
        _filterInput = _toolbarLayout.addChildComponent<Gui::TextInput>();

        // Style
        _backButton->setWidth(_backButton->getHeight());
        _refreshButton->setWidth(_refreshButton->getHeight());
        _pathInput->setFlex(Gui::Widget::Flex::FlexWidth);
        _filterInput->setWidth(150.f);
        _filterInput->setPlaceholder("Filter...");

        // Events
        _backButton->onClick.subscribe(
            [this](auto)
            {
                tryOpenParentDir();
            });
        _refreshButton->onClick.subscribe(
            [this](auto)
            {
                refresh();
            });
        _pathInput->onInput.subscribe(
            [this](const char* path)
            {
                if (path)
                {
                    tryOpenPath(std::filesystem::path(path));
                }
            });

        openPath(assetsPath);

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

    void AssetsManagerWindowEWC::openPath(const std::filesystem::path& path)
    {
        _openedPath = path;
        _pathInput->setInputtedData(_openedPath.generic_string());
    }

    void AssetsManagerWindowEWC::copyFrom(const std::filesystem::path& path)
    {
        _isCopy = true;
        _selectedPath = path;
    }

    void AssetsManagerWindowEWC::cutFrom(const std::filesystem::path& path)
    {
        _isCopy = false;
        _selectedPath = path;
    }

    void AssetsManagerWindowEWC::pasteTo(const std::filesystem::path& path)
    {
        std::error_code er;

        std::filesystem::copy(_selectedPath, path, std::filesystem::copy_options::recursive, er);

        if (er)
        {
            errorLog("Error while copying of file: " + er.message());
        }
        else if (!_isCopy)
        {
            deleteAt(_selectedPath);
        }

        _selectedPath.clear();
        refresh();
    }

    void AssetsManagerWindowEWC::deleteAt(const std::filesystem::path& path)
    {
        std::error_code er;
        std::filesystem::remove_all(path, er);
        if (er)
        {
            errorLog("Error while deleting of file: " + er.message());
        }
    }

    bool AssetsManagerWindowEWC::isFiltered(const std::filesystem::path& p) const
    {
        if (!_filterInput)
        {
            return false;
        }

        auto&& filter = _filterInput->getInputtedData();

        if (filter.empty() || filter[0] == '\0')
        {
            return false;
        }

        return p.generic_string().find(filter) == std::string::npos;
    }

    void AssetsManagerWindowEWC::drawExplorerTree()
    {
        if (ImGui::BeginChild("Explorer tree", glm::vec2(200.0f, 0), ImGuiChildFlags_ResizeX))
        {
            ImGui::Dummy({});
            bool isSelected = false;
            drawOneLevel(_rootCacheNode, isSelected);
            ImGui::Dummy({});
        }
        ImGui::EndChild();
    }

    void AssetsManagerWindowEWC::drawExplorer()
    {
        const auto defaultSpace = ImGui::GetStyle().ItemSpacing;
        const auto padding = ImGui::GetStyle().WindowPadding.x;

        if (ImGui::BeginChild("Explorer"))
        {
            _toolbarLayout.tick(GetWorld().timeDelta);
            const auto availX = ImGui::GetContentRegionAvail().x - padding * 2.f;

            const int maxCountPerWidth
                = static_cast<int>(availX / (_thumbnailSize.x + defaultSpace.x)) - 1;

            if (ImGui::BeginPopupContextWindow("ExplorerContextMenu",
                                               ImGuiPopupFlags_MouseButtonRight
                                                   | ImGuiPopupFlags_NoOpenOverItems))
            {
                if (ImGui::MenuItem("Back"))
                {
                    tryOpenParentDir();
                }
                if (!_selectedPath.empty() && ImGui::MenuItem("Paste"))
                {
                    pasteTo(_openedPath);
                }
                if (ImGui::MenuItem("Refresh"))
                {
                    refresh();
                }
                if (ImGui::MenuItem("To root"))
                {
                    openPath(assetsPath);
                }

                ImGui::EndPopup();
            }

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

            int i = 1;
            for (auto&& entry : std::filesystem::directory_iterator(_openedPath))
            {
                const auto& path = entry.path();
                const auto fileFormat = getNodeType(entry);

                if (isFiltered(path))
                {
                    continue;
                }

                drawFileThumbnail(_nodeTypesData[fileFormat].getTextureId(), entry, _thumbnailSize);
                if (maxCountPerWidth != 0 && i % maxCountPerWidth != 0)
                {
                    ImGui::SameLine();
                }
                ++i;
            }

            ImGui::PopStyleVar();

            ImGui::Dummy({});
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

            const bool isOpened = ImGui::TreeNodeEx(filename.c_str(), flags);

            // Check for LMB click to check content
            if (node.type == NodeType::Folder && ImGui::IsItemHovered()
                && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                openPath(node.path);
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

    void AssetsManagerWindowEWC::drawFileThumbnail(ImTextureID texture,
                                                   const std::filesystem::directory_entry& entry,
                                                   glm::vec2 size)
    {
        auto path = entry.path();
        auto filename = path.filename().generic_string();
        const auto& originalFileName = filename;
        bool needOpen = false;
        const bool isSelected = path == _selectedPath;

        if (isSelected)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.6f);
        }

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x);

        ImGui::BeginGroup();

        size -= ImGui::GetStyle().FramePadding * 2.f;
        ImGui::PushStyleColor(ImGuiCol_Button, glm::vec4(0, 0, 0, 0));
        ImGui::ImageButton(filename.data(), texture, size);
        ImGui::PopStyleColor();
        size += ImGui::GetStyle().FramePadding * 2.f;

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            needOpen = true;
        }

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup(filename.c_str());
        }

        if (ImGui::BeginPopup(filename.c_str()))
        {
            if (ImGui::MenuItem("Copy"))
            {
                copyFrom(path);
            }
            if (ImGui::MenuItem("Cut"))
            {
                cutFrom(path);
            }
            if (!_selectedPath.empty() && ImGui::MenuItem("Paste"))
            {
            }
            ImGui::EndPopup();
        }

        std::string value = filename;
        if (InputText(filename.data(), value, size.x, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (value != originalFileName)
            {
                std::error_code ec;

                const auto newPath = path.parent_path() / value;
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

        if (ImGui::IsItemHovered())
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
                ImGui::Text("File size: %s", PrettyBytes(fileSize).c_str());
            }
            ImGui::EndTooltip();
        }

        if (needOpen)
        {
            if (entry.is_directory())
            {
                openPath(path);
            }
            else if (entry.is_regular_file() && getNodeType(entry) == NodeType::Code)
            {
                gGameInstance->gameEditor.showWindow<TextEditorEWC>(".*",
                                                                    path.generic_string().data());
            }
        }

        if (isSelected)
        {
            ImGui::PopStyleVar();
        }

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x);
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

} // namespace Core
