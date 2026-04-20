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

#include "Actor.h"

namespace Core
{
    ECS_COMPONENT_IMPL(Actor);
    R_FRIEND_IMPL(Actor);

    void Actor::onInitialize()
    {
        BaseComponent::onInitialize();

        if (_name.isEmpty())
        {
            _name = _type;
        }
    }

    void Actor::onPreDeserialize(AbstractComponent* obj)
    {
        BaseComponent::onPreDeserialize(obj);
        Transformable::onPreDeserialize(this);
    }

    void Actor::onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs)
    {
        BaseComponent::onPostDeserialize(obj, logs);
        Transformable::onPostDeserialize(this, logs);
    }

    void Actor::onPreSerialize(const AbstractComponent* obj) const
    {
        BaseComponent::onPreSerialize(obj);
        Transformable::onPreSerialize(this);
    }

    void Actor::onPostSerialize(const AbstractComponent* obj, const RLogsCollector& logs) const
    {
        BaseComponent::onPostSerialize(obj, logs);
        Transformable::onPostSerialize(this, logs);
    }

} // namespace Core
