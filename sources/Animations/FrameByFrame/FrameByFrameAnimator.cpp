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
    void to_json(nlohmann::json& j,
                 const std::unordered_map<StringAtom, BaseAnimation::Ptr>& v)
    {
        j = nlohmann::json::object();

        for (const auto& [name, animation] : v)
        {
            j[name.c_str()] = animation;
        }
    }

    void from_json(const nlohmann::json& j,
                   std::unordered_map<StringAtom, BaseAnimation::Ptr>& v)
    {
        if (!j.is_object())
        {
            throw nlohmann::json::type_error::create(
                302, "Frame-by-frame animations must be represented by a JSON object", &j);
        }

        std::unordered_map<StringAtom, BaseAnimation::Ptr> animations;
        animations.reserve(j.size());

        for (const auto& [name, animationJson] : j.items())
        {
            if (!animationJson.contains("_type"))
            {
                continue;
            }

            const auto type
                = StringAtom::Intern(animationJson["_type"].get<StringAtom>());
            BaseComponent::Ptr component = GetGlobalComponentFactory().create(type);
            auto* animation = dynamic_cast<BaseAnimation*>(component.get());
            if (!animation)
            {
                continue;
            }

            RResourceStream<RJsonResourceStream> stream{ animationJson };
            animation->deserialize(stream);
            StringAtom animationName{ name.c_str() };
            animation->setComponentName(animationName);
            animations.emplace(std::move(animationName), animation);
        }

        v = std::move(animations);
    }

    ECS_IMPL(FrameByFrameAnimator);

    bool FrameByFrameAnimator::startAnimation(const StringAtom& name)
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

    bool FrameByFrameAnimator::addAnimation(FrameByFrameAnimation animation)
    {
        if (!animation.isReady())
        {
            return false;
        }

        const auto name = animation.getComponentName();
        auto storedAnimation = FrameByFrameAnimation::Create();
        *storedAnimation = std::move(animation);
        _animations.insert_or_assign(name, std::move(storedAnimation));
        return true;
    }

    bool FrameByFrameAnimator::removeAnimation(const StringAtom& name)
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

    bool FrameByFrameAnimator::containAnimation(const StringAtom& name) const
    {
        return _animations.contains(name);
    }

    void FrameByFrameAnimator::onTick(float delta)
    {
        BaseComponent::onTick(delta);
        updateCurrentAnimation(delta);
        applyCurrentFrameToRectangle();
    }

    void FrameByFrameAnimator::applyCurrentFrameToRectangle()
    {
        auto* rectangle = getParentAs<SceneObj::Rectangle>();
        const auto* animation = getActiveAnimation();
        if (!Verify(
                rectangle,
                ("Only '{}' type is supported for animations."_f
                 << R<SceneObj::Rectangle>::FullName())
                    .c_str())
            || !animation)
        {
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
            animation->update(delta);
        }
    }
} // namespace Core::Animation
