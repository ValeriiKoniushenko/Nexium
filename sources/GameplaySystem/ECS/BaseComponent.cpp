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

#include "BaseComponent.h"

#include "Editor/Gizmo.h"
#include "Editor/Windows/NxECSBasedEditor.h"

namespace
{
    template<typename T, typename... Rest>
    void hash_combine(std::size_t& seed, const T& v, const Rest&... rest)
    {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hash_combine(seed, rest), ...);
    }
} // namespace

namespace Core
{
    ECS_COMPONENT_IMPL(InvalidComponent);

    const StringAtom BaseComponent::componentType = "BaseComponent"_atom;

    BaseComponent* GlobalComponentFactory::create(const StringAtom& type)
    {
        if (const auto found = _map.find(type); found != _map.end()) [[likely]]
        {
            if (found->second) [[likely]]
            {
                return found->second();
            }
        }

        Assert(false,
               ("Can't reflect the type '{}'. Maybe you forgot to register your class with needed "
                "macros. Look at the documentation, or check commen above the class "
                "Core::BaseComponent "
                "to get more details."_f
                << type)
                   .data());
        return nullptr;
    }

    bool GlobalComponentFactory::registerNewType(const StringAtom& type,
                                                 std::function<BaseComponent*()> callback)
    {
        Assert(type.isStatic());

#if defined(DEBUG)
        if (_map.contains(type))
        {
            criticalLog("You're trying to register the type '{}' second(or more) time."_f << type);
        }
#endif
        _map.emplace(type, std::move(callback));
        traceLog("Type '{}' has been registered."_f << type);

        return true;
    }

    AbstractComponent::AbstractComponent(AbstractComponent&& other) noexcept
    {
        *this = std::move(other);
    }

    AbstractComponent& AbstractComponent::operator=(AbstractComponent&& other) noexcept
    {
        if (this != &other)
        {
            _isEnabled = other._isEnabled;
            _noTick = other._noTick;

            other._isEnabled = {};
            other._noTick = {};
        }

        return *this;
    }

    void AbstractComponent::tick(float delta)
    {
        if (_isEnabled && !_noTick)
        {
            onTick(delta);
        }
    }

    void AbstractComponent::ioFieldsUpdate(DataStream& out)
    {
        auto stream = out.dedicatedNesting("AbstractComponent");

        stream.field("isEnabled", _isEnabled);
        stream.field("noTick", _noTick);
    }

    BaseComponent* BaseComponent::attachChild(const BaseComponent::Ptr& child)
    {
        return rawAddChildComponent(child->clone().get());
    }

    BaseComponent* BaseComponent::attachUniqueChild(const BaseComponent::Ptr& child)
    {
        for (auto&& c : _children)
        {
            if (c.get() == child.get())
            {
                return c.get();
            }
        }

        return attachChild(child);
    }

    void BaseComponent::detachChild(BaseComponent* child)
    {
        if (const auto it = std::ranges::find(_children, child); it != _children.end())
        {
            _children.erase(it);
        }
    }

    void BaseComponent::removeChild(const BaseComponent* child)
    {
        const auto [first, last] = std::ranges::remove_if(_children,
                                                          [&child, this](const Ptr& c)
                                                          {
                                                              if (*c == *child)
                                                              {
                                                                  onRemoveChild(c.get());
                                                                  return true;
                                                              }
                                                              return false;
                                                          });
        _children.erase(first, last);
    }

    void BaseComponent::removeChildIf(const std::function<bool(const BaseComponent*)>& pred)
    {
        if (!pred)
        {
            return;
        }

        const auto [first, last] = std::ranges::remove_if(_children,
                                                          [&pred, this](const Ptr& c)
                                                          {
                                                              if (pred(c.get()))
                                                              {
                                                                  onRemoveChild(c.get());
                                                                  return true;
                                                              }
                                                              return false;
                                                          });
        _children.erase(first, last);
    }

    BaseComponent* BaseComponent::getOwner() noexcept
    {
        BaseComponent* comp = this;

        while (comp)
        {
            if (comp->hasParent())
            {
                comp = comp->getParent();
            }
            else
            {
                break;
            }
        }

        return comp;
    }

    void BaseComponent::clear()
    {
        AbstractComponent::clear();

        _name.clear();
        _parent = nullptr;
        _children.clear();
    }

    void BaseComponent::onTick(float delta)
    {
        for (auto&& child : _children)
        {
            child->tick(delta);
        }
    }

    void BaseComponent::ioFieldsUpdate(DataStream& out)
    {
        AbstractComponent::ioFieldsUpdate(out);

        auto stream = out.dedicatedNesting("BaseComponent");

        stream.field("name", _name, _type);
        stream.field("type", _type);
        stream.array("children",
                     [this](DataStream& out, std::size_t size)
                     {
                         if (out.getMode() == DataStream::Mode::Output)
                         {
                             for (auto& child : _children)
                             {
                                 out.tryPushBackEmptyArrayElement();
                                 out.field(*child);
                             }
                         }
                         else
                         {
                             for (auto&& child : out.getRaw().items())
                             {
                                 // std::cout << child.value().dump(4) << std::endl;
                                 auto&& in = child.value();

                                 if (!in.contains("BaseComponent"))
                                 {
                                     warnLog(
                                         "No root key 'BaseComponent' for child component. "
                                         "Impossible to fetch type data.");
                                     Assert(false);
                                     continue;
                                 }

                                 if (!in["BaseComponent"].contains("type"))
                                 {
                                     warnLog("No type specified for child component");
                                     Assert(false);
                                     continue;
                                 }

                                 auto&& last
                                     = _children.emplace_back(GetGlobalComponentFactory().create(
                                         in["BaseComponent"]["type"].get<StringAtom>()));
                                 last->_parent = this;

                                 DataStream childStream;
                                 childStream.setMode(DataStream::Mode::Input);
                                 childStream.getRaw() = in;
                                 last->ioFieldsUpdate(childStream);
                             }
                         }
                     });
    }

    void BaseComponent::initialize()
    {
        if (!_isInitialized)
        {
            onPreInitialize();
            _isInitialized = true;
            onInitialize();

            for (auto&& child : _children)
            {
                child->initialize();
            }
        }
    }

    BaseComponent::BaseComponent(BaseComponent&& other) noexcept
    {
        *this = std::move(other);
    }

    BaseComponent& BaseComponent::operator=(BaseComponent&& other) noexcept
    {
        if (this != &other)
        {
            AbstractComponent::operator=(std::move(other));
            _name = std::move(other._name);
            _children = std::move(other._children);
            _type = std::move(other._type);
            _parent = other._parent;
            _isInitialized = other._isInitialized;

            other._parent = nullptr;
            other._isInitialized = false;

            // resetting to new parent because old one will be invalid
            for (const auto& child : _children)
            {
                child->_parent = this;
            }
        }

        return *this;
    }

    bool BaseComponent::operator==(const BaseComponent& other) const
    {
        Assert(!_name.isEmpty());
        Assert(!other._name.isEmpty());

        return _name == other._name && _type == other._type;
    }

    void BaseComponent::setComponentName(const StringAtom& name)
    {
        _name = name;
        _name.shrinkToFit();
    }

    void BaseComponent::setComponentName(StringAtom&& name)
    {
        _name = std::move(name);
        _name.shrinkToFit();
    }

    bool BaseComponent::isValid() const
    {
        return !_name.isEmpty();
    }

    std::size_t BaseComponent::makeHash() const
    {
        std::size_t seed = 0;

        const auto* i = this;
        while (i)
        {
            hash_combine(seed, i->_name);
            i = dynamic_cast<const BaseComponent*>(i->_parent);
        }

        return seed;
    }

    StringAtom BaseComponent::getCacheHash() const
    {
        return _type + _name;
    }

    BaseComponent* BaseComponent::rawAddChildComponent(BaseComponent* newOne)
    {
        if (!newOne)
        {
            return nullptr;
        }

        if (!addChildValidator(newOne))
        {
            return nullptr;
        }

        // if this parent wasn't init, lets init at least here
        if (!isInitialized())
        {
            initialize();
        }
        auto* added = _children.emplace_back(newOne).get();
        added->_parent = this;
        added->initialize();

        onAddChild(added);

        return added;
    }

    BaseComponent::BaseComponent(const BaseComponent& other)
        : AbstractComponent(other)
    {
        *this = other;
    }

    BaseComponent& BaseComponent::operator=(const BaseComponent& other)
    {
        if (&other != this)
        {
            AbstractComponent::operator=(other);
            _name = other._name;
            _type = other._type;

            _children.clear();
            for (const auto& child : other._children)
            {
                _children.emplace_back(child->clone());
                _children.back()->_parent = this;
            }
        }

        return *this;
    }
} // namespace Core