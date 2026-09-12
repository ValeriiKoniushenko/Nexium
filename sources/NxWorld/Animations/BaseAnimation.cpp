// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "BaseAnimation.h"

namespace NX::Animation
{
    ECS_IMPL(BaseAnimation);

    void BaseAnimation::stop()
    {
        _isStopped = true;
        _isPaused = false;
    }

    void BaseAnimation::start()
    {
        _isStopped = false;
        _isPaused = false;
        _isFinished = false;
    }

    void BaseAnimation::reset()
    {
        _isStopped = true;
        _isPaused = false;
        _isFinished = false;
    }

    void BaseAnimation::pause()
    {
        if (isPlaying())
        {
            _isPaused = true;
        }
    }

    void BaseAnimation::resume()
    {
        if (_isPaused && !_isFinished)
        {
            _isStopped = false;
            _isPaused = false;
        }
    }

    void BaseAnimation::finish()
    {
        _isStopped = true;
        _isPaused = false;
        _isFinished = true;
    }

    void BaseAnimation::restart()
    {
        reset();
        start();
    }

    spdlog::logger* BaseAnimation::getLogger() const
    {
        return NxSubsystems::getLogger();
    };

    bool BaseAnimation::isPlaying() const noexcept
    {
        return !_isStopped && !_isPaused && !_isFinished;
    }
} // namespace NX::Animation
