// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/String.h"
#include "JustReflectMe/Adapter.h"
#include "NxFundamental/ECS/BaseComponent.h"
#include "NxSubsystems/PrivateModuleInfo.h"

namespace NX::Animation
{
    CLASS();
    class BaseAnimation : public BaseComponent
    {
        ECS_DECL(BaseAnimation, NX::BaseComponent);

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

        [[nodiscard]] bool isPlaying() const noexcept;

        [[nodiscard]] spdlog::logger* getLogger() const override;

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
} // namespace NX::Animation

#include "BaseAnimation.generated.h" // added by the code generator. Better don't move it.
