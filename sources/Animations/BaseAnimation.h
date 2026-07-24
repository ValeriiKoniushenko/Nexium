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

#include "Core/String.h"
#include "JustReflectMe/Adapter.h"

namespace Core::Animation
{
    CLASS();
    class BaseAnimation
    {
        R_FRIEND(BaseAnimation);

    public:
        BaseAnimation() = default;
        BaseAnimation(const BaseAnimation&) = default;
        BaseAnimation(BaseAnimation&&) noexcept = default;
        BaseAnimation& operator=(const BaseAnimation&) = default;
        BaseAnimation& operator=(BaseAnimation&&) noexcept = default;
        virtual ~BaseAnimation() = default;

        void setAnimationName(const StringAtom& animationName) { _animationName = animationName; }

        [[nodiscard]] StringAtom getAnimationName() const { return _animationName; }

        virtual void stop();
        virtual void start();
        virtual void reset();
        virtual void pause();
        virtual void resume();
        virtual void finish();
        virtual void restart();

        virtual void update(float delta) = 0;

        void setLoop(bool value) noexcept { _isLooping = value; }

        [[nodiscard]] bool isStopped() const noexcept { return _isStopped; }
        [[nodiscard]] bool isPaused() const noexcept { return _isPaused; }
        [[nodiscard]] bool isLooping() const noexcept { return _isLooping; }
        [[nodiscard]] bool isFinished() const noexcept { return _isFinished; }
        [[nodiscard]] bool isPlaying() const noexcept
        {
            return !_isStopped && !_isPaused && !_isFinished;
        }

    protected:
        FIELD();
        StringAtom _animationName;
        FIELD();
        bool _isLooping{ false };
        FIELD();
        bool _isStopped{ true };
        FIELD();
        bool _isPaused{ false };
        FIELD();
        bool _isFinished{ false };
    };

} // namespace Core::Animation

#include "BaseAnimation.generated.h" // added by the code generator. Better don't move it.
