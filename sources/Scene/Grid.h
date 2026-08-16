/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "Core/Color.h"

namespace Core
{
    class HorizontalGrid
    {
    public:
        [[nodiscard]] float getGridSize() const noexcept { return _gridSize; }
        [[nodiscard]] float getCellSize() const noexcept { return _cellSize; }
        void setGridSize(float value) noexcept { _gridSize = value; }
        void setCellSize(float value) noexcept { _cellSize = value; }

        [[nodiscard]] bool isDraw() const noexcept { return _isDraw; }
        void setIsDraw(bool value) noexcept { _isDraw = value; }

        void draw();

    protected:
        float _gridSize = 10'000.f;
        float _cellSize = 100.f;
        bool _isDraw = true;
    };

    class VerticalGrid
    {
    public:
        [[nodiscard]] float getGridSize() const noexcept { return _gridSize; }
        [[nodiscard]] float getCellSize() const noexcept { return _cellSize; }
        void setGridSize(float value) noexcept { _gridSize = value; }
        void setCellSize(float value) noexcept { _cellSize = value; }

        [[nodiscard]] bool isDraw() const noexcept { return _isDraw; }
        void setIsDraw(bool value) noexcept { _isDraw = value; }

        void draw();

    protected:
        float _gridSize = 10'000.f;
        float _cellSize = 100.f;
        bool _isDraw = true;
    };

} // namespace Core