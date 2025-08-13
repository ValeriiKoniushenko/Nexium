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

#include "Misc/IconsFontAwesome.h"

#include <format>

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(AssetsManagerWindowEWC)

    void AssetsManagerWindowEWC::onInit()
    {
        BaseFloatEWC::onInit();

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
                _nodeTypesData.emplace(type, std::move(tmp));
            }
        }
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
        if (ImGui::BeginChild("Explorer tree", ImVec2(200.0f, 0), ImGuiChildFlags_ResizeX))
        {
            ImGui::Dummy({}); // extra padding
            drawOneLevel(assetsPath, false);
            ImGui::Dummy({}); // extra padding
        }
        ImGui::EndChild();
    }

    void AssetsManagerWindowEWC::drawExplorer()
    {
        if (ImGui::BeginChild("Explorer"))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

            for (auto&& entry : std::filesystem::directory_iterator(_openedPath))
            {
                auto path = entry.path();
                const auto fileFormat = getNodeType(entry);

                drawFileThumbnail(_nodeTypesData[fileFormat].getTextureId(), entry, _thumbnailSize);
                ImGui::SameLine();
            }

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
    }

    void AssetsManagerWindowEWC::drawOneLevel(const std::filesystem::path& rootPath,
                                              bool isSelected)
    {
        for (auto&& entry : std::filesystem::directory_iterator(rootPath))
        {
            if (!entry.is_regular_file() && !entry.is_directory())
            {
                continue;
            }

            auto path = entry.path();
            auto nodePath = std::filesystem::relative(path, rootPath).generic_string();
            int flags = _commonTreeFlags;

            if (entry.is_regular_file())
            {
                auto icon = ICON_FA_FILE;
                const auto fileFormat = getNodeType(entry);
                if (fileFormat == NodeType::Code)
                {
                    icon = ICON_FA_FILE_CODE_O;
                }
                else if (fileFormat == NodeType::Image)
                {
                    icon = ICON_FA_FILE_IMAGE_O;
                }
                nodePath = icon + (" " + nodePath);
                flags |= ImGuiTreeNodeFlags_Leaf;
            }
            else if (entry.is_directory())
            {
                nodePath = ICON_FA_FOLDER + (" " + nodePath);
            }

            const bool isOpened = ImGui::TreeNodeEx(nodePath.c_str(), flags);
            if (isOpened)
            {
                if (entry.is_directory())
                {
                    drawOneLevel(path, isSelected);
                }

                ImGui::TreePop();
            }

            if (!isSelected && ImGui::IsItemClicked())
            {
                if (entry.is_directory())
                {
                    _openedPath = path;
                    isSelected = true;
                }
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
                                                   ImVec2 size)
    {
        auto path = entry.path();
        auto filename = path.filename().generic_string();
        const auto originalFileName = filename;

        bool clicked = false;

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
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
                errorLog("Some error {}"_f << ec.message());
            }

            ImGui::Text("Name:      %s", originalFileName.data());
            ImGui::Text("Modified:  %s", std::format("{}", lastWrite).data());
            ImGui::Text("Location:  %s", path.generic_string().data());

            if (entry.is_regular_file())
            {
                const uint32_t fileSize = std::filesystem::file_size(path);
                ImGui::Text("File size: %d", fileSize);
            }
            ImGui::EndTooltip();
        }

        return clicked;
    }

} // namespace Core
