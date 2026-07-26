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

#include <limits>

namespace Core::Animation
{
    void to_json(nlohmann::json& j, const Frame& v)
    {
        j["textureName"]= v.textureName;
        j["uvOffset"]= v.uvOffset;
        j["uvSize"]= v.uvSize;
    }

    void from_json(const nlohmann::json& j, Frame& v)
    {
        v.textureName= j["textureName"].get<std::string>();
        v.uvOffset= j["uvOffset"].get<GlobalPosition2F>();
        v.uvSize= j["uvSize"].get<GlobalPosition2F>();
    }

    ECS_IMPL(Core::Animation::FrameByFrameAnimation);

    void FrameByFrameAnimation::update(float delta)
    {
        if (!isPlaying() || _frames.empty() || delta <= 0.f)
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
                    finish();
                    break;
                }
            }
        }
    }

    void FrameByFrameAnimation::setFPS(float fps)
    {
        if (fps <= 0.f)
        {
            errorLog("FPS must be > 0");
            return;
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
        BaseAnimation::finish();

        if (!_frames.empty())
        {
            _currentFrame = _frames.size() - 1;
        }
        _timer = 0.f;
    }

    bool FrameByFrameAnimation::addFrame(StringAtom textureName)
    {
        if (textureName.isEmpty())
        {
            return false;
        }

        _frames.emplace_back(Frame{.textureName = std::move(textureName)});
        return true;
    }

    bool FrameByFrameAnimation::addFrame(GlobalPosition2F uvOffset, GlobalPosition2F uvSize)
    {
        if (uvOffset.x < 0.f || uvOffset.y < 0.f || uvSize.x <= 0.f || uvSize.y <= 0.f
            || uvOffset.x + uvSize.x > 1.f || uvOffset.y + uvSize.y > 1.f)
        {
            return false;
        }

        _frames.emplace_back(Frame{
            .textureName = std::nullopt,
            .uvOffset = std::move(uvOffset),
            .uvSize = std::move(uvSize)
        });
        return true;
    }

    bool FrameByFrameAnimation::addFramesFromSpriteSheet(std::size_t columns, std::size_t rows,
                                                         std::size_t frameCount)
    {
        if (columns == 0 || rows == 0 || columns > std::numeric_limits<std::size_t>::max() / rows)
        {
            return false;
        }

        const auto capacity = columns * rows;
        if (frameCount == 0)
        {
            frameCount = capacity;
        }
        if (frameCount > capacity)
        {
            return false;
        }

        const auto frameSize
            = GlobalPosition2F{1.f / static_cast<float>(columns), 1.f / static_cast<float>(rows)};
        _frames.reserve(_frames.size() + frameCount);

        for (std::size_t index = 0; index < frameCount; ++index)
        {
            const auto column = index % columns;
            const auto row = index / columns;
            _frames.emplace_back(
                Frame{
                    .textureName = std::nullopt,
                    .uvOffset = GlobalPosition2F{
                        static_cast<float>(column) * frameSize.x,
                        static_cast<float>(row) * frameSize.y
                    },
                    .uvSize = frameSize
                });
        }

        return true;
    }

    bool FrameByFrameAnimation::isValid() const noexcept
    {
        if (_animationName.isEmpty() || _atlasName.isEmpty() || _frames.empty()
            || _frameTime <= 0.f)
        {
            return false;
        }

        return !_textureName.isEmpty()
            || std::ranges::all_of(
                _frames, [](const Frame& frame)
                {
                    return frame.textureName && !frame.textureName->isEmpty();
                });
    }

    const Frame* FrameByFrameAnimation::getCurrentFrame() const noexcept
    {
        if (_currentFrame >= _frames.size())
        {
            return nullptr;
        }

        return &_frames[_currentFrame];
    }
} // namespace Core::Animation
