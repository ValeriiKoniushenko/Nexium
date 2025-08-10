// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "OpenGL.h"

namespace SW
{

    void UpdateGlViewport(Core::FSize2 originalSize, float aspectRatio, ViewportMode mode)
    {
        Core::FSize2 view = originalSize;
        glm::vec2 pos = {};

        if (mode.cast() == ViewportMode::Default)
        {
            view.height = view.width / aspectRatio;

            if (view.height > originalSize.height)
            {
                view.height = originalSize.height;
                view.width = originalSize.height * aspectRatio;
            }

            pos.x = (originalSize.width - view.width) / 2.f;
            pos.y = (originalSize.height - view.height) / 2.f;
        }
        else
        {
            view.width = view.height * aspectRatio;

            if (view.width < originalSize.width)
            {
                view.width = originalSize.width;
                view.height = view.width / aspectRatio;
            }
            pos.x = (originalSize.width - view.width) / 2.f;
            pos.y = 0;
        }

        glViewport(static_cast<GLint>(pos.x), static_cast<GLint>(pos.y),
                   static_cast<GLsizei>(view.width), static_cast<GLsizei>(view.height));
    }

} // namespace SW