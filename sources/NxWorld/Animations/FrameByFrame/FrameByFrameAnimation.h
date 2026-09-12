// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "../BaseAnimation.h"
#include "Core/Position.h"
#include "Foundation/BaseLog.h"

#include <algorithm>
#include <optional>
#include <utility>
#include <vector>

namespace NX::Animation
{
    using Core::GlobalPosition2F;

    CLASS();
    struct Frame
    {
        R_FRIEND(Frame);

        FIELD();
        Core::StringAtom name;
        FIELD();
        std::optional<Core::StringAtom> textureName;
        FIELD();
        Core::GlobalPosition2F uvOffset = Core::GlobalPosition2F{ 0.f, 0.f };
        FIELD();
        Core::GlobalPosition2F uvSize = Core::GlobalPosition2F{ 1.f, 1.f };
    };

    void to_json(nlohmann::json& j, const Frame& v);
    void from_json(const nlohmann::json& j, Frame& v);

    CLASS();
    class FrameByFrameAnimation : public BaseAnimation
    {
        ECS_DECL(FrameByFrameAnimation, NX::Animation::BaseAnimation);

    public:
        void setFPS(float fps);
        [[nodiscard]] float getFPS() const noexcept { return _fps; }
        void start() override;
        void reset() override;
        void finish() override;

        void setAtlasName(const Core::StringAtom& atlasName) { _atlasName = atlasName; }
        void setTextureName(const Core::StringAtom& textureName) { _textureName = textureName; }

        bool addFrame(Core::StringAtom textureName);
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
        [[nodiscard]] const Core::StringAtom& getAtlasName() const noexcept { return _atlasName; }
        [[nodiscard]] const Core::StringAtom& getTextureName() const noexcept
        {
            return _textureName;
        }

        [[nodiscard]] spdlog::logger* getLogger() const override
        {
            return NxSubsystems::getLogger();
        };

    protected:
        void onTick(float delta) override;

    private:
        FIELD();
        Core::StringAtom _atlasName;
        FIELD();
        Core::StringAtom _textureName;
        FIELD();
        std::vector<NX::Animation::Frame> _frames;

        FIELD();
        std::size_t _currentFrame = 0;
        FIELD();
        float _frameTimeAccumulator = 0.f;
        FIELD();
        float _fps = 10.f;
    };
} // namespace NX::Animation

#include "FrameByFrameAnimation.generated.h" // added by the code generator. Better don't move it.
