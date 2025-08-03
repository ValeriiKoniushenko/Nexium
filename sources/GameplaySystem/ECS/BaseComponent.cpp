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

#include "BaseComponent.h"

namespace
{
    template<typename T, typename... Rest>
    void hash_combine(std::size_t& seed, const T& v, const Rest&... rest)
    {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hash_combine(seed, rest), ...);
    }

} // namespace

namespace SW
{

    void AbstractComponent::tick()
    {
        if (_isEnabled && _isInited)
        {
            onTick();
        }
    }

    bool ComponentHolder::removeChild(const BaseComponent* child)
    {
        for (auto i = _children.begin(); i != _children.end(); ++i)
        {
            if (i->get() == child)
            {
                _children.erase(i);
                return true;
            }
        }

        return false;
    }

    bool ComponentHolder::removeChildIf(std::function<bool(const BaseComponent*)>&& pred)
    {
        bool removedAtLeastOne = false;

        for (auto i = _children.begin(); i != _children.end();)
        {
            if (pred(i->get()))
            {
                i = _children.erase(i);
                removedAtLeastOne = true;
            }
            else
            {
                ++i;
            }
        }

        return removedAtLeastOne;
    }
    void ComponentHolder::clear()
    {
        AbstractComponent::clear();

        _children.clear();
    }

    bool BaseComponent::operator==(const BaseComponent& other) const
    {
        Assert(!_name.isEmpty());
        Assert(!other._name.isEmpty());

        return _name == other._name;
    }

    void BaseComponent::setComponentName(const Core::StringAtom& name)
    {
        if (name.isEmpty()) [[unlikely]]
        {
            Assert(false);
            errorLog("Was passed empty name to the component.");
            return;
        }

        _name = name;
        _name.shrink_to_fit();
    }

    bool BaseComponent::isValid() const
    {
        return !_name.isEmpty();
    }

    std::size_t BaseComponent::makeHash() const
    {
        std::size_t seed = 0;

        auto* i = this;
        while (i)
        {
            hash_combine(seed, i->_name);
            i = dynamic_cast<const BaseComponent*>(i->_parent);
        }

        return seed;
    }

    void BaseComponent::clear()
    {
        ComponentHolder::clear();

        _name.clear();
        _parent = nullptr;
    }

    void BaseComponent::onSuccessAddChildComponentValidation(BaseComponent* newComponent)
    {
        ComponentHolder::onSuccessAddChildComponentValidation(newComponent);

        newComponent->_parent = this;
    }

} // namespace SW
