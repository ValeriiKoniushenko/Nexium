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

#include "BaseAsset.h"
#include "Graphics/Texture.h"

namespace Core
{
    class TextureAsset : public BaseAsset
    {
    public:
        explicit TextureAsset(const StringAtom& logicPath)
            : BaseAsset(logicPath)
        {
        }

        ~TextureAsset() override = default;

        void onLoadRequest() override;
        void onUnloadRequest() override;
        void onFillData(nlohmann::json&& json) override;

        [[nodiscard]] const Texture& getData() const noexcept { return _data; }
        [[nodiscard]] Texture& getData() noexcept { return _data; }

    protected:
        Texture _data;

        // properties
        bool _isFlipVertically = false;
    };

    using NXTexture = AssetRef<TextureAsset>;
} // namespace Core