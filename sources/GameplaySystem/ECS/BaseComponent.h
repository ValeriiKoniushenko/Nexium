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
    [[nodiscard]] const Core::StringAtom& getComponentType() const noexcept                        \
    {                                                                                              \
        return CurrentClass::componentType;                                                        \
    }                                                                                              \
    CurrentClass(const Core::StringAtom& name = ""_atom)                                           \
        : BaseComponentClass(&componentType, name)                                                 \
    {                                                                                              \
    }

namespace SW
{

    class BaseComponent;

    template<class T>
    concept IsComponent = std::derived_from<T, BaseComponent> && requires(T t) {
        { t.getComponentType() };
        { T::componentType };
    };

    class BaseComponent : public BaseLog, public boost::intrusive_ref_counter<BaseComponent>
    {
    public:
        using Self = BaseComponent;
        using Ptr = boost::intrusive_ptr<BaseComponent>;
        using CPtr = boost::intrusive_ptr<const BaseComponent>;

    public:
        ~BaseComponent() override = default;

        [[nodiscard]] bool operator==(const BaseComponent& other) const;

        template<IsComponent T>
        [[nodiscard]] T* castTo()
        {
            auto* casted = dynamic_cast<T*>(this);
            Assert(casted);
            return casted;
        }

        void setComponentName(const Core::StringAtom& name);
        [[nodiscard]] const Core::StringAtom& getComponentName() const noexcept { return _name; }

        [[nodiscard]] const BaseComponent* getParent() const noexcept { return _parent; }
        [[nodiscard]] BaseComponent* getParent() noexcept { return _parent; }
        [[nodiscard]] bool hasParent() const noexcept { return _parent; }

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

            newOne->_parent = this;
            return static_cast<ComponentT*>(_children.emplace_back(std::move(newOne)).get());
        }

        bool removeChild(const BaseComponent* child);
        bool removeChildIf(std::function<bool(const BaseComponent*)>&& pred);

        [[nodiscard]] bool isValid() const;

        [[nodiscard]] spdlog::logger* getLogger() const override final { return Ecs::getLogger(); }
        [[nodiscard]] const char* getPrefix() const override { return "Component"; }

        [[nodiscard]] std::size_t makeHash() const;

    protected:
        explicit BaseComponent(const Core::StringAtom* type, const Core::StringAtom& name = ""_atom)
            : _type{ type },
              _name{ name }
        {
        }

        [[nodiscard]] virtual bool onAddChildComponentValidation(const BaseComponent*)
        {
            return true;
        }

    protected:
        const Core::StringAtom* const _type = nullptr;
        Core::StringAtom _name;

        BaseComponent* _parent = nullptr;
        std::list<Ptr> _children;
    };

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