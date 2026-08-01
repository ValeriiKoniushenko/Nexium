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

    CLASS();
    struct Frame
    {
        R_FRIEND(Frame);

        FIELD();
        StringAtom name;
        FIELD();
        std::optional<StringAtom> textureName;
        FIELD();
        GlobalPosition2F uvOffset = Core::GlobalPosition2F{ 0.f, 0.f };
        FIELD();
        GlobalPosition2F uvSize = Core::GlobalPosition2F{ 1.f, 1.f };
    };

    void to_json(nlohmann::json& j, const Frame& v);
    void from_json(const nlohmann::json& j, Frame& v);

    CLASS();
    class FrameByFrameAnimation : public BaseAnimation
    {
        ECS_DECL(FrameByFrameAnimation, Core::Animation::BaseAnimation);

    public:
        void setFPS(float fps);
        [[nodiscard]] float getFPS() const noexcept { return _fps; }
        void start() override;
        void reset() override;
        void finish() override;

        void setAtlasName(const StringAtom& atlasName) { _atlasName = atlasName; }
        void setTextureName(const StringAtom& textureName) { _textureName = textureName; }

        bool addFrame(StringAtom textureName);
        bool addFrame(GlobalPosition2F uvOffset, GlobalPosition2F uvSize);
        bool addFramesFromSpriteSheet(std::size_t columns, std::size_t rows,
                                      std::size_t frameCount = 0, std::size_t startRow = 0);
        bool setFrame(std::size_t index, const Frame& frame);
        bool removeFrame(std::size_t index);
        void clearFrames();

        [[nodiscard]] bool isValid() const override;
        [[nodiscard]] const Frame* getCurrentFrame() const;
        [[nodiscard]] bool hasFrames() const noexcept { return !_frames.empty(); }
        [[nodiscard]] std::size_t getFramesCount() const noexcept { return _frames.size(); }
        [[nodiscard]] const std::vector<Frame>& getFrames() const noexcept { return _frames; }
        [[nodiscard]] const StringAtom& getAtlasName() const noexcept { return _atlasName; }
        [[nodiscard]] const StringAtom& getTextureName() const noexcept { return _textureName; }

        void onTick(float delta) override;

        [[nodiscard]] spdlog::logger* getLogger() const override
        {
            return Animations::getLogger();
        };

    private:
        FIELD();
        StringAtom _atlasName;
        FIELD();
        StringAtom _textureName;
        FIELD();
        std::vector<Frame> _frames;

        FIELD();
        std::size_t _currentFrame = 0;
        FIELD();
        float _frameTimeAccumulator = 0.f;
        FIELD();
        float _fps = 10.f;
    };
} // namespace Core::Animation

#include "FrameByFrameAnimation.generated.h" // added by the code generator. Better don't move it.
