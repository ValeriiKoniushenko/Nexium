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
#include "ECS/BaseComponent.h"
#include "JustReflectMe/Adapter.h"
#include "ModuleInfo.h"

namespace Core::Animation
{
    CLASS();
    class BaseAnimation : public BaseComponent
    {
        ECS_DECL(BaseAnimation, Core::BaseComponent);

    public:
        BaseAnimation(const BaseAnimation&) = default;
        BaseAnimation(BaseAnimation&&) noexcept = default;
        BaseAnimation& operator=(const BaseAnimation&) = default;
        BaseAnimation& operator=(BaseAnimation&&) noexcept = default;
        ~BaseAnimation() override = default;

        /**
         * Stops playback without resetting the current playback position.
         * State transition: Running/Paused -> Stopped.
         */
        virtual void stop();

        /**
         * Starts playback from the current playback position.
         *
         * State transition: Idle/Stopped -> Running.
         */
        virtual void start();

        /**
         * Restores the animation to its initial playback position without starting it.
         *
         * State transition: Stopped/Finished -> Idle.
         */
        virtual void reset();

        /**
         * Suspends a running animation at its current playback position.
         *
         * State transition: Running -> Paused.
         * Has no effect unless the animation is running.
         */
        virtual void pause();

        /**
         * Continues playback from the position at which the animation was paused.
         *
         * State transition: Paused -> Running.
         * Has no effect unless the animation is paused.
         */
        virtual void resume();

        /**
         * Completes the animation immediately and moves it to its final playback position.
         *
         * State transition: Running/Paused -> Finished.
         */
        virtual void finish();

        /**
         * Resets the animation and starts it again from its initial playback position.
         *
         * State transition: Any state -> Running.
         */
        virtual void restart();

        void setLoop(bool value) noexcept { _isLooping = value; }

        [[nodiscard]] bool isStopped() const noexcept { return _isStopped; }
        [[nodiscard]] bool isPaused() const noexcept { return _isPaused; }
        [[nodiscard]] bool isLooping() const noexcept { return _isLooping; }
        [[nodiscard]] bool isFinished() const noexcept { return _isFinished; }

        [[nodiscard]] bool isPlaying() const noexcept
        {
            return !_isStopped && !_isPaused && !_isFinished;
        }

        [[nodiscard]] spdlog::logger* getLogger() const override
        {
            return Animations::getLogger();
        };

    protected:
        FIELD();
        bool _isLooping = false;
        FIELD();
        bool _isStopped = true;
        FIELD();
        bool _isPaused = false;
        FIELD();
        bool _isFinished = false;
    };
} // namespace Core::Animation

#include "BaseAnimation.generated.h" // added by the code generator. Better don't move it.
