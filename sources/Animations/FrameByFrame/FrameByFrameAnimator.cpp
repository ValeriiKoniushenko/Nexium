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

namespace Core
{
    R_FRIEND_IMPL(FrameByFrameAnimator);
    ECS_COMPONENT_IMPL(FrameByFrameAnimator);

    void FrameByFrameAnimator::update() const
    {
        if (_currentAnimation)
        {
            _currentAnimation->update();
        }
    }

    void FrameByFrameAnimator::startAnimation(const StringAtom& name)
    {
        _currentAnimation = getAnimation(name);
        _currentAnimation->reset();
    }

    void FrameByFrameAnimator::addAnimation(FrameByFrameAnimation* animation)
    {
        _animations[animation->getAnimationName()] = animation;
    }

    FRect FrameByFrameAnimator::getCurrentAnimationFrame() const
    {
        return _currentAnimation->getCurrentFrame();
    }

    FrameByFrameAnimation* FrameByFrameAnimator::getAnimation(const StringAtom& name) const
    {
        const auto it = _animations.find(name);
        if (it == _animations.end())
        {
            return nullptr;
        }
        return it->second;
    }
} // namespace Core