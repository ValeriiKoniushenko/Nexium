// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "GameplaySystem/ECS/BaseComponent.h"
#include "GameplaySystem/ECS/Transformable.h"
#include "Misc/JsonCacheable.h"

namespace Core
{

    class Actor : public Transformable, public BaseComponent, public JsonCacheable
    {
        ECS_REGISTER_NEW_COMPONENT(Actor, BaseComponent);

    public:
        Actor(Actor&&) = default;
        Actor(const Actor& other)
            : BaseComponent(other._type, other._name)
        {
        }
        Actor& operator=(Actor&&) = default;
        Actor& operator=(const Actor&) = default;

        ~Actor() override = default;

        [[nodiscard]] nlohmann::json toJson() const override;
        void fromJson(const nlohmann::json& json, bool isIgnoreChildren) override;

        /**
         * @brief will draw with default shader & logic. Single draw bundle!
         */
        virtual void draw() {};

        /**
         * @brief only draw call with graphics modifiers - nothing more.
         * Before using of this function you must manually prepare the shader &
         * 'use' it.
         */
        virtual void pureDraw() {};

    protected:
        void onInit() override;
    };

    template<class T>
    concept IsActorBased = std::is_base_of_v<Actor, T>;

} // namespace Core
