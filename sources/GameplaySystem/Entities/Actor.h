/*
 * MIT License
 *
 * Copyright (c) 2018-2026 Valerii Koniushenko
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

#include "ECS/BaseComponent.h"
#include "ECS/Transformable.h"
#include "Graphics/IDrawable.h"

namespace Core
{
    CLASS();
    class Actor : public Transformable, public IDrawable, public BaseComponent
    {
        R_FRIEND_DECL(Actor, Core::BaseComponent, Core::Transformable, Core::IDrawable);
        ECS_COMPONENT_DECL(Actor, BaseComponent);

    public:
        Actor(Actor&&) = default;
        Actor(const Actor& other) = default;
        Actor& operator=(Actor&&) = default;
        Actor& operator=(const Actor&) = default;
        ~Actor() override = default;

        friend void swap(Actor& a, Actor& b) noexcept
        {
            std::swap(static_cast<BaseComponent&>(a), static_cast<BaseComponent&>(b));
            std::swap(static_cast<Transformable&>(a), static_cast<Transformable&>(b));
        }

        void onPreDeserialize(AbstractComponent* obj) override;
        void onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs) override;
        void onPreSerialize(const AbstractComponent* obj) const override;
        void onPostSerialize(const AbstractComponent* obj,
                             const RLogsCollector& logs) const override;

    protected:
        void onInitialize() override;
    };

    template<class T>
    concept IsActorBased = std::is_base_of_v<Actor, T>;
} // namespace Core
#include "Actor.generated.h" // added by the code generator. Better don't move it.
