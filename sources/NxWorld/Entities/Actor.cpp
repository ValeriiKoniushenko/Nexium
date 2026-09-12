// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Actor.h"

namespace Core
{
    ECS_IMPL(Actor);

    void Actor::onInitialize()
    {
        SceneObject::onInitialize();

        if (_name.isEmpty())
        {
            _name = _type;
        }
    }

    void Actor::onPreDeserialize(AbstractComponent* obj)
    {
        SceneObject::onPreDeserialize(obj);
    }

    void Actor::onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs)
    {
        SceneObject::onPostDeserialize(obj, logs);
    }

    void Actor::onPreSerialize(const AbstractComponent* obj) const
    {
        SceneObject::onPreSerialize(obj);
    }

    void Actor::onPostSerialize(const AbstractComponent* obj, const RLogsCollector& logs) const
    {
        SceneObject::onPostSerialize(obj, logs);
    }

} // namespace Core
