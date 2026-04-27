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

#include "AssetsManager/TextureAtlas.h"
#include "ECSAsset.h"
#include "SkyboxAsset.h"
#include "TextureAsset.h"
#include "Utils/Functions.h"

#include <set>
#include <unordered_map>

namespace Core
{
    /**
     * Assets manager class to get/load/find/free some asset.
     * In general this class we must use to manage some assets. It stores & works
     * not directly with a file at the filesystem, but with some 'view' to this file
     * in the engine's context. I.e.:
     * - Texture (.png, .jpeg, ...) -> .nxtex
     * - 3D Model (.fbx, .obj, ...) -> .nx3dmesh
     * - XXX thing (.XXX) -> .nXXX
     * @details What are .nX files:
     * .nX files are a type of data that doesn't contain the referred content itself
     * it contains some configurations, settings, and the path to the real asset(i.e.,
     * some texture: Stone.png). Roughly speaking, it can contain such a structure for a
     * texture:
     * @code{cpp}
     * struct NXRawTexture {
     *     std::filesystem::path path;
     *     Core::RGBA colorFilter;
     * };
     * @endcode
     * The final file content can be almost the same as the struct above. Also, want
     * to highlight that it's only a 'raw' texture - data representation from the
     * filesystem. But finally, inside AssetsManager it will be reworked to smth like
     * that (but it's very(!) rough example):
     * @code{cpp}
     * struct NXTexture : public NXRawTexture {
     *     NXTexture* get() { ++refCounter; return ...; }
     *     void free() { if (--refCounter) deleteFromGPU(); }
     * private:
     *     GLuint _idOnGPU = 0;
     *     uint64_t refCounter = 0;
     * };
     * @endcode
     * So now, you can get this texture from asset manager and don't worry when it
     * should be free or should we load it from the real filesystem or no. Just
     * use it!
     * In general, to create a necessary .nX file, you should open an editor, and using
     * Assets tab create it (at the moment of writing this doc - it's not impl.)
     * @details How AssetsManager works:
     * 1. At the start of the game/editor it will read all .nX files from the configured
     * path (where is it, you can check at Core::Config::Path::bakedAssets).
     * 2. It reads all found data to the RAM to provide the best speed.
     * 3. Use provided functionality to get the necessary asset. I.e.,
     * @code{cpp}
     * // Use only atomic string to speed up search of the necessary asset.
     * // CORRECT way:
     * auto texture = GetAssetsManager().getTexture("/path/to/my/texture.png"_atom);
     *
     * // Don't use some type of dynamic string or string literals directly.
     * // WRONG:
     * auto texture = GetAssetsManager().getTexture("/path/to/my/texture.png");
     * @endcode
     */
    class AssetsManager : public BaseLog
    {
    public:
        ENUM_CLASS();
        enum class NodeType
        {
            Default,
            Code,
            Image,
            Folder,
            NxFile
        };

    public:
        AssetsManager();
        AssetsManager(const AssetsManager&) = delete;
        AssetsManager(AssetsManager&&) = delete;
        AssetsManager& operator=(const AssetsManager&) = delete;
        AssetsManager& operator=(AssetsManager&&) = delete;
        ~AssetsManager() override = default;

        // ================= STATIC ======================
        [[nodiscard]] static StringAtom OpenFileSelectionDialog(
            const std::vector<std::string>& filter);

        [[nodiscard]] static NodeType GetNodeType(const std::filesystem::directory_entry& entry);
        static void TryToOpenFile(const std::filesystem::directory_entry& path);
        static void TryToOpenNxFile(const std::filesystem::directory_entry& path);
        static void OpenPathFromOSExplorer(const std::filesystem::path& path);

        // ================= MAIN ======================
        void rescanFileSystem();
        void unloadAllResources();
        void generateTextureAtlas(const std::filesystem::path& atlasFolder);

        [[nodiscard]] const std::set<std::filesystem::path>& getRegisteredPaths() const noexcept;
        void registerNewAssetPath(std::filesystem::path path);

        [[nodiscard]] bool validatePath(const StringAtom& logicPath, const char* requiredExt);

        // TODO: remove it, merge to main NXAsset
        [[nodiscard]] NXTexture getTexture(const StringAtom& logicPath);

        // TODO: remove it, merge to main NXAsset
        [[nodiscard]] NXSkybox getSkybox(const StringAtom& logicPath);

        // ============== WORKING WITH ASSETS ==========

        /**
         * @brief Get asset by its logical path. Will load the asset if it wasn't loaded previously.
         * @param logicPath Logical path to the asset
         * @return Strong reference to the asset
         */
        [[nodiscard]] NXAsset getAsset(const StringAtom& logicPath);

        /**
         * @brief Get asset by its logical path without loading it. Returns nullptr if asset wasn't
         * loaded previously.
         * @param logicPath Logical path to the asset
         * @return Weak reference to the asset if it's loaded, nullptr otherwise
         */
        [[nodiscard]] WeakNXAsset getWeakAsset(const StringAtom& logicPath);

        /**
         * @brief Get asset by its index in assets collection. Will load the asset if it wasn't
         * loaded previously.
         * @param index Index of the asset in collection
         * @param filter Filter to apply when searching assets
         * @return Strong reference to the asset
         */
        [[nodiscard]] NXAsset getAssetAt(std::size_t index, AssetAction filter = AA_None);

        /**
         * @brief Get asset by its index without loading it. Returns nullptr if asset wasn't loaded
         * previously.
         * @param index Index of the asset in collection
         * @param filter Filter to apply when searching assets
         * @return Weak reference to the asset if it's loaded, nullptr otherwise
         */
        [[nodiscard]] WeakNXAsset getWeakAssetAt(std::size_t index, AssetAction filter = AA_None);

        /**
         * @brief Get asset by filesystem path. Will load the asset if it wasn't loaded previously.
         * @param path Filesystem path to the asset
         * @return Strong reference to the asset
         */
        [[nodiscard]] NXAsset getAssetByPath(const std::filesystem::path& path);

        /**
         * @brief Get asset by filesystem path without loading it. Returns nullptr if asset wasn't
         * loaded previously.
         * @param path Filesystem path to the asset
         * @return Weak reference to the asset if it's loaded, nullptr otherwise
         */
        [[nodiscard]] WeakNXAsset getWeakAssetByPath(const std::filesystem::path& path);

        /**
         * @brief Get total count of assets matching the filter
         * @param filter Filter to apply when counting assets
         * @return Number of assets matching the filter
         */
        [[nodiscard]] std::size_t getAssetCount(AssetAction filter = AA_None) const;

        // ================ OVERRIDEs ==================

        // override BaseLog
        [[nodiscard]] spdlog::logger* getLogger() const override;

    protected:
        [[nodiscard]] std::unordered_map<StringAtom, NXAsset>::iterator findAssetByPath(
            const std::filesystem::path& path);

    protected:
        TextureAtlas _textureAtlas;
        std::set<std::filesystem::path> _registeredPaths;
        std::unordered_map<StringAtom, NXAsset> _assets;
        std::unordered_map<StringAtom, AssetRef<BaseAsset>> _textures;
        std::unordered_map<StringAtom, AssetRef<BaseAsset>> _skyboxes;
    };

} // namespace Core

#include "AssetsManager.generated.h" // added by the code generator. Better don't move it.
