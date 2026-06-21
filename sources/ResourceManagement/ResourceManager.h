/*
 * MIT License
 *
 * Copyright (c) 2018-2026 Valerii Koniushenko
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

#include "Core/Singleton.h"
#include "Graphics/Texture.h"
#include "Misc/BaseLog.h"

#include <Core/IntrusivePtr.h>
#include <filesystem>

namespace Core::Test
{
    class IBlueprint
    {
    public:
        IBlueprint(const IBlueprint&) = default;

        IBlueprint& operator=(const IBlueprint&) = default;

        IBlueprint(IBlueprint&&) = default;

        IBlueprint& operator=(IBlueprint&&) = default;

        virtual ~IBlueprint() = default;

    protected:
        IBlueprint() = default;
    };

    //
    //
    //
    class BlueprintManager : public Singleton<BlueprintManager>, public BaseLog
    {
        SINGLETONS_FRIEND(BlueprintManager);

    public:
        ~BlueprintManager() override = default;

        [[nodiscard]] spdlog::logger* getLogger() const override;

        [[nodiscard]] const char* getPrefix() const override;

    protected:
    };

    [[nodiscard]] inline BlueprintManager& GetBlueprintManager()
    {
        return BlueprintManager::Instance();
    }

    /// It helps to work with assets in an easier way. It tracks how many things refer to some
    /// asset, and after some type of logic it will destroy unused assets. Also, it seems to be some
    /// type of GarbageCollector
    ///
    /// The final Asset is a type of atomic information that can't be divided or somehow
    /// extended with extra logic. E.g.: images (.png, .jpg), audio (.wav, .mp3), 3D models, etc.
    class AssetsManager : public Singleton<AssetsManager>, public BaseLog
    {
        SINGLETONS_FRIEND(AssetsManager);

    public:
        ~AssetsManager() override = default;

        [[nodiscard]] spdlog::logger* getLogger() const override;

        [[nodiscard]] const char* getPrefix() const override;

        [[nodiscard]] Texture::Ptr getTexture(const std::string& path,
                                              bool isFlipVertically = true);

    protected:
        [[nodiscard]] bool isValidPath(const std::filesystem::path& path) const;

        /// Converts an absolute asset path to a path relative to the project root.
        ///
        /// Guarantees:
        ///   - No filesystem access (lexical operations only)
        ///   - No exceptions thrown
        ///   - Cross-platform (Linux, Windows, macOS)
        ///
        /// Limitations (by design - FS access is disallowed):
        ///   - Symlinks (Linux/macOS) are NOT resolved
        ///   - NTFS junctions/reparse points (Windows) are NOT resolved
        ///   - Windows .lnk shell links are stripped by extension only (target NOT followed)
        ///   If real symlink resolution is needed, canonicalize the path before calling this.
        ///
        /// Returns an empty path on an unrecoverable error.
        [[nodiscard]] std::filesystem::path toProjectRelativePath(
            const std::filesystem::path& path);

        /// Strips known link-like suffixes that can be detected lexically.
        /// This is purely a filename-level heuristic - no FS access.
        ///
        /// NOTE: Symlinks (Linux) and NTFS junctions (Windows) are fully opaque
        /// without filesystem access; they cannot be resolved here. If the caller
        /// needs real symlink resolution, they must do so before passing the path.
        [[nodiscard]] std::filesystem::path stripLexicalLinkSuffixes(
            const std::filesystem::path& p);

        /// Lexically normalizes a path: resolves "." and ".." purely by token
        /// manipulation - no filesystem access, no exceptions.
        [[nodiscard]] std::filesystem::path lexicallyNormalize(const std::filesystem::path& p);

    protected:
        std::unordered_map<std::string, Texture::Ptr> _textures; // std::string -> path
    };

    [[nodiscard]] inline AssetsManager& GetAssetsManager()
    {
        return AssetsManager::Instance();
    }
} // namespace Core::Test