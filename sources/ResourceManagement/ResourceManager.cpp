

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

#include "ResourceManager.h"

#include "Misc/Configs.h"
#include "ModuleInfo.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace fs = std::filesystem;

namespace Core::Test
{
    spdlog::logger* BlueprintManager::getLogger() const
    {
        return ResourceManagement::getLogger();
    }

    const char* BlueprintManager::getPrefix() const
    {
        return "BlueprintManager";
    }

    spdlog::logger* AssetsManager::getLogger() const
    {
        return ResourceManagement::getLogger();
    }

    const char* AssetsManager::getPrefix() const
    {
        return "AssetsManager";
    }

    Texture::Ptr AssetsManager::getTexture(const std::string& path,
                                           bool isFlipVertically /* = true*/)
    {
        std::filesystem::path p = path;
        if (!isValidPath(p))
        {
            warnLog("Was passed absolute path. Try to avoid it. Path: {}"_f << path);
            p = toProjectRelativePath(path);
        }

        if (auto it = _textures.find(p.generic_string()); it != _textures.end())
        {
            return it->second;
        }

        auto texture = Texture::Create();
        if (!texture->loadFromFile(Config::Path::projectAbsPath / p, isFlipVertically))
        {
            criticalLog("The texture wasn't loaded by the next path: {}"_f << path);
            return {};
        }

        _textures.emplace(std::move(p.generic_string()), texture);

        return texture;
    }

    bool AssetsManager::isValidPath(const std::filesystem::path& path) const
    {
        return path.is_relative();
    }

    [[nodiscard]] fs::path AssetsManager::lexicallyNormalize(const fs::path& p)
    {
        fs::path result;

        for (const auto& part : p)
        {
            if (part == "..")
            {
                // Pop last real component, but preserve leading ".."
                // in already-relative paths (can't pop past unknown root).
                if (!result.empty() && result.filename() != "..")
                {
                    result = result.parent_path();
                }
                else
                {
                    result /= part;
                }
            }
            else if (part == ".")
            {
                // skip
            }
            else
            {
                result /= part;
            }
        }

        return result;
    }

    [[nodiscard]] fs::path AssetsManager::stripLexicalLinkSuffixes(const fs::path& p)
    {
        fs::path result;

        for (const auto& part : p)
        {
            // Skip root-name, root-dir, and empty parts - pass through unchanged.
            const auto str = part.generic_string();
            if (str.empty() || str == "/" || str == "\\" || part == part.root_name())
            {
                result /= part;
                continue;
            }

#if defined(_WIN32)
            // Windows Shell Links (.lnk): purely a file with a special extension.
            // We can strip it lexically. The actual binary target inside the .lnk
            // requires COM (IShellLink) + FS access - out of scope here.
            constexpr std::string_view lnkExt = ".lnk";
            if (str.size() > lnkExt.size())
            {
                const auto suffix = std::string_view(str).substr(str.size() - lnkExt.size());
                if (suffix == lnkExt)
                {
                    const auto stripped = str.substr(0, str.size() - lnkExt.size());
                    warnLog(
                        "toProjectRelativePath: component '{}' appears to be a Windows shell "
                        "link (.lnk). Stripping extension lexically - real link target requires "
                        "FS access and is NOT resolved."_f
                        << str);
                    result /= fs::path(stripped);
                    continue;
                }
            }
#endif

            // Linux/Windows symlinks and NTFS junctions are invisible at this level.
            // We cannot detect or resolve them without stat()/readlink()/DeviceIoControl().
            result /= part;
        }

        return result;
    }

    [[nodiscard]] fs::path AssetsManager::toProjectRelativePath(const fs::path& path)
    {
        if (path.empty())
        {
            warnLog("toProjectRelativePath: received an empty path.");
            return {};
        }

        const fs::path& projectRoot = Config::Path::projectAbsPath;

        if (projectRoot.empty())
        {
            criticalLog(
                "toProjectRelativePath: Config::Path::projectAbsPath is empty. "
                "Cannot relativize '{}'."_f
                << path.generic_string());
            return {};
        }

        // Step 1: Lexical normalization - stdlib first, then our own ".." folding
        //         on top for any residual edge cases. No FS access.
        fs::path normAsset = lexicallyNormalize(path.lexically_normal());
        fs::path normProject = lexicallyNormalize(projectRoot.lexically_normal());

        // Step 2: Strip lexically detectable link suffixes (e.g. .lnk on Windows)
        normAsset = stripLexicalLinkSuffixes(normAsset);
        normProject = stripLexicalLinkSuffixes(normProject);

        // Step 3: Validate - both must be absolute for relativization to make sense
        if (!normAsset.is_absolute())
        {
            errorLog(
                "toProjectRelativePath: asset path '{}' is not absolute after normalization. "
                "Pass an absolute path."_f
                << normAsset.generic_string());
            return {};
        }

        if (!normProject.is_absolute())
        {
            criticalLog(
                "toProjectRelativePath: project root '{}' is not absolute. "
                "Config::Path::projectAbsPath must always be absolute."_f
                << normProject.generic_string());
            return {};
        }

        // Step 4: Relativize - lexically_relative() is purely syntactic per the standard,
        //         no FS access, no exceptions.
        const fs::path relative = normAsset.lexically_relative(normProject);

        if (relative.empty())
        {
            // Happens when paths share no common root (e.g. C:\ vs D:\ on Windows,
            // or roots that diverge immediately).
            errorLog(
                "toProjectRelativePath: '{}' cannot be made relative to '{}'. "
                "No common root ancestor (possibly different drive letters on Windows)."_f
                << normAsset.generic_string() << normProject.generic_string());
            return {};
        }

        // Step 5: Warn if the asset escapes the project tree
        if (*relative.begin() == "..")
        {
            warnLog(
                "toProjectRelativePath: asset '{}' is outside the project root '{}'. "
                "Relative path '{}' escapes via '..'."_f
                << normAsset.generic_string() << normProject.generic_string()
                << relative.generic_string());
            // Still return it - policy decision belongs to the caller.
        }

        return relative;
    }
} // namespace Core::Test