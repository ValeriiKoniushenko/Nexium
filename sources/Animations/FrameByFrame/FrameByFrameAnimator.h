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

        bool addAnimation(const FrameByFrameAnimation& animation);
        bool removeAnimation(const StringAtom& name);
        void clearAnimations();

        [[nodiscard]] FrameByFrameAnimation* getAnimation(const StringAtom& name);

        [[nodiscard]] const FrameByFrameAnimation* getAnimation(const StringAtom& name) const;

        [[nodiscard]] FrameByFrameAnimation* getActiveAnimation();
        [[nodiscard]] const FrameByFrameAnimation* getActiveAnimation() const;

        [[nodiscard]] bool containAnimation(const StringAtom& name) const;
        [[nodiscard]] const StringAtom& getActiveAnimationName() const noexcept
        {
            return _currentAnimationName;
        }
        [[nodiscard]] const auto& getAnimations() const noexcept { return _animations; }
        [[nodiscard]] Tag getTags() const override;

    protected:
        void onTick(float delta) override;

    private:
        void applyCurrentFrameToRectangle();
        void updateCurrentAnimation(float delta);

    private:
        FIELD();
        std::unordered_map<Core::StringAtom, BaseAnimation::Ptr> _animations;

        FIELD();
        StringAtom _currentAnimationName;
    };

    void to_json(nlohmann::json& j, const std::unordered_map<StringAtom, BaseAnimation::Ptr>& v);
    void from_json(const nlohmann::json& j, std::unordered_map<StringAtom, BaseAnimation::Ptr>& v);
} // namespace Core::Animation

#include "FrameByFrameAnimator.generated.h" // added by the code generator. Better don't move it.
