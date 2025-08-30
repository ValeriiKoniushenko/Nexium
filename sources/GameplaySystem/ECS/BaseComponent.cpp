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

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(InvalidComponent);

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
               "Maybe you forget to register your own component with ECS_REGISTER_NEW_COMPONENT?");
        return nullptr;
    }

    bool GlobalComponentFactory::registerNewType(StringAtom type,
                                                 std::function<BaseComponent*()> callback)
    {
        Assert(type.isStatic());
#if defined(DEBUG)
        _debugTypeTracker_NotifyNewAboutType(type);
#endif
        _map.emplace(std::move(type), std::move(callback));
        return true;
    }

    void GlobalComponentFactory::_debugTypeTracker_NotifyNewAboutType(const StringAtom& newType)
    {
#if defined(DEBUG)
        // Should go here from ECS_REGISTER_NEW_COMPONENT[_TYPE]
        if (_debugTypeTracker.contains(newType))
        {
            traceLog("The type '{}' was implemented."_f << newType);

            // 2 - because we should hit it from both macroses:
            // - ECS_REGISTER_NEW_COMPONENT
            // - ECS_REGISTER_NEW_COMPONENT_TYPE
            if (++_debugTypeTracker[newType] > 2)
            {
                warnLog("The type '{}' was implemented more times then needed."_f << newType);
            }
        }
        else
        {
            traceLog("The type '{}' was declared."_f << newType);
            _debugTypeTracker[newType] = 1;
        }
#endif
    }

    GlobalComponentFactory::~GlobalComponentFactory()
    {
        // Debug type checker
#if defined(DEBUG)
        for (auto [type, counter] : _debugTypeTracker)
        {
            // 2 - because we should hit it from both macroses:
            // - ECS_REGISTER_NEW_COMPONENT
            // - ECS_REGISTER_NEW_COMPONENT_TYPE
            constexpr uint32_t hitCount = 2;

            if (counter < hitCount)
            {
                Assert(
                    false,
                    ("The type '{}' wasn't fully registered. Try to find where this type was declared/"
                     "created and add 'ECS_REGISTER_NEW_COMPONENT' to the classe's body, and "
                     "'ECS_REGISTER_NEW_COMPONENT_TYPE' to its implementation(.cpp)"
                     " Also, you can enable(if it's disabled) spdlog::level::trace and check what "
                     "type was declared and what type was implemented"_f
                     << type)
                        .data());
            }
            if (counter > hitCount)
            {
                Assert(
                    false,
                    ("New unknown component wasn't registered. Somewhere you inherited with new type"
                     " from base type '{}'. If you inherit from the component you must register your"
                     " new type with 'ECS_REGISTER_NEW_COMPONENT' inside classe's body, and "
                     "register a type with 'ECS_REGISTER_NEW_COMPONENT_TYPE' inside .cpp file."
                     " Also, you can enable(if it's disabled) spdlog::level::trace and check what "
                     "type was declared and what type was implemented"_f
                     << type)
                        .data());
            }
        }
#endif
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
            _isInited = other._isInited;
            _noTick = other._noTick;

            other._isEnabled = {};
            other._isInited = {};
            other._noTick = {};
        }

        return *this;
    }

    void AbstractComponent::tick(float delta)
    {
        if (_isEnabled && _isInited && !_noTick)
        {
            onTick(delta);
        }
    }

    nlohmann::json AbstractComponent::toJson() const
    {
        nlohmann::json json;

        json["isEnabled"] = _isEnabled;
        json["noTick"] = _noTick;

        return json;
    }

    void AbstractComponent::fromJson(const nlohmann::json& json, bool isIgnoreChildren /* = false*/)
    {
        if (json.contains("isEnabled"))
        {
            _isEnabled = json["isEnabled"];
        }
        if (json.contains("noTick"))
        {
            _noTick = json["noTick"].get<bool>();
        }
    }

    void BaseComponent::attachChild(BaseComponent* child)
    {
        _children.push_back(child);
    }

    void BaseComponent::detachChild(BaseComponent* child)
    {
        if (auto it = std::ranges::find(_children, child); it != _children.end())
        {
            _children.erase(it);
        }
    }

    void BaseComponent::removeChild(const BaseComponent* child)
    {
        const auto [first, last] = std::ranges::remove_if(_children,
                                                          [&child](const BaseComponent::Ptr& c)
                                                          {
                                                              return *c == *child;
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
                                                          [&pred](const Ptr& c)
                                                          {
                                                              return pred(c.get());
                                                          });
        _children.erase(first, last);
    }

    void BaseComponent::clear()
    {
        AbstractComponent::clear();
        _name.clear();
        _parent = nullptr;
        _children.clear();
    }

    nlohmann::json BaseComponent::toJson() const
    {
        auto json = AbstractComponent::toJson();

        static int64_t idGen = 0;

        if (_name.isEmpty())
        {
            json["name"] = _type;
        }
        else
        {
            json["name"] = _name;
        }
        json["type"] = _type;

        for (auto& child : _children)
        {
            json["children"].push_back(child->toJson());
        }

        return json;
    }

    void BaseComponent::fromJson(const nlohmann::json& json, bool isIgnoreChildren)
    {
        AbstractComponent::fromJson(json, isIgnoreChildren);

        _name = requireAs<StringAtom>(json, "name");
        _type = StringAtom::Intern(requireAs<StringAtom>(json, "type"));

        if (!isIgnoreChildren)
        {
            if (json.contains("children"))
            {
                for (auto& childJson : json["children"])
                {
                    auto type = StringAtom::Intern(requireAs<StringAtom>(childJson, "type"));
                    auto c = rawAddChildComponent(GetGlobalComponentFactory().create(type));
                    c->fromJson(childJson, false);
                }
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

            other._parent = nullptr;

            // resetting to new parent because old one will be invalid
            for (auto& child : _children)
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

    void BaseComponent::onSuccessAddChildComponentValidation(BaseComponent* newComponent)
    {
        newComponent->_parent = this;
    }

    BaseComponent* BaseComponent::rawAddChildComponent(BaseComponent* newOne)
    {
        if (!onAddChildComponentValidation(newOne))
        {
            return nullptr;
        }

        onSuccessAddChildComponentValidation(newOne);

        // if this parent wasn't init, lets init at least here
        if (!isInited())
        {
            initialize();
        }
        auto* added = _children.emplace_back(newOne).get();
        added->initialize();
        return added;
    }

    BaseComponent::BaseComponent(BaseComponent& other)
    {
        *this = other;
    }

    BaseComponent& BaseComponent::operator=(const BaseComponent& other)
    {
        if (&other != this)
        {
            _name = other._name;
            _type = other._type;

            _children.clear();
            for (auto& child : other._children)
            {
                _children.emplace_back(new BaseComponent(*child));
                _children.back()->_parent = this;
            }
        }

        return *this;
    }

} // namespace Core
