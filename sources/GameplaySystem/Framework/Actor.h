// MIT License
//
// Copyright (c) 2018-2025 Valerii Koniushenko
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
#include "Graphics/Primitives/StaticMesh.h"
#include "Misc/DataStream.h"

namespace Core
{
    class Actor : public Transformable, public BaseComponent
    {
        ECS_COMPONENT_DECL(Actor, BaseComponent);

    public:
        Actor(Actor&&) = default;
        Actor(const Actor& other)
            : BaseComponent(other._type, other._name)
        {
            *this = other;
        }
        Actor& operator=(Actor&&) = default;
        Actor& operator=(const Actor&) = default;
        ~Actor() override = default;

        /**
         * @brief will draw with the default shader & logic. Single draw bundle!
         */
        virtual void draw() {}

        /**
         * @brief only draw call with graphics modifiers - nothing more.
         * Before using of this function you must manually prepare the shader &
         * 'use' it.
         */
        virtual void pureDraw(const std::function<void(StaticMesh*)>& onUniformSet,
                              const std::function<bool(const Actor*)>& conditional)
        {
        }

        void setIsPostDraw(bool value) noexcept { _isPostDraw = value; }
        [[nodiscard]] bool isPostDraw() const noexcept { return _isPostDraw; }

        void ioFieldsUpdate(DataStream& stream) override;

    protected:
        void onInitialize() override;
        [[nodiscard]] StringAtom getCacheHash() const override;

    protected:
        bool _isPostDraw = false;
    };

    template<class T>
    concept IsActorBased = std::is_base_of_v<Actor, T>;
} // namespace Core