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
        j["name"] = v.name;
        j["textureName"] = v.textureName;
        j["uvOffset"] = v.uvOffset;
        j["uvSize"] = v.uvSize;
    }

    void from_json(const nlohmann::json& j, Frame& v)
    {
        v.name = j.value("name", StringAtom{});
        if (j.contains("textureName") && !j["textureName"].is_null())
        {
            v.textureName = j["textureName"].get<StringAtom>();
        }
        else
        {
            v.textureName = std::nullopt;
        }
        v.uvOffset = j["uvOffset"].get<GlobalPosition2F>();
        v.uvSize = j["uvSize"].get<GlobalPosition2F>();
    }

    ECS_IMPL(Core::Animation::FrameByFrameAnimation);

    void FrameByFrameAnimation::update(float delta)
    {
        if (!isPlaying() || _frames.empty() || delta <= 0.f)
        {
            return;
        }

        _timer += delta;

        const float frameTime = 1.f / _fps;
        while (_timer >= frameTime)
        {
            _timer -= frameTime;
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
        _fps = fps;
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

        _frames.emplace_back(Frame{
            .name = textureName,
            .textureName = std::move(textureName)
        });
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
            .name = "Frame "_atom + StringAtom::MakeFrom(_frames.size() + 1),
            .textureName = std::nullopt,
            .uvOffset = std::move(uvOffset),
            .uvSize = std::move(uvSize)
        });
        return true;
    }

    bool FrameByFrameAnimation::addFramesFromSpriteSheet(std::size_t columns, std::size_t rows,
                                                         std::size_t frameCount,
                                                         std::size_t startRow)
    {
        if (columns == 0 || rows == 0 || columns > std::numeric_limits<std::size_t>::max() / rows)
        {
            return false;
        }

        const auto capacity = columns * rows;
        if (startRow >= rows)
        {
            return false;
        }
        if (frameCount == 0)
        {
            frameCount = capacity - startRow * columns;
        }
        if (frameCount > capacity - startRow * columns)
        {
            return false;
        }

        const auto frameSize
            = GlobalPosition2F{1.f / static_cast<float>(columns), 1.f / static_cast<float>(rows)};
        _frames.reserve(_frames.size() + frameCount);

        for (std::size_t index = 0; index < frameCount; ++index)
        {
            const auto sheetIndex = startRow * columns + index;
            const auto column = sheetIndex % columns;
            const auto row = sheetIndex / columns;
            _frames.emplace_back(
                Frame{
                    .name = "Frame "_atom + StringAtom::MakeFrom(_frames.size() + 1),
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

    bool FrameByFrameAnimation::setFrame(std::size_t index, Frame frame)
    {
        if (index >= _frames.size() || frame.uvOffset.x < 0.f || frame.uvOffset.y < 0.f
            || frame.uvSize.x <= 0.f || frame.uvSize.y <= 0.f
            || frame.uvOffset.x + frame.uvSize.x > 1.f
            || frame.uvOffset.y + frame.uvSize.y > 1.f)
        {
            return false;
        }
        _frames[index] = std::move(frame);
        return true;
    }

    bool FrameByFrameAnimation::removeFrame(std::size_t index)
    {
        if (index >= _frames.size())
        {
            return false;
        }
        _frames.erase(_frames.begin() + static_cast<std::ptrdiff_t>(index));
        _currentFrame = std::min(_currentFrame, _frames.empty() ? std::size_t{0}
                                                                : _frames.size() - 1);
        return true;
    }

    void FrameByFrameAnimation::clearFrames()
    {
        _frames.clear();
        _currentFrame = 0;
        _timer = 0.f;
    }

    bool FrameByFrameAnimation::isValid() const noexcept
    {
        if (getComponentName().isEmpty() || _atlasName.isEmpty() || _frames.empty()
            || _fps <= 0.f)
        {
            return false;
        }

        return std::ranges::all_of(
            _frames, [this](const Frame& frame)
            {
                const bool validUV = frame.uvOffset.x >= 0.f && frame.uvOffset.y >= 0.f
                    && frame.uvSize.x > 0.f && frame.uvSize.y > 0.f
                    && frame.uvOffset.x + frame.uvSize.x <= 1.f
                    && frame.uvOffset.y + frame.uvSize.y <= 1.f;
                return validUV;
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
