// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "NxFundamental/ECS/BaseComponent.h"
#include "RawBackend/IDrawable.h"
#include "Scene/SceneObject.h"

namespace Core
{
    CLASS();
    class Actor : public SceneObject
    {
        ECS_DECL(Actor, Core::SceneObject);

    public:
        Actor(Actor&&) = default;
        Actor(const Actor& other) = default;
        Actor& operator=(Actor&&) = default;
        Actor& operator=(const Actor&) = default;
        ~Actor() override = default;

        friend void swap(Actor& a, Actor& b) noexcept
        {
            using std::swap;
            swap(static_cast<SceneObject&>(a), static_cast<SceneObject&>(b));
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
