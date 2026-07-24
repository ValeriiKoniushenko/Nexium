/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "ModuleInfo.h"

#include <set>

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

    ECS_IMPL(InvalidComponent);

    nlohmann::json AbstractComponent::serialize() const
    {
        return R<AbstractComponent>::Serialize(*this).getData();
    }

    void AbstractComponent::deserialize(RResourceStream<RJsonResourceStream>& stream)
    {
        R<AbstractComponent>::Deserialize(stream, *this);
    }

    void AbstractComponent::onPreDeserialize(AbstractComponent* obj)
    {
    }

    void AbstractComponent::onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs)
    {
    }

    void AbstractComponent::onPreSerialize(const AbstractComponent* obj) const
    {
    }

    void AbstractComponent::onPostSerialize(const AbstractComponent* obj,
                                            const RLogsCollector& logs) const
    {
    }

    const StringAtom BaseComponent::componentType = "BaseComponent"_atom;

    GlobalComponentFactory::~GlobalComponentFactory()
    {
#if defined(DEBUG)
        if (_startRegTime)
        {
            std::cout << "[Debug] GlobalComponentFactory: type registration took: "
                      << std::chrono::duration<float>(_endRegTime - *_startRegTime).count()
                      << std::endl;
        }
#endif
    }

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

    spdlog::logger* GlobalComponentFactory::getLogger() const
    {
        return Ecs::getLogger();
    }

    std::vector<StringAtom> GlobalComponentFactory::getRegisteredTypesAsVector(
        bool sort /*= false*/) const
    {
        std::vector<StringAtom> out;
        out.reserve(_map.size());
        for (const auto& [type, _] : _map)
        {
            out.emplace_back(type);
        }

        if (sort)
        {
            std::ranges::sort(out);
        }

        return out;
    }

    bool GlobalComponentFactory::containsSuchType(const StringAtom& type) const
    {
        return _map.contains(type);
    }

    std::optional<std::type_index> GlobalComponentFactory::getTypeIdByTypeName(
        const StringAtom& type)
    {
        Assert(type.isStatic());

        if (const auto it = _typeToNameMap.find(type); it != _typeToNameMap.end())
        {
            return it->second;
        }

        criticalLog("Can't find typeid by typename '{}' inside type-map"_f << type);

        static std::once_flag onceFlag;
        std::call_once(onceFlag,
                       [this]()
                       {
                           std::set<StringAtom> sorted;
                           for (auto regType : _typeToNameMap | std::views::keys)
                           {
                               // Limit super-long type names
                               if (regType.size() > 32)
                               {
                                   regType.resize(30);
                                   regType += "..";
                               }
                               sorted.emplace(std::move(regType));
                           }

                           std::string out;
                           out.reserve(1024);
                           out = "Registered types: ";
                           for (auto&& s : sorted)
                           {
                               out += s;
                               out += ", ";
                           }
                           out.pop_back();
                           out.pop_back();

                           infoLog(out);
                       });

        return std::nullopt;
    }

    void GlobalComponentFactory::_createTypeToTagMap()
    {
        for (auto&& [typeName, creator] : _map)
        {
            auto tmp = creator();
            if (Verify(tmp))
            {
                _typeToTagMap.emplace(typeName, tmp->getTags());
            }
        }
    }

    AbstractComponent::AbstractComponent(AbstractComponent&& other) noexcept
        : _isEnabled(other._isEnabled),
          _noTick(other._noTick)
    {
        other._isEnabled = other._noTick = {};
    }

    AbstractComponent& AbstractComponent::operator=(AbstractComponent&& other) noexcept
    {
        if (this != &other) [[likely]]
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

    spdlog::logger* AbstractComponent::getLogger() const
    {
        return Ecs::getLogger();
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
        if (const auto it = std::ranges::find(_children, IntrusivePtr(child));
            it != _children.end())
        {
            _children.erase(it);
        }
    }

    void BaseComponent::removeChild(const BaseComponent* child)
    {
        for (auto i = _children.begin(); i != _children.end(); ++i)
        {
            if (i->get() == child)
            {
                onRemoveChild(i->get());
                _children.erase(i);
                return;
            }
        }
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

    nlohmann::json BaseComponent::serialize() const
    {
        return R<BaseComponent>::Serialize(*this).getData();
    }

    void BaseComponent::deserialize(RResourceStream<RJsonResourceStream>& stream)
    {
        R<BaseComponent>::Deserialize(stream, *this);
    }

    void BaseComponent::onPreDeserialize(AbstractComponent* obj)
    {
        AbstractComponent::onPreDeserialize(obj);
    }

    void BaseComponent::onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs)
    {
        AbstractComponent::onPostDeserialize(obj, logs);
        if (!_type.isEmpty()) [[likely]]
        {
            _type = StringAtom::Intern(_type);
        }

        for (auto&& child : _children)
        {
            if (!child->_parent)
            {
                child->_parent = this;
            }
        }
    }

    void BaseComponent::onPreSerialize(const AbstractComponent* obj) const
    {
        AbstractComponent::onPreSerialize(obj);
    }

    void BaseComponent::onPostSerialize(const AbstractComponent* obj,
                                        const RLogsCollector& logs) const
    {
        AbstractComponent::onPostSerialize(obj, logs);
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

    bool BaseComponent::isDescendantOf(const BaseComponent* p) const noexcept
    {
        const auto* i = _parent;
        while (i)
        {
            if (i == p)
            {
                return true;
            }

            i = _parent->_parent;
        }
        return false;
    }

    bool BaseComponent::IsSelfOrDescendantOf(const BaseComponent* p) const noexcept
    {
        return this == p || isDescendantOf(p);
    }

    void BaseComponent::onTick(float delta)
    {
        for (auto&& child : _children)
        {
            child->tick(delta);
        }
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
        : AbstractComponent(std::move(other)),
          _children(std::move(other._children)),
          _name(std::move(other._name)),
          _type(std::move(other._type)),
          _parent(other._parent),
          _isInitialized(other._isInitialized)
    {
        Assert(_type.isStatic());

        other._parent = nullptr;
        other._isInitialized = false;

        for (auto& child : _children)
        {
            child->_parent = this;
        }
    }

    BaseComponent& BaseComponent::operator=(BaseComponent&& other) noexcept
    {
        if (this == &other) [[unlikely]]
        {
            return *this;
        }

        BaseComponent tmp(std::move(other));
        swap(*this, tmp);
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
        : AbstractComponent(other),
          _name(other._name),
          _type(other._type)
    {
        Assert(_type.isStatic());
        Assert(other._type.isStatic());

        _children.reserve(other._children.size());

        for (const auto& child : other._children)
        {
            _children.emplace_back(child->clone())->_parent = this;
        }
    }

    BaseComponent& BaseComponent::operator=(const BaseComponent& other)
    {
        if (this == &other)
        {
            return *this;
        }

        BaseComponent tmp(other);
        swap(*this, tmp);
        return *this;
    }

} // namespace Core