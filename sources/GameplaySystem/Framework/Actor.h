/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "../../RawDataManagement/DataStream.h"
#include "GameplaySystem/ECS/Transformable.h"
#include "Graphics/IDrawable.h"
#include "Graphics/Primitives/StaticMesh.h"
#include "WorldObject.h"

namespace Core
{
    class Actor : public Transformable, public IDrawable, public BaseComponent
    {
        ECS_COMPONENT_DECL(Actor, BaseComponent);

    public:
        Actor(Actor&&) = default;
        Actor(const Actor& other) = default;
        Actor& operator=(Actor&&) = default;
        Actor& operator=(const Actor&) = default;
        ~Actor() override = default;

        void ioFieldsUpdate(DataStream& out) override;

        friend void swap(Actor& a, Actor& b) noexcept
        {
            std::swap(static_cast<BaseComponent&>(a), static_cast<BaseComponent&>(b));
            std::swap(static_cast<Transformable&>(a), static_cast<Transformable&>(b));
        }

    protected:
        void onInitialize() override;
        [[nodiscard]] StringAtom getCacheHash() const override;
    };

    template<class T>
    concept IsActorBased = std::is_base_of_v<Actor, T>;
} // namespace Core