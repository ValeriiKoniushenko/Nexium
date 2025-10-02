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

#include "Editor/Configs.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Mesh3DAsset.h"

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
                    else if (ext == NXMesh3D::AssetT::fileExtension)
                    {
                        _mesh3ds.emplace(id, new Mesh3DAsset(id))
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

    NXMesh3D AssetsManager::getMesh3D(const StringAtom& logicPath)
    {
        return getAssetOf<NXMesh3D>(logicPath, _mesh3ds);
    }

    void AssetsManager::unloadAllResources()
    {
        _textures.clear();
        _skyboxes.clear();
        _mesh3ds.clear();
    }

    void AssetsManager::registerNewAssetPath(std::filesystem::path path)
    {
        infoLog("Registered new asset path: " + path.generic_string());
        _registeredPaths.emplace(std::move(path));
    }

    void AssetsManager::spawnMesh3DOnScene(const StringAtom& logicPath)
    {
        if (auto&& mesh = getMesh3D(logicPath))
        {
            gGameInstance->gameScene.addActor(mesh);
        }
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
            DEBUG_ASSERT(false);
            traceLog("Path is empty");
            return false;
        }

        DEBUG_ASSERT(logicPath.isStatic(), "You must use only _atom strings!");

        auto* found = logicPath.reverseFind(".nx");
        if (!found)
        {
            DEBUG_ASSERT(false);
            traceLog("Invalid file extension was passed");
            return false;
        }

        if (strcmp(found, requiredExt) != 0)
        {
            DEBUG_ASSERT(false);
            traceLog("Path is invalid. Expected extension is {} but passed {}"_f << requiredExt
                                                                                 << found);
            return false;
        }

        return true;
    }
} // namespace Core