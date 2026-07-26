#include "Player.h"

#include "Animations/FrameByFrame/FrameByFrameAnimation.h"
#include "Animations/FrameByFrame/FrameByFrameAnimator.h"

namespace Game
{
    bool Player::bind(Core::SceneObj::Rectangle* rectangle)
    {
        if (!rectangle)
        {
            return false;
        }

        auto* animator =
            rectangle->findFirstChildOf<Core::Animation::FrameByFrameAnimator>();
        if (!animator)
        {
            return false;
        }

        _rectangle = rectangle;
        _animator = animator;

        return configureAnimations();
    }

    bool Player::configureAnimations()
    {
        Core::Animation::FrameByFrameAnimation walk;
        walk.setAnimationName("walk"_atom);
        walk.setAtlasName("santa_walk"_atom);
        walk.setFPS(12.0f);
        walk.setLoop(true);

        walk.addFrame("Walk _1.png"_f);
        for (int frame = 2; frame <= 13; ++frame)
        {
            walk.addFrame("Walk_{}.png"_f << frame);
        }

        return _animator->addAnimation(std::move(walk));
    }

    bool Player::playWalk()
    {
        return _animator && _animator->startAnimation("walk"_atom);
    }
} // namespace Game
