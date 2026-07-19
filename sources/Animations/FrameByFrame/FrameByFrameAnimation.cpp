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

#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{

    void FrameByFrameAnimation::update()
    {
        if (_frames.empty() || _isFinished)
        {
            return;
        }

        _timer += GetWorld().getTimeDelta();

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
                    _isFinished = true;
                }
            }
        }
    }

    void FrameByFrameAnimation::setFPS(float fps)
    {
        if (fps <= 0.f)
            throw std::logic_error("FPS must be > 0");
        _frameTime = 1.0f / fps;
    }

    FRect FrameByFrameAnimation::getCurrentFrame() const
    {
        if (_frames.empty())
        {
            errorLog("Animation: '{}' has no frames"_f << _animationName);
            return {};
        }

        return _frames[_currentFrame];
    }
} // namespace Core