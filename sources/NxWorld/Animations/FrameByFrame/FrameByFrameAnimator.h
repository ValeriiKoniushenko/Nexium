// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/String.h"
#include "FrameByFrameAnimation.h"
#include "JustReflectMe/Adapter.h"
#include "NxFundamental/ECS/BaseComponent.h"

#include <unordered_map>

namespace NX::Animation
{
    CLASS();
    class FrameByFrameAnimator : public BaseComponent
    {
        ECS_DECL(FrameByFrameAnimator, NX::BaseComponent);

    public:
        bool startAnimation(const Core::StringAtom& name);

        bool addAnimation(const FrameByFrameAnimation& animation);
        bool removeAnimation(const Core::StringAtom& name);
        void clearAnimations();

        [[nodiscard]] FrameByFrameAnimation* getAnimation(const Core::StringAtom& name);

        [[nodiscard]] const FrameByFrameAnimation* getAnimation(const Core::StringAtom& name) const;

        [[nodiscard]] FrameByFrameAnimation* getActiveAnimation();
        [[nodiscard]] const FrameByFrameAnimation* getActiveAnimation() const;

        [[nodiscard]] bool containAnimation(const Core::StringAtom& name) const;
        [[nodiscard]] const Core::StringAtom& getActiveAnimationName() const noexcept
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
        Core::StringAtom _currentAnimationName;
    };

    void to_json(nlohmann::json& j,
                 const std::unordered_map<Core::StringAtom, BaseAnimation::Ptr>& v);
    void from_json(const nlohmann::json& j,
                   std::unordered_map<Core::StringAtom, BaseAnimation::Ptr>& v);
} // namespace NX::Animation

#include "FrameByFrameAnimator.generated.h" // added by the code generator. Better don't move it.
