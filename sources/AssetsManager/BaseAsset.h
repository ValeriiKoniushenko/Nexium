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

#include "Core/String.h"

namespace Core
{
    class BaseAsset
    {
    public:
        virtual ~BaseAsset() = default;

        [[nodiscard]] bool isLoaded() const { return _refCount > 1; }

        [[nodiscard]] const StringAtom& getPath() const { return _path; }

        virtual void onLoadRequest() = 0;
        virtual void onUnloadRequest() = 0;

    protected:
    protected:
        StringAtom _path;
        uint32_t _refCount = 0;

        template<class T>
        friend class AssetRef;
    };

    template<class T>
    class AssetRef
    {
    public:
        explicit AssetRef(T& asset)
            : _asset(asset)
        {
            if (++_asset._refCount > 0)
            {
                _asset.onLoadRequest();
            }
        }

        [[nodiscard]] T& get() { return _asset; }
        [[nodiscard]] const T& get() const { return _asset; }
        [[nodiscard]] T& operator*() { return _asset; }
        [[nodiscard]] const T& operator*() const { return _asset; }
        [[nodiscard]] T& operator->() { return _asset; }
        [[nodiscard]] const T& operator->() const { return _asset; }

        ~AssetRef()
        {
            Assert(_asset._refCount != 0,
                   "Invalid ref count, it will be less than zero - impossible.");
            if (--_asset._refCount == 0)
            {
                _asset.onUnloadRequest();
            }
        }

    private:
        T& _asset;
    };
} // namespace Core