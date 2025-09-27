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

#include "OpenGL.h"

#include <Core/Size.h>

namespace Core
{
    class RenderTargetToTexture
    {
    public:
        void generate();

        void callMePreDraw() const;

        void callMeAfterDraw() const;

        [[nodiscard]] GLuint getTextureId() const noexcept { return _tex; }
        [[nodiscard]] ISize2 getRenderSize() const noexcept { return _size; }

        void setRenderSize(ISize2 size);

        void destroy();

    private:
        ISize2 _size = ISize2{ 400, 400 };
        GLuint _fbo = 0, _tex = 0, _rbo = 0;
        bool _isGenerated = false;
    };
} // namespace Core