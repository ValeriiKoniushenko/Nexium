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

#include "TextureAsset.h"

#include "Editor/Configs.h"

namespace Core
{
    void TextureAsset::onLoadRequest()
    {
        _data.release();
        _data.loadFromFile(_path, _isFlipVertically);
        traceLog("Loaded: " + _logicPath);
    }

    void TextureAsset::onUnloadRequest()
    {
        traceLog("Unloaded: " + _logicPath);
        _data.release();
    }

    void TextureAsset::onFillData(nlohmann::json&& json)
    {
        if (Verify(json.contains("path")))
        {
            _path = Config::Path::assets / json["path"].get<std::filesystem::path>();
        }
        else
        {
            errorLog(
                "The asset '{}' is configured incorrectly. The property 'path' is not determined."_f
                << _logicPath);
        }

        if (json.contains("isFlipVertically"))
        {
            _isFlipVertically = json["isFlipVertically"].get<bool>();
        }
    }
} // namespace Core