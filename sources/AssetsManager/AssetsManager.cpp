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

#include "../Editor/Windows/ECSAdapters/EditorStaticMeshBundleAdapter.h"
#include "../Misc/Configs.h"
#include "Editor/Windows/ImageViewer.h"
#include "Editor/Windows/NxECSBasedEditor.h"
#include "Editor/Windows/NxTextureEditor.h"
#include "Editor/Windows/TextEditor.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "ModuleInfo.h"

#include <array>
#include <memory>

#ifdef _WIN32
// clang-format off
    #include <Windows.h>
    #include <Commdlg.h>
// clang-format on
#endif

using namespace Core;

namespace
{

    template<class T>
    T getAssetOf(const StringAtom& logicPath,
                 std::unordered_map<StringAtom, AssetRef<BaseAsset>>& lookupContainer)
    {
        if (!GetAssetsManager().validatePath(logicPath, T::AssetT::fileExtension))
        {
            return T();
        }

        if (!lookupContainer.contains(logicPath))
        {
            const std::filesystem::path path = logicPath.data();
            for (auto&& registered : GetAssetsManager().getRegisteredPaths())
            {
                try
                {
                    std::filesystem::path normalized;
                    if (registered.is_relative())
                    {
                        normalized = Config::Path::projectAbsPath;
                    }
                    normalized /= registered;

                    const auto realRegistered = std::filesystem::absolute(normalized).parent_path();
                    const auto relative = std::filesystem::relative(path, realRegistered);
                    if (lookupContainer.contains(StringAtom(relative.generic_string())))
                    {
                        return T(reinterpret_cast<T::AssetT&>(
                            *lookupContainer.at(StringAtom(relative.generic_string()))));
                    }
                }
                catch (const std::filesystem::filesystem_error& e)
                {
                    GetAssetsManager().errorLog(
                        "Can't resolve a path due to internal error, of was met junction symlink: {}"_f
                        << e.what());
                }
                catch (...)
                {
                    GetAssetsManager().errorLog(
                        "Can't resolve a path due to internal error, of was met junction symlink");
                }
            }

            GetAssetsManager().criticalLog("The asset is not found by the next path: {}"_f
                                           << logicPath);
            return T();
        }

        return T(reinterpret_cast<T::AssetT&>(*lookupContainer.at(logicPath)));
    }
} // namespace

namespace Core
{
    AssetsManager::AssetsManager()
    {
        registerNewAssetPath(Config::Path::assets);
    }

    void AssetsManager::rescanFileSystem()
    {
        _textures.clear();

        for (auto&& path : _registeredPaths)
        {
            try
            {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
                {
                    if (!entry.is_regular_file())
                    {
                        continue;
                    }

                    const auto absPath = std::filesystem::absolute(entry.path());
                    const auto ext = absPath.extension().generic_string();

                    // check for non baked
                    if (ext.size() < 3 || strncmp(ext.c_str(), ".nx", 3) != 0)
                    {
                        continue;
                    }

                    auto relPath = std::filesystem::relative(absPath, Config::Path::projectAbsPath);

                    auto id = StringAtom::Intern(relPath.generic_string());
                    if (ext == ".nx")
                    {
                        _assets.emplace(id, new ECSAsset(id))
                            .first->second->connectSourceFile(absPath);
                    }
                    if (ext == NXTexture::AssetT::fileExtension)
                    {
                        _textures.emplace(id, new TextureAsset(id))
                            .first->second->attachAndReadFromFile(absPath);
                    }
                    else if (ext == NXSkybox::AssetT::fileExtension)
                    {
                        _skyboxes.emplace(id, new SkyboxAsset(id))
                            .first->second->attachAndReadFromFile(absPath);
                    }
                }
            }
            catch (std::filesystem::filesystem_error& e)
            {
                criticalLog("Got a error while scanning a folder '{}' for assets. Details: {}"_f
                            << path.generic_string() << e.what());
            }
        }
    }

    NXTexture AssetsManager::getTexture(const StringAtom& logicPath)
    {
        return getAssetOf<NXTexture>(logicPath, _textures);
    }

    NXSkybox AssetsManager::getSkybox(const StringAtom& logicPath)
    {
        return getAssetOf<NXSkybox>(logicPath, _skyboxes);
    }

    spdlog::logger* AssetsManager::getLogger() const
    {
        return ::AssetsManager::getLogger();
    }

    void AssetsManager::unloadAllResources()
    {
        _textures.clear();
        _skyboxes.clear();
    }

    void AssetsManager::registerNewAssetPath(std::filesystem::path path)
    {
        infoLog("Registered new asset path: " + path.generic_string());
        _registeredPaths.emplace(std::move(path));
    }

    NXAsset AssetsManager::getAsset(const StringAtom& logicPath)
    {
        return _assets.at(logicPath);
    }

    NXAsset AssetsManager::getAssetByPath(const std::filesystem::path& path)
    {
        if (path.extension().generic_string() != NXAsset::ValueT::fileExtension)
        {
            return {};
        }

        for (auto&& registered : GetAssetsManager().getRegisteredPaths())
        {
            try
            {
                std::filesystem::path normalized;
                if (registered.is_relative())
                {
                    normalized = Config::Path::projectAbsPath;
                }
                normalized /= registered;

                const auto realRegistered = std::filesystem::absolute(normalized).parent_path();
                const auto relative = std::filesystem::relative(path, realRegistered);
                const auto id = StringAtom(relative.generic_string());
                if (_assets.contains(id))
                {
                    return _assets.at(id);
                }
            }
            catch (const std::filesystem::filesystem_error& e)
            {
                GetAssetsManager().errorLog(
                    "Can't resolve a path due to internal error, of was met junction symlink: {}"_f
                    << e.what());
            }
            catch (...)
            {
                GetAssetsManager().errorLog(
                    "Can't resolve a path due to internal error, of was met junction symlink");
            }
        }

        return {};
    }

    void AssetsManager::spawnMesh3DOnScene(const StringAtom& logicPath)
    {
        Assert(false, "Not implemented");
        // if (auto&& mesh = getMesh3D(logicPath))
        //{
        //     gGameInstance->gameScene.addActor(mesh);
        // }
    }

    const std::set<std::filesystem::path>& AssetsManager::getRegisteredPaths() const noexcept
    {
        return _registeredPaths;
    }

    StringAtom AssetsManager::OpenFileSelectionDialog(const std::vector<std::string>& filter)
    {
        constexpr std::size_t maxFilePath = 4096;
        std::array<char, maxFilePath> buffer{};

        bool isPressedOk = false;
#ifdef _WIN32
        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFile = buffer.data();
        ofn.nMaxFile = MAX_PATH;
        std::string filterStr;
        filterStr = "All Files\0*\0";
        if (filter.empty())
        {
        }
        else
        {
            /*for (auto&& filterPath : filter)
            {
                filterStr += filterPath;
            }*/
        }

        ofn.lpstrFilter = filterStr.c_str();
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        isPressedOk = GetOpenFileNameA(&ofn) == TRUE;
#else
        std::string cmd = "zenity --file-selection --file-filter=";
        if (filter.empty())
        {
            cmd += "*";
        }
        else
        {
            cmd += "\"";
            for (const auto& f : filter)
            {
                cmd += f + " ";
            }
            cmd.pop_back();
            cmd += "\"";
        }

        const std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe)
        {
            Core::globalLog.criticalLog("Can't open CMD for file selection dialog");
            return {};
        }

        fgets(buffer.data(), buffer.size(), pipe.get());
        isPressedOk = buffer[0] != '\0';
#endif

        if (!isPressedOk)
        {
            return {};
        }

        StringAtom out(buffer.data(), strlen(buffer.data()));
        out.trim('\n');

        if (std::filesystem::is_directory(out.data()))
        {
            return {};
        }

        return out;
    }

    bool AssetsManager::validatePath(const StringAtom& logicPath, const char* requiredExt)
    {
        if (logicPath.isEmpty() || !requiredExt)
        {
            Assert(false);
            traceLog("Path is empty");
            return false;
        }

        Assert(logicPath.isStatic(), "You must use only _atom strings!");

        auto* found = logicPath.reverseFind(".nx");
        if (!found)
        {
            Assert(false);
            traceLog("Invalid file extension was passed");
            return false;
        }

        if (strcmp(found, requiredExt) != 0)
        {
            Assert(false);
            traceLog("Path is invalid. Expected extension is {} but passed {}"_f << requiredExt
                                                                                 << found);
            return false;
        }

        return true;
    }

    void AssetsManager::tryToOpenFile(const std::filesystem::directory_entry& entry)
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
        else if (type == NodeType::NxFile)
        {
            tryToOpenNxFile(entry);
        }
    }

    void AssetsManager::tryToOpenNxFile(const std::filesystem::directory_entry& entry)
    {
        if (getNodeType(entry) != NodeType::NxFile)
        {
            return;
        }

        const auto path = entry.path();
        const auto ext = path.extension().generic_string();
        if (ext == NXAsset::ValueT::fileExtension)
        {
            gGameInstance->gameEditor.showWindow<NxECSBasedEditorEWC>(".*",
                                                                      path.generic_string().data());
        }
        else if (ext == NXTexture::AssetT::fileExtension)
        {
            gGameInstance->gameEditor.showWindow<NxTextureEditorEWC>(".*",
                                                                     path.generic_string().data());
        }
    }

    AssetsManager::NodeType AssetsManager::getNodeType(
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

    void AssetsManager::openPathFromOSExplorer(const std::filesystem::path& path)
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