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

#include "Misc/Configs.h"

namespace Core
{
    void TextureAsset::setFilePath(const std::filesystem::path& value)
    {
        Assert(value.is_relative(), "Path should be relative");
        _path = value;
    }

    void TextureAsset::onLoadRequest()
    {
        _data.release();

        if (_path.empty()) [[unlikely]]
        {
            errorLog("Can't load the texture. The path is empty.");
            return;
        }

        Image img;
        if (!img.loadFromFile(Config::Path::projectAbsPath / _path, _isFlipVertically))
        {
            return;
        }

        _size = img.getSize();
        _channels = img.getChannel();

        _data.loadFromImage(img);
    }

    void TextureAsset::onUnloadRequest()
    {
        _data.release();
    }

    nlohmann::json TextureAsset::serialize() const
    {
        RResourceStream<RJsonResourceStream> out(BaseAsset::serialize());
        R<TextureAsset>::Serialize(*this, out);
        return out.getData();
    }

    void TextureAsset::deserialize(const nlohmann::json& json)
    {
        BaseAsset::deserialize(json);
        R<TextureAsset>::Deserialize({ json }, *this);
    }

} // namespace Core