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

#include "Animations/FrameByFrame/FrameByFrameAnimation.h"
#include "Core/String.h"
#include "ECS/BaseComponent.h"
#include "JustReflectMe/Adapter.h"

#include <unordered_map>

namespace Core::Animation
{

    CLASS();
    class FrameByFrameAnimator : public BaseComponent
    {
        ECS_DECL(FrameByFrameAnimator, Core::BaseComponent);

    public:
        bool startAnimation(const StringAtom& name);
        bool stopAnimation();
        bool pauseAnimation();
        bool resumeAnimation();
        bool resetAnimation();
        bool finishAnimation();

        bool addAnimation(FrameByFrameAnimation animation);
        bool removeAnimation(const StringAtom& name);
        void clearAnimations();

        [[nodiscard]] const StringAtom& getCurrentAtlasName() const;
        [[nodiscard]] const Frame* getCurrentFrame() const;
        [[nodiscard]] FrameByFrameAnimation* getAnimation(const StringAtom& name)
        {
            const auto it = _animations.find(name);
            if (it == _animations.end()) [[unlikely]]
            {
                return nullptr;
            }
            return &it->second;
        }

        [[nodiscard]] const FrameByFrameAnimation*
            getAnimation(const StringAtom& name) const
        {
            const auto it = _animations.find(name);
            if (it == _animations.end()) [[unlikely]]
            {
                return nullptr;
            }
            return &it->second;
        }
        [[nodiscard]] bool hasAnimation(const StringAtom& name) const;
        [[nodiscard]] bool hasCurrentAnimation() const;
        [[nodiscard]] bool isPlaying() const;

    protected:
        void onTick(float delta) override;

    private:
        void applyCurrentFrame();
        void updateCurrentAnimation(float delta);

        std::unordered_map<StringAtom, FrameByFrameAnimation> _animations;

        FIELD();
        StringAtom _currentState;
    };
} // namespace Core::Animation

#include "FrameByFrameAnimator.generated.h" // added by the code generator. Better don't move it.
