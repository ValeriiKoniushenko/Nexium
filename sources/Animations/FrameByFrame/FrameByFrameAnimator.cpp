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

#include "FrameByFrameAnimator.h"

#include "FrameByFrameAnimation.h"
#include "Scene/Rectangle.h"

namespace Core::Animation
{
    ECS_IMPL(FrameByFrameAnimator);

    void FrameByFrameAnimator::update(float delta)
    {
        if (auto* animation = getAnimation(_currentState))
        {
            animation->update(delta);
        }
    }

    bool FrameByFrameAnimator::startAnimation(const StringAtom& name)
    {
        auto* animation = getAnimation(name);
        if (!animation)
        {
            return false;
        }

        _currentState = name;
        animation->reset();
        animation->start();
        applyCurrentFrame();
        return true;
    }

    bool FrameByFrameAnimator::stopAnimation()
    {
        auto* animation = getAnimation(_currentState);
        if (!animation)
        {
            return false;
        }
        animation->stop();
        return true;
    }

    bool FrameByFrameAnimator::pauseAnimation()
    {
        auto* animation = getAnimation(_currentState);
        if (!animation || !animation->isPlaying())
        {
            return false;
        }
        animation->pause();
        return true;
    }

    bool FrameByFrameAnimator::resumeAnimation()
    {
        auto* animation = getAnimation(_currentState);
        if (!animation || !animation->isPaused())
        {
            return false;
        }
        animation->resume();
        return true;
    }

    bool FrameByFrameAnimator::resetAnimation()
    {
        auto* animation = getAnimation(_currentState);
        if (!animation)
        {
            return false;
        }
        animation->reset();
        applyCurrentFrame();
        return true;
    }

    bool FrameByFrameAnimator::finishAnimation()
    {
        auto* animation = getAnimation(_currentState);
        if (!animation)
        {
            return false;
        }
        animation->finish();
        applyCurrentFrame();
        return true;
    }

    bool FrameByFrameAnimator::addAnimation(FrameByFrameAnimation animation)
    {
        if (!animation.isValid())
        {
            return false;
        }

        const auto name = animation.getAnimationName();
        _animations.insert_or_assign(name, std::move(animation));
        return true;
    }

    bool FrameByFrameAnimator::removeAnimation(const StringAtom& name)
    {
        if (_currentState == name)
        {
            _currentState = StringAtom{};
        }
        return _animations.erase(name) > 0;
    }

    void FrameByFrameAnimator::clearAnimations()
    {
        _currentState = StringAtom{};
        _animations.clear();
    }

    const StringAtom& FrameByFrameAnimator::getCurrentAtlasName() const
    {
        if (const auto* animation = getAnimation(_currentState))
        {
            return animation->getAtlasName();
        }

        static const StringAtom empty;
        return empty;
    }

    const StringAtom& FrameByFrameAnimator::getCurrentFrameName() const
    {
        if (const auto* animation = getAnimation(_currentState))
        {
            return animation->getCurrentFrameName();
        }

        static const StringAtom empty;
        return empty;
    }

    FrameByFrameAnimation* FrameByFrameAnimator::getAnimation(const StringAtom& name)
    {
        const auto it = _animations.find(name);
        if (it == _animations.end())
        {
            return nullptr;
        }
        return &it->second;
    }

    const FrameByFrameAnimation* FrameByFrameAnimator::getAnimation(const StringAtom& name) const
    {
        const auto it = _animations.find(name);
        if (it == _animations.end())
        {
            return nullptr;
        }
        return &it->second;
    }

    bool FrameByFrameAnimator::hasAnimation(const StringAtom& name) const
    {
        return _animations.contains(name);
    }

    bool FrameByFrameAnimator::hasCurrentAnimation() const
    {
        return getAnimation(_currentState) != nullptr;
    }

    bool FrameByFrameAnimator::isPlaying() const
    {
        const auto* animation = getAnimation(_currentState);
        return animation && animation->isPlaying();
    }

    void FrameByFrameAnimator::onTick(float delta)
    {
        BaseComponent::onTick(delta);
        update(delta);
        applyCurrentFrame();
    }

    void FrameByFrameAnimator::applyCurrentFrame()
    {
        auto* rectangle = getParentAs<SceneObj::Rectangle>();
        auto* animation = getAnimation(_currentState);
        if (!rectangle || !animation)
        {
            return;
        }

        rectangle->setAtlas(animation->getAtlasName());
        rectangle->setTexture(animation->getCurrentFrameName());
    }
} // namespace Core::Animation
