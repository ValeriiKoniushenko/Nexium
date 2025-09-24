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

namespace Core
{
    void AssetsManager::rescanFileSystem()
    {
        const auto absBasePath = std::filesystem::absolute(Config::Path::assets);

        for (const auto& entry :
             std::filesystem::recursive_directory_iterator(Config::Path::bakedAssets))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            const auto absPath = std::filesystem::absolute(entry.path());
            const auto ext = absPath.extension().generic_string();
            auto relPath = std::filesystem::relative(absPath, absBasePath).generic_string();

            // check for non baked
            if (ext.size() < 3 && strncmp(ext.c_str(), ".nx", 3) != 0)
            {
                continue;
            }

            if (ext == ".nxtex")
            {
                auto id = StringAtom::Intern(relPath);
                _textures.emplace(id, std::make_unique<TextureAsset>());
            }
        }
    }

    NXTexture AssetsManager::getTexture(const StringAtom& logicPath)
    {
        Assert(logicPath.isStatic(), "You must use only _atom strings!");

        if (!_textures.contains(logicPath))
        {
            throw std::runtime_error("Texture not found!");
        }

        return NXTexture(reinterpret_cast<TextureAsset&>(*_textures.at(logicPath)));
    }
} // namespace Core