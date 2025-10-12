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
#include "Camera/Camera.h"

namespace Core
{
    class CameraAsset : public BaseAsset
    {
    public:
        inline static const char* fileExtension = ".nxcamera";

    public:
        explicit CameraAsset(const StringAtom& logicPath)
            : BaseAsset(logicPath)
        {
        }

        ~CameraAsset() override = default;

        void onLoadRequest() override;

        void onUnloadRequest() override;

        [[nodiscard]] const char* getPrefix() const override { return "Camera"; }

        [[nodiscard]] BaseCamera& getData() noexcept { return _data; }

    protected:
        void ioFieldsUpdate(DataStream& stream) override;

    protected:
        BaseCamera _data;

        // properties
        float _fov = 75.f;
        float _far = 10'000.f;
        float _near = 0.1f;
        FSize2 _frameSize = FSize2{ 600, 600 };
        bool _isPostDraw = false;
    };

    using NXCamera = AssetRef<CameraAsset>;
} // namespace Core