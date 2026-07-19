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

#include "Animations/BaseAnimation.h"
#include "Core/Rect.h"
#include "Misc/BaseLog.h"

#include <vector>

namespace Core
{

    class FrameByFrameAnimation : public BaseAnimation, public BaseLog
    {
    public:
        FrameByFrameAnimation() = default;

        void update();
        void setFPS(float fps);

        void addFrame(FRect frame) { _frames.emplace_back(frame); }

        [[nodiscard]] FRect getCurrentFrame() const;

    protected:
        [[nodiscard]] spdlog::logger* getLogger() const override { return nullptr; }

    private:
        std::vector<FRect> _frames;

        int _currentFrame{ 0 };
        float _timer{ 0.f };
        float _frameTime{ 0.1f };
    };

} // namespace Core
