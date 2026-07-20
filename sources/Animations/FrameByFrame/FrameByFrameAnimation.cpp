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

#include "FrameByFrameAnimation.h"

#include <cmath>

namespace Core
{

    void FrameByFrameAnimation::update(float delta)
    {
        if (!isPlaying() || _frames.empty() || !std::isfinite(delta) || delta <= 0.f)
        {
            return;
        }

        _timer += delta;

        while (_timer >= _frameTime)
        {
            _timer -= _frameTime;
            _currentFrame++;

            if (_currentFrame >= _frames.size())
            {
                if (_isLooping)
                {
                    _currentFrame = 0;
                    _isFinished = false;
                }
                else
                {
                    _currentFrame = static_cast<int>(_frames.size() - 1);
                    finish();
                    break;
                }
            }
        }
    }

    void FrameByFrameAnimation::setFPS(float fps)
    {
        if (!std::isfinite(fps) || fps <= 0.f)
        {
            throw std::logic_error("FPS must be > 0");
        }
        _frameTime = 1.0f / fps;
    }

    void FrameByFrameAnimation::start()
    {
        if (isFinished())
        {
            reset();
        }
        BaseAnimation::start();
    }

    void FrameByFrameAnimation::reset()
    {
        BaseAnimation::reset();
        _currentFrame = 0;
        _timer = 0.f;
    }

    void FrameByFrameAnimation::finish()
    {
        if (!_frames.empty())
        {
            _currentFrame = static_cast<int>(_frames.size() - 1);
        }
        _timer = 0.f;
        BaseAnimation::finish();
    }

    bool FrameByFrameAnimation::addFrame(const StringAtom& frameName)
    {
        if (frameName.isEmpty())
        {
            return false;
        }

        _frames.emplace_back(frameName);
        return true;
    }

    bool FrameByFrameAnimation::isValid() const noexcept
    {
        return !_animationName.isEmpty() && !_atlasName.isEmpty() && !_frames.empty()
            && std::isfinite(_frameTime) && _frameTime > 0.f;
    }

    const StringAtom& FrameByFrameAnimation::getCurrentFrameName() const
    {
        if (_frames.empty())
        {
            static const StringAtom empty;
            return empty;
        }

        if (_currentFrame < 0 || static_cast<std::size_t>(_currentFrame) >= _frames.size())
        {
            static const StringAtom empty;
            return empty;
        }

        return _frames[_currentFrame];
    }
} // namespace Core
