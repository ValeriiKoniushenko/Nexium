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

#include "AssetsManager.h"

#include "GameplaySystem/Framework/GameInstance.h"
#include "Windows/ImageViewer.h"
#include "Windows/TextEditor.h"

namespace Core
{
    void EditorAssetsManager::tryToOpenFile(const std::filesystem::directory_entry& entry)
    {
        if (!entry.is_regular_file())
        {
            return;
        }

        auto type = getNodeType(entry);

        if (type == NodeType::Code)
        {
            gGameInstance->gameEditor.showWindow<TextEditorEWC>(
                ".*", entry.path().generic_string().data());
        }
        else if (type == NodeType::Image)
        {
            gGameInstance->gameEditor.showWindow<ImageViewerEWC>(
                ".*", entry.path().generic_string().data());
        }
    }

    EditorAssetsManager::NodeType EditorAssetsManager::getNodeType(
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

        if (ext.rfind(".nx") != std::string::npos)
        {
            return NodeType::NxFile;
        }

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

    void EditorAssetsManager::openPathFromOSExplorer(const std::filesystem::path& path)
    {
#ifdef _WIN32
        const std::string command = "explorer \"" + path.generic_string() + "\"";
#elif __APPLE__
        const std::string command = "open \"" + path.generic_string() + "\"";
#else
        const std::string command = "xdg-open \"" + path.generic_string() + "\"";
#endif
        std::system(command.c_str());
    }
} // namespace Core