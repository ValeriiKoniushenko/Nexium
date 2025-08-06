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
    ECS_REGISTER_NEW_COMPONENT_TYPE(InvalidComponent);

    BaseComponent* GlobalComponentFactory::create(const Core::StringAtom& type)
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

    bool GlobalComponentFactory::registerNewType(Core::StringAtom type,
                                                 std::function<BaseComponent*()> callback)
    {
        Assert(type.isStatic());
        _map.emplace(std::move(type), std::move(callback));
        return true;
    }

    void AbstractComponent::tick()
    {
        if (_isEnabled && _isInited && !_noTick)
        {
            onTick();
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
        _isEnabled = json["isEnabled"];
        _noTick = json["noTick"].get<bool>();
    }

    bool BaseComponent::removeChild(const BaseComponent* child)
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

    bool BaseComponent::removeChildIf(std::function<bool(const BaseComponent*)>&& pred)
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

        json["name"] = _name;
        json["type"] = _type;

        for (auto& child : _children)
        {
            json["children"].push_back(child->toJson());
        }

        return json;
    }

    void BaseComponent::fromJson(const nlohmann::json& json, bool isIgnoreChildren /* = false*/)
    {
        AbstractComponent::fromJson(json, isIgnoreChildren);

        _name = json["name"];
        const_cast<Core::StringAtom&>(_type)
            = Core::StringAtom::Intern(json["type"].get<std::string>());

        if (!isIgnoreChildren)
        {
            if (json.contains("children"))
            {
                for (auto& childJson : json["children"])
                {
                    auto c = rawAddChildComponent(
                        GetGlobalComponentFactory().create(childJson["type"]));
                    c->fromJson(childJson);
                }
            }
        }
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

    void BaseComponent::onSuccessAddChildComponentValidation(BaseComponent* newComponent)
    {
        newComponent->_parent = this;
    }

} // namespace SW
