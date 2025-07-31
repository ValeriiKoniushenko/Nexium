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

#include "BaseComponent.h"
#include "Misc/BaseLog.h"
#include "ModuleInfo.h"
#include "boost/intrusive_ptr.hpp"
#include "boost/smart_ptr/intrusive_ref_counter.hpp"

#include <list>

#define ECS_REGISTER_NEW_COMPONENT(CurrentClass, BaseComponentClass)                               \
public:                                                                                            \
    using Self = CurrentClass;                                                                     \
    using Ptr = boost::intrusive_ptr<CurrentClass>;                                                \
    using CPtr = boost::intrusive_ptr<const CurrentClass>;                                         \
    inline static const auto componentType = Core::StringAtom::Intern(#CurrentClass);              \
    [[nodiscard]] static const Core::StringAtom& GetComponentType()                                \
    {                                                                                              \
        return CurrentClass::componentType;                                                        \
    }                                                                                              \
    CurrentClass(const Core::StringAtom& name = ""_atom)                                           \
        : BaseComponentClass(&componentType, name)                                                 \
    {                                                                                              \
    }

#define ECS_REGISTER_NEW_COMPONENT_PROTECTED(CurrentClass, BaseComponentClass)                     \
public:                                                                                            \
    using Self = CurrentClass;                                                                     \
    template<bool isConst>                                                                         \
    using AdaptivePtr                                                                              \
        = boost::intrusive_ptr<std::conditional_t<isConst, const typename Self, typename Self>>;   \
    template<bool isConst>                                                                         \
    using AdaptiveRawPtr = std::conditional_t<isConst, const typename Self, typename Self>*;       \
    using Ptr = boost::intrusive_ptr<CurrentClass>;                                                \
    using CPtr = boost::intrusive_ptr<const CurrentClass>;                                         \
    inline static const auto componentType = Core::StringAtom::Intern(#CurrentClass);              \
    [[nodiscard]] const Core::StringAtom& GetComponentType()                                       \
    {                                                                                              \
        return CurrentClass::componentType;                                                        \
    }                                                                                              \
                                                                                                   \
protected:                                                                                         \
    CurrentClass(const Core::StringAtom& name = ""_atom)                                           \
        : BaseComponentClass(&componentType, name)                                                 \
    {                                                                                              \
    }                                                                                              \
                                                                                                   \
public:

namespace SW
{

    class BaseComponent;

    template<class T>
    concept IsComponent = std::derived_from<T, BaseComponent> && requires(T t) {
        { t.getComponentType() };
        { T::componentType };
    };

    template<class T>
    concept IsComponentOrBase = IsComponent<T> || std::is_same_v<BaseComponent, T>;

    template<class T>
    concept IsComponentOrVoid = IsComponent<T> || std::is_void_v<T>;

    class AbstractComponent : public BaseLog, public boost::intrusive_ref_counter<BaseComponent>
    {
    public:
        AbstractComponent(const AbstractComponent&) = default;
        AbstractComponent(AbstractComponent&&) noexcept = default;
        AbstractComponent& operator=(const AbstractComponent&) = default;
        AbstractComponent& operator=(AbstractComponent&&) noexcept = default;

        ~AbstractComponent() override = default;

        [[nodiscard]] spdlog::logger* getLogger() const override final { return Ecs::getLogger(); }
        [[nodiscard]] const char* getPrefix() const override { return "Component"; }

        virtual void clear() {}

        template<IsComponent T>
        [[nodiscard]] T* castTo()
        {
            auto* casted = dynamic_cast<T*>(this);
            Assert(casted);
            return casted;
        }

    protected:
        AbstractComponent() = default;
    };

    class ComponentHolder : public AbstractComponent
    {
    public:
        using Self = BaseComponent;
        using Ptr = boost::intrusive_ptr<BaseComponent>;
        using CPtr = boost::intrusive_ptr<const BaseComponent>;

    public:
        [[nodiscard]] const std::list<Ptr>& getChildren() const noexcept { return _children; }
        [[nodiscard]] std::size_t getChildrenSize() const noexcept { return _children.size(); }
        [[nodiscard]] bool hasChildren() const noexcept { return !_children.empty(); }

        template<IsComponent ComponentT>
        [[nodiscard]] ComponentT* addChildComponent()
        {
            typename ComponentT::Ptr newOne = new ComponentT;
            if (!onAddChildComponentValidation(newOne.get()))
            {
                return nullptr;
            }

            onSuccessAddChildComponentValidation(newOne.get());
            return static_cast<ComponentT*>(_children.emplace_back(std::move(newOne)).get());
        }

        bool removeChild(const BaseComponent* child);
        bool removeChildIf(std::function<bool(const BaseComponent*)>&& pred);

        void clear() override;

    protected:
        [[nodiscard]] virtual bool onAddChildComponentValidation(const BaseComponent* newComponent)
        {
            return true;
        }
        virtual void onSuccessAddChildComponentValidation(BaseComponent* newComponent) {}

    protected:
        std::list<Ptr> _children;
    };

    class BaseComponent : public ComponentHolder
    {
    public:
        using Self = BaseComponent;
        template<bool isConst>
        using AdaptivePtr
            = boost::intrusive_ptr<std::conditional_t<isConst, const BaseComponent, BaseComponent>>;
        template<bool isConst>
        using AdaptiveRawPtr = std::conditional_t<isConst, const BaseComponent, BaseComponent>*;
        using Ptr = boost::intrusive_ptr<BaseComponent>;
        using CPtr = boost::intrusive_ptr<const BaseComponent>;

    public:
        ~BaseComponent() override = default;

        [[nodiscard]] bool operator==(const BaseComponent& other) const;

        void setComponentName(const Core::StringAtom& name);
        [[nodiscard]] const Core::StringAtom& getComponentName() const noexcept { return _name; }
        [[nodiscard]] const Core::StringAtom& getComponentType() const noexcept { return *_type; }

        [[nodiscard]] const ComponentHolder* getParent() const noexcept { return _parent; }
        [[nodiscard]] ComponentHolder* getParent() noexcept { return _parent; }
        [[nodiscard]] bool hasParent() const noexcept { return _parent; }

        [[nodiscard]] bool isValid() const;

        template<IsComponent T>
        [[nodiscard]] bool isTypeOf() const
        {
            return *_type == T::componentType;
        }

        [[nodiscard]] std::size_t makeHash() const;

        void clear() override;

    protected:
        explicit BaseComponent(const Core::StringAtom* type, const Core::StringAtom& name = ""_atom)
            : _type{ type },
              _name{ name }
        {
#ifdef DEBUG
            Assert(_type->isStatic());
#endif
        }

        void onSuccessAddChildComponentValidation(BaseComponent* newComponent) override;

    protected:
        const Core::StringAtom* const _type = nullptr;
        Core::StringAtom _name;

        ComponentHolder* _parent = nullptr;
    };

    // clang-format off
    template<IsComponentOrBase TargetComponentT, bool isConst>
    class BaseComponentIterator final :
        public Core::IBidirectionalIterator
        <
            typename TargetComponentT::template AdaptiveRawPtr<isConst>,
            BaseComponentIterator<TargetComponentT, isConst>,
            Utils::CopyableAndMoveable,
            true
        >
    // clang-format on
    {
    public:
        using ValueT = typename TargetComponentT::template AdaptiveRawPtr<isConst>;

        BaseComponentIterator() = default;
        ~BaseComponentIterator() override = default;

        void swap(BaseComponentIterator& other) override { std::swap(_data, other._data); }
        [[nodiscard]] bool operator==(const BaseComponentIterator& other) const noexcept override
        {
            return _data == other._data;
        }
        [[nodiscard]] bool operator!=(const BaseComponentIterator& other) const noexcept override
        {
            return _data != other._data;
        }
        [[nodiscard]] const ValueT operator*() const noexcept override { return _data; }
        [[nodiscard]] const ValueT operator->() const override { return _data; }
        [[nodiscard]] ValueT operator*() noexcept override { return _data; }
        [[nodiscard]] ValueT operator->() noexcept override { return _data; }
        BaseComponentIterator& operator++() noexcept override { return *this; }
        BaseComponentIterator operator++(int) noexcept override { return *this; }
        BaseComponentIterator& operator--() noexcept override { return *this; }
        BaseComponentIterator operator--(int) noexcept override { return *this; }

    private:
        ValueT _data = nullptr;
    };

    template<IsComponentOrBase T = BaseComponent>
    using ComponentIterator = BaseComponentIterator<T, false>;
    template<IsComponentOrBase T = BaseComponent>
    using ConstComponentIterator = BaseComponentIterator<T, true>;
} // namespace SW

template<>
struct std::hash<SW::BaseComponent>
{
    std::size_t operator()(const SW::BaseComponent& x) const noexcept { return x.makeHash(); }
};

template<>
struct std::hash<SW::BaseComponent::CPtr>
{
    std::size_t operator()(const SW::BaseComponent::CPtr& x) const noexcept
    {
        return x->makeHash();
    }
};

template<>
struct std::hash<SW::BaseComponent::Ptr>
{
    std::size_t operator()(const SW::BaseComponent::Ptr& x) const noexcept { return x->makeHash(); }
};