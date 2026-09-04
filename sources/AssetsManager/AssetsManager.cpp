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

#include "AssetsManager.h"

#include "Editor/Windows/Editors/NxTextureEditor.h"
#include "Editor/Windows/Editors/TextEditor.h"
#include "Editor/Windows/ImageViewer.h"
#include "Editor/Windows/NxECSBasedEditor.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Misc/Configs.h"
#include "ModuleInfo.h"

#include <algorithm>
#include <array>
#include <memory>
#include <unordered_set>

#ifdef _WIN32
// clang-format off
    #include <Windows.h>
    #include <Commdlg.h>

// clang-format on
#endif

using namespace Core;

namespace fs = std::filesystem;

namespace
{
    [[nodiscard]] StringAtom ToLogicPath(const fs::path& path)
    {
        auto normalized = path.lexically_normal();
        if (normalized.is_absolute())
        {
            normalized
                = normalized.lexically_relative(Config::Path::projectAbsPath.lexically_normal());
        }

        if (normalized.empty())
        {
            return {};
        }

        return StringAtom::Intern(normalized.generic_string());
    }

    template<class T>
    T getAssetOf(Core::AssetsManager& manager, const StringAtom& logicPath,
                 std::unordered_map<StringAtom, AssetRef<BaseAsset>>& lookupContainer)
    {
        if (!manager.validatePath(logicPath, T::AssetT::fileExtension))
        {
            return T();
        }

        auto it = lookupContainer.find(logicPath);
        if (it == lookupContainer.end())
        {
            it = lookupContainer.find(ToLogicPath(fs::path(logicPath.data())));
        }

        if (it == lookupContainer.end())
        {
            manager.criticalLog("Asset is not found by the next path: {}"_f << logicPath);
            return {};
        }

        return T(static_cast<T::AssetT&>(*it->second));
    }
} // namespace

namespace Core
{
    AssetsManager::AssetsManager()
    {
        _textureAtlases.try_emplace("default"_atom);
        registerNewAssetPath(Config::Path::data);
    }

    void AssetsManager::initScanFileSystem()
    {
        _textures.clear();
        _skyboxes.clear();
        _ecsAssets.clear();
        scanFileSystem(false);
    }

    void AssetsManager::refreshFilesSystem()
    {
        scanFileSystem(true);
    }

    void AssetsManager::scanFileSystem(bool removeMissingAssets)
    {
        std::unordered_set<StringAtom> foundEcsAssets;
        std::unordered_set<StringAtom> foundTextures;
        std::unordered_set<StringAtom> foundSkyboxes;
        bool scanCompleted = true;

        for (const auto& registeredPath : _registeredPaths)
        {
            try
            {
                auto scanRoot = registeredPath;
                if (scanRoot.is_relative())
                {
                    scanRoot = Config::Path::projectAbsPath / scanRoot;
                }

                for (const auto& entry : fs::recursive_directory_iterator(scanRoot))
                {
                    if (!entry.is_regular_file())
                    {
                        continue;
                    }

                    const auto absPath = fs::absolute(entry.path()).lexically_normal();
                    const auto ext = absPath.extension().generic_string();
                    const auto id = ToLogicPath(absPath);
                    if (id.isEmpty())
                    {
                        continue;
                    }

                    if (ext == ECSAsset::fileExtension)
                    {
                        foundEcsAssets.emplace(id);
                        auto it = _ecsAssets.find(id);
                        if (it == _ecsAssets.end())
                        {
                            it = _ecsAssets.emplace(id, new ECSAsset(id)).first;
                        }
                        it->second->connectSourceFile(absPath);
                    }
                    else if (ext == TextureAsset::fileExtension)
                    {
                        foundTextures.emplace(id);
                        auto it = _textures.find(id);
                        if (it == _textures.end())
                        {
                            it = _textures.emplace(id, new TextureAsset(id)).first;
                        }
                        it->second->attachAndReadFromFile(absPath);
                    }
                    else if (ext == SkyboxAsset::fileExtension)
                    {
                        foundSkyboxes.emplace(id);
                        auto it = _skyboxes.find(id);
                        if (it == _skyboxes.end())
                        {
                            it = _skyboxes.emplace(id, new SkyboxAsset(id)).first;
                        }
                        it->second->attachAndReadFromFile(absPath);
                    }
                }
            }
            catch (const fs::filesystem_error& e)
            {
                scanCompleted = false;
                criticalLog("Got a error while scanning a folder '{}' for assets. Details: {}"_f
                            << registeredPath.generic_string() << e.what());
            }
        }

        if (removeMissingAssets && scanCompleted)
        {
            std::erase_if(_ecsAssets, [&foundEcsAssets](const auto& item)
                          { return !foundEcsAssets.contains(item.first); });
            std::erase_if(_textures, [&foundTextures](const auto& item)
                          { return !foundTextures.contains(item.first); });
            std::erase_if(_skyboxes, [&foundSkyboxes](const auto& item)
                          { return !foundSkyboxes.contains(item.first); });
        }
    }

    void AssetsManager::generateTextureAtlas(const fs::path& atlasFolder)
    {
        generateTextureAtlas("default"_atom, atlasFolder);
    }

    void AssetsManager::generateTextureAtlas(const StringAtom& atlasName,
                                             const fs::path& atlasFolder)
    {
        _textureAtlases.try_emplace(atlasName).first->second.generateTextureAtlas(atlasFolder);
    }

    TextureAtlas& AssetsManager::getAtlas(const StringAtom& atlasName)
    {
        auto it = _textureAtlases.find(atlasName);
        if (it == _textureAtlases.end()) [[unlikely]]
        {
            throw std::runtime_error("AssetsManager: No texture atlas with name '"
                                     + atlasName.toStdString() + "' found");
        }

        return it->second;
    }

    const TextureAtlas& AssetsManager::getAtlas(const StringAtom& atlasName) const
    {
        auto it = _textureAtlases.find(atlasName);
        if (it == _textureAtlases.end()) [[unlikely]]
        {
            throw std::runtime_error("AssetsManager: No texture atlas with name '"
                                     + atlasName.toStdString() + "' found");
        }

        return it->second;
    }

    std::vector<StringAtom> AssetsManager::getAtlasesAsVector() const
    {
        std::vector<StringAtom> atlases;
        atlases.reserve(_textureAtlases.size());
        std::ranges::transform(_textureAtlases, std::back_inserter(atlases),
                               [](const auto& pair) { return pair.first; });
        std::ranges::sort(atlases, [](const auto& lhs, const auto& rhs)
                          { return lhs.toStdString() < rhs.toStdString(); });

        return atlases;
    }

    TextureAtlas& AssetsManager::getTextureAtlas()
    {
        return getAtlas("default"_atom);
    }

    const TextureAtlas& AssetsManager::getTextureAtlas() const
    {
        return getAtlas("default"_atom);
    }

    NXTexture AssetsManager::getTexture(const StringAtom& logicPath)
    {
        return getAssetOf<NXTexture>(*this, logicPath, _textures);
    }

    NXSkybox AssetsManager::getSkybox(const StringAtom& logicPath)
    {
        return getAssetOf<NXSkybox>(*this, logicPath, _skyboxes);
    }

    spdlog::logger* AssetsManager::getLogger() const
    {
        return ::AssetsManager::getLogger();
    }

    void AssetsManager::unloadAllResources()
    {
        _textures.clear();
        _skyboxes.clear();
        _ecsAssets.clear();
    }

    void AssetsManager::registerNewAssetPath(fs::path path)
    {
        infoLog("Registered new asset path: " + path.generic_string());
        _registeredPaths.emplace(std::move(path));
    }

    NXECSAsset AssetsManager::getEcsAsset(const StringAtom& logicPath)
    {
        const auto it = _ecsAssets.find(logicPath);
        return it == _ecsAssets.end() ? NXECSAsset{} : it->second;
    }

    BaseComponent::Ptr AssetsManager::getUniqueEcsAsset(const StringAtom& logicPath)
    {
        auto&& it = _ecsAssets.find(logicPath);
        if (it == _ecsAssets.end() || !Verify(it->second.isValid()))
        {
            return nullptr;
        }

        return it->second->uniqueLoad();
    }

    WeakNXECSAsset AssetsManager::getWeakEcsAsset(const StringAtom& logicPath)
    {
        const auto it = _ecsAssets.find(logicPath);
        return it == _ecsAssets.end() ? WeakNXECSAsset{} : it->second;
    }

    NXECSAsset AssetsManager::getEcsAssetAt(std::size_t index, Tag tagMask)
    {
        std::size_t i = 0;
        for (auto& asset : _ecsAssets | std::views::values)
        {
            if (asset->getTags() & tagMask)
            {
                if (i == index)
                {
                    return asset;
                }
                ++i;
            }
        }

        errorLog("Can't get asset at filtered index {} because it is out of range"_f << index);
        return {};
    }

    WeakNXECSAsset AssetsManager::getWeakEcsAssetAt(std::size_t index, Tag tagMask)
    {
        std::size_t i = 0;
        for (auto& asset : _ecsAssets | std::views::values)
        {
            if (asset->getTags() & tagMask)
            {
                if (i == index)
                {
                    return asset;
                }
                ++i;
            }
        }

        errorLog("Can't get asset at filtered index {} because it is out of range"_f << index);
        return {};
    }

    std::optional<ECSAsset::Meta> AssetsManager::getECSAssetMeta(std::size_t index, Tag tagMask)
    {
        int* p = NULL;
        p = new int(10
        );

        std::size_t i = 0;
        for (auto& asset : _ecsAssets | std::views::values)
        {
            if (asset->getTags() & tagMask)
            {
                if (i == index)
                {
                    return asset->getMeta();
                }
                ++i;
            }
        }

        errorLog("Can't get asset at filtered index {} because it is out of range"_f << index);
        return std::nullopt;
    }

    NXECSAsset AssetsManager::getEcsAssetByPath(const fs::path& path)
    {
        const auto it = findAssetByPath(path);
        if (it == _ecsAssets.end()) [[unlikely]]
        {
            return {};
        }
        return it->second;
    }

    WeakNXECSAsset AssetsManager::getWeakEcsAssetByPath(const fs::path& path)
    {
        const auto it = findAssetByPath(path);
        if (it == _ecsAssets.end()) [[unlikely]]
        {
            return {};
        }
        return it->second;
    }

    std::size_t AssetsManager::getEcsAssetCountByTag(Tag tagMask) const
    {
        std::size_t count = 0;

        if (tagMask == Tag_None)
        {
            return count;
        }

        for (auto&& [_, asset] : _ecsAssets)
        {
            if (asset->getTags() & tagMask)
            {
                ++count;
            }
        }
        return count;
    }

    const std::set<fs::path>& AssetsManager::getRegisteredPaths() const noexcept
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

        const auto pipe = std::unique_ptr<FILE, std::function<void(FILE*)>>(popen(cmd.c_str(), "r"),
                                                                            [](FILE* f)
                                                                            {
                                                                                if (f)
                                                                                {
                                                                                    pclose(f);
                                                                                }
                                                                            });
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

        if (fs::is_directory(out.data()))
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

        Assert(logicPath.isStatic(), "You must use only _atom strings for asset paths!");

#if defined(DEBUG)
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
#endif

        return true;
    }

    void AssetsManager::TryToOpenFile(const fs::directory_entry& entry)
    {
        if (!entry.is_regular_file())
        {
            return;
        }

        auto type = GetNodeType(entry);

        if (type == NodeType::Code || type == NodeType::Default)
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
            TryToOpenNxFile(entry);
        }
    }

    void AssetsManager::TryToOpenNxFile(const fs::directory_entry& entry)
    {
        if (GetNodeType(entry) != NodeType::NxFile)
        {
            return;
        }

        const auto path = entry.path();
        const auto ext = path.extension().generic_string();
        if (ext == NXECSAsset::ValueT::fileExtension)
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

    AssetsManager::NodeType AssetsManager::GetNodeType(const fs::directory_entry& entry)
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

    void AssetsManager::OpenPathFromOSExplorer(const fs::path& path)
    {
#ifdef _WIN32
        std::string command = "explorer \"" + path.generic_string() + "\"";
        std::replace(command.begin(), command.end(), '/', '\\');
#elif __APPLE__
        const std::string command = "open \"" + path.generic_string() + "\"";
#else
        const std::string command = "xdg-open \"" + path.generic_string() + "\"";
#endif
        std::system(command.c_str());
    }

    std::unordered_map<StringAtom, NXECSAsset>::iterator AssetsManager::findAssetByPath(
        const fs::path& path)
    {
        if (path.extension().generic_string() != NXECSAsset::ValueT::fileExtension)
        {
            return _ecsAssets.end();
        }

        return _ecsAssets.find(ToLogicPath(path));
    }
} // namespace Core
