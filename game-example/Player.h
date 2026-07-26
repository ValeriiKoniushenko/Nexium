#pragma once

#include "Scene/Rectangle.h"

namespace Core::Animation
{
    class FrameByFrameAnimator;
}

namespace Game
{
    class Player
    {
    public:
        bool bind(Core::SceneObj::Rectangle* rectangle);
        bool playWalk();

        [[nodiscard]] Core::SceneObj::Rectangle* getRectangle() noexcept
        {
            return _rectangle;
        }

        [[nodiscard]] const Core::SceneObj::Rectangle* getRectangle() const noexcept
        {
            return _rectangle;
        }

    private:
        bool configureAnimations();

        Core::SceneObj::Rectangle* _rectangle{};
        Core::Animation::FrameByFrameAnimator* _animator{};
    };
}
