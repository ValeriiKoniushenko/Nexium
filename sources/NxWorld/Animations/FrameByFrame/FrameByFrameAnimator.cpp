// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "FrameByFrameAnimator.h"

#include "FrameByFrameAnimation.h"
#include "Scene/Rectangle.h"

namespace NX::Animation
{
    void to_json(nlohmann::json& j,
                 const std::unordered_map<Core::StringAtom, BaseAnimation::Ptr>& v)
    {
        j = nlohmann::json::object();

        for (const auto& [name, animation] : v)
        {
            if (animation)
            {
                j[name.c_str()] = animation->serialize();
            }
        }
    }

    void from_json(const nlohmann::json& j,
                   std::unordered_map<Core::StringAtom, BaseAnimation::Ptr>& v)
    {
        if (!j.is_object() && !j.is_array())
        {
            throw nlohmann::json::type_error::create(
                302, "Animations must be represented by a JSON array or object", &j);
        }

        std::unordered_map<Core::StringAtom, BaseAnimation::Ptr> animations;
        animations.reserve(j.size());

        const auto deserializeAnimation
            = [&animations](const nlohmann::json& animationJson,
                            const Core::StringAtom& fallbackName = Core::StringAtom{})
        {
            if (!animationJson.contains("_type"))
            {
                return;
            }

            const auto type
                = Core::StringAtom::Intern(animationJson["_type"].get<Core::StringAtom>());
            BaseComponent::Ptr component = GetGlobalComponentFactory().create(type);
            auto* animation = dynamic_cast<BaseAnimation*>(component.get());
            if (!animation)
            {
                return;
            }

            RResourceStream<RJsonResourceStream> stream{ animationJson };
            animation->deserialize(stream);
            if (animation->getComponentName().isEmpty())
            {
                animation->setComponentName(fallbackName);
            }
            if (!animation->getComponentName().isEmpty())
            {
                animations.insert_or_assign(animation->getComponentName(), animation);
            }
        };

        if (j.is_array())
        {
            for (const auto& animationJson : j)
            {
                deserializeAnimation(animationJson);
            }
        }
        else
        {
            for (const auto& [name, animationJson] : j.items())
            {
                deserializeAnimation(animationJson, Core::StringAtom{ name.c_str() });
            }
        }

        v = std::move(animations);
    }

    ECS_IMPL(FrameByFrameAnimator);

    bool FrameByFrameAnimator::startAnimation(const Core::StringAtom& name)
    {
        auto* animation = getAnimation(name);
        if (!animation)
        {
            return false;
        }

        _currentAnimationName = name;
        animation->restart();
        applyCurrentFrameToRectangle();
        return true;
    }

    bool FrameByFrameAnimator::addAnimation(const FrameByFrameAnimation& animation)
    {
        if (!animation.isValid())
        {
            return false;
        }

        const auto name = animation.getComponentName();
        auto storedAnimation = FrameByFrameAnimation::Create();
        *storedAnimation = animation;
        return _animations.insert_or_assign(name, std::move(storedAnimation)).second;
    }

    bool FrameByFrameAnimator::removeAnimation(const Core::StringAtom& name)
    {
        if (_currentAnimationName == name)
        {
            _currentAnimationName.clear();
        }
        return _animations.erase(name) > 0;
    }

    void FrameByFrameAnimator::clearAnimations()
    {
        _currentAnimationName.clear();
        _animations.clear();
    }
    FrameByFrameAnimation* FrameByFrameAnimator::getAnimation(const Core::StringAtom& name)
    {
        const auto it = _animations.find(name);
        if (it == _animations.end()) [[unlikely]]
        {
            return nullptr;
        }
        return dynamic_cast<FrameByFrameAnimation*>(it->second.get());
    }

    const FrameByFrameAnimation* FrameByFrameAnimator::getAnimation(
        const Core::StringAtom& name) const
    {
        const auto it = _animations.find(name);
        if (it == _animations.end())
        {
            return nullptr;
        }
        return dynamic_cast<const FrameByFrameAnimation*>(it->second.get());
    }

    FrameByFrameAnimation* FrameByFrameAnimator::getActiveAnimation()
    {
        return getAnimation(_currentAnimationName);
    }

    const FrameByFrameAnimation* FrameByFrameAnimator::getActiveAnimation() const
    {
        return getAnimation(_currentAnimationName);
    }

    bool FrameByFrameAnimator::containAnimation(const Core::StringAtom& name) const
    {
        return _animations.contains(name);
    }

    Tag FrameByFrameAnimator::getTags() const
    {
        return BaseComponent::getTags() | Tag_AnimationController;
    }

    void FrameByFrameAnimator::onTick(float delta)
    {
        BaseComponent::onTick(delta);
        updateCurrentAnimation(delta);
        applyCurrentFrameToRectangle();
    }

    void FrameByFrameAnimator::applyCurrentFrameToRectangle()
    {
        auto* rectangle = getParentAs<SceneObj::RectangleAnimated>();
        const auto* animation = getActiveAnimation();
        if (!rectangle || !animation)
        {
            LOG_ASSERT_ERROR_ONCE(("Only '{}' type is supported for animations."_f
                                   << R<SceneObj::RectangleAnimated>::FullName())
                                      .c_str());
            return;
        }

        const auto* frame = animation->getCurrentFrame();
        if (!frame)
        {
            return;
        }

        rectangle->setAtlas(animation->getAtlasName());

        const auto& textureName
            = frame->textureName ? *frame->textureName : animation->getTextureName();
        rectangle->setTexture(textureName);
        rectangle->setTextureUV(frame->uvOffset, frame->uvSize);
    }

    void FrameByFrameAnimator::updateCurrentAnimation(float delta)
    {
        if (auto* animation = getActiveAnimation())
        {
            animation->tick(delta);
        }
    }
} // namespace NX::Animation
