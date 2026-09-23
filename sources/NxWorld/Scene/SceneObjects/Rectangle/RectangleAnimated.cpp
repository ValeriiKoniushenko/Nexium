// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "RectangleAnimated.h"

#include "NxWorld/Animations/FrameByFrame/FrameByFrameAnimator.h"

using namespace Core;

namespace NX::SceneObj
{
    ECS_IMPL(RectangleAnimated);

    void RectangleAnimated::setAnimationEnabled(bool value)
    {
        _animationEnabled = value;

        auto* animator = findFirstChildOf<Animation::FrameByFrameAnimator>();
        if (!animator)
        {
            return;
        }

        animator->setEnabled(value);
        if (!value)
        {
            if (auto* animation = animator->getActiveAnimation())
            {
                animation->stop();
            }
            resetTextureUV();
            return;
        }

        if (!_animationOverrideName.isEmpty())
        {
            animator->startAnimation(_animationOverrideName);
        }
        else if (!animator->getActiveAnimationName().isEmpty())
        {
            animator->startAnimation(animator->getActiveAnimationName());
        }
    }

    void RectangleAnimated::setAnimationOverride(const StringAtom& animationName, float fps)
    {
        _animationOverrideName = animationName;
        _animationOverrideFPS = fps;

        auto* animator = findFirstChildOf<Animation::FrameByFrameAnimator>();
        if (!animator || animationName.isEmpty() || fps <= 0.f)
        {
            return;
        }
        if (auto* animation = animator->getAnimation(animationName))
        {
            animation->setFPS(fps);
            if (_animationEnabled)
            {
                animator->startAnimation(animationName);
            }
        }
    }

    const StringAtom& RectangleAnimated::getAnimationOverrideName() const noexcept
    {
        return _animationOverrideName;
    }

    float RectangleAnimated::getAnimationOverrideFPS() const noexcept
    {
        return _animationOverrideFPS;
    }

    nlohmann::json RectangleAnimated::getTypeSpecificSceneDataAsJson() const
    {
        auto out = Rectangle::getTypeSpecificSceneDataAsJson();
        out["_animationEnabled"] = _animationEnabled;
        if (!_animationOverrideName.isEmpty() && _animationOverrideFPS > 0.f)
        {
            out["_animationName"] = _animationOverrideName;
            out["_animationFPS"] = _animationOverrideFPS;
        }
        return out;
    }

    void RectangleAnimated::applyTypeSpecificSceneData(const nlohmann::json& data)
    {
        Rectangle::applyTypeSpecificSceneData(data);

        _animationEnabled = data.value("_animationEnabled", true);

        if (data.contains("_animationName") && data.contains("_animationFPS"))
        {
            setAnimationOverride(data.at("_animationName").get<StringAtom>(),
                                 data.at("_animationFPS").get<float>());
        }

        setAnimationEnabled(_animationEnabled);
    }

} // namespace NX::SceneObj
