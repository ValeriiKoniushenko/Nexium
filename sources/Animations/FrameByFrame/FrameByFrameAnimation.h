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
#include "Core/Position.h"
#include "Misc/BaseLog.h"

#include <algorithm>
#include <optional>
#include <utility>
#include <vector>

namespace Core::Animation
{
    struct Frame
    {
        std::optional<StringAtom> textureName;
        GlobalPosition2F uvOffset{0.f, 0.f};
        GlobalPosition2F uvSize{1.f, 1.f};
    };

    class FrameByFrameAnimation : public BaseAnimation, public BaseLog
    {
    public:
        void setFPS(float fps);
        void start() override;
        void reset() override;
        void finish() override;

        void setAtlasName(StringAtom atlasName) { _atlasName = std::move(atlasName); }
        void setTextureName(StringAtom textureName) { _textureName = std::move(textureName); }

        bool addFrame(StringAtom textureName);
        bool addFrame(GlobalPosition2F uvOffset, GlobalPosition2F uvSize);
        bool addFramesFromSpriteSheet(std::size_t columns, std::size_t rows,
                                      std::size_t frameCount = 0);

        [[nodiscard]] bool isValid() const noexcept;
        [[nodiscard]] const Frame* getCurrentFrame() const noexcept;
        [[nodiscard]] bool hasFrames() const noexcept { return !_frames.empty(); }
        [[nodiscard]] std::size_t getFramesCount() const noexcept { return _frames.size(); }
        [[nodiscard]] const StringAtom& getAtlasName() const noexcept { return _atlasName; }
        [[nodiscard]] const StringAtom& getTextureName() const noexcept { return _textureName; }

        void update(float delta) override;

        [[nodiscard]] virtual spdlog::logger* getLogger() const { return nullptr; };

    private:
        StringAtom _atlasName;
        StringAtom _textureName;
        std::vector<Frame> _frames;

        std::size_t _currentFrame{0};
        float _timer{0.f};
        float _frameTime{0.1f};
    };
} // namespace Core::Animation
