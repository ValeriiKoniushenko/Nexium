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
#include <queue>
#include <unordered_set>

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

    //
    //     ___   _           _                       _     _____
    //    / _ \ | |         | |                     | |   /  __ \
    //   / /_\ \| |__   ___ | |_  _ __   __ _   ___ | |_  | /  \/
    //   |  _  || '_ \ / __|| __|| '__| / _` | / __|| __| | |
    //   | | | || |_) |\__ \| |_ | |   | (_| || (__ | |_  | \__/\ _
    //   \_| |_/|_.__/ |___/ \__||_|    \__,_| \___| \__|  \____/(_)
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

    //
    //    _____      _   _         _      _
    //   /  __ \    | | | |       | |    | |
    //   | /  \/    | |_| |  ___  | |  __| |  ___  _ __
    //   | |        |  _  | / _ \ | | / _` | / _ \| '__|
    //   | \__/\ _  | | | || (_) || || (_| ||  __/| |
    //    \____/(_) \_| |_/ \___/ |_| \__,_| \___||_|
    class ComponentHolder : public AbstractComponent
    {
    public:
        using Self = ComponentHolder;
        template<bool isConst>
        using AdaptivePtr = boost::intrusive_ptr<std::conditional_t<isConst, const Self, Self>>;
        template<bool isConst>
        using AdaptiveRawPtr = std::conditional_t<isConst, const Self, Self>*;
        using Ptr = boost::intrusive_ptr<Self>;
        using CPtr = boost::intrusive_ptr<const Self>;
        using CChildT = boost::intrusive_ptr<const BaseComponent>;
        using ChildT = boost::intrusive_ptr<BaseComponent>;
        using ChildrenT = std::vector<boost::intrusive_ptr<BaseComponent>>;

    public:
        // ========================== WORKING WITH CHILDREN ==========================
        [[nodiscard]] ChildT getChildAt(std::size_t i) { return _children.at(i); }
        [[nodiscard]] CChildT getChildAt(std::size_t i) const { return _children.at(i); }
        [[nodiscard]] const ChildrenT& getChildren() const noexcept { return _children; }
        [[nodiscard]] std::size_t getChildrenCount() const noexcept { return _children.size(); }
        [[nodiscard]] bool hasChildren() const noexcept { return !_children.empty(); }

        template<IsComponent ComponentT, class... Args>
        [[nodiscard]] ComponentT* addChildComponent(Args&&... args)
        {
            typename ComponentT::Ptr newOne = new ComponentT(std::forward<Args>(args)...);
            if (!onAddChildComponentValidation(newOne.get()))
            {
                return nullptr;
            }

            onSuccessAddChildComponentValidation(newOne.get());
            return static_cast<ComponentT*>(_children.emplace_back(std::move(newOne)).get());
        }

        bool removeChild(const BaseComponent* child);
        bool removeChild(const CChildT& child) { return removeChild(child.get()); }
        bool removeChildIf(std::function<bool(const BaseComponent*)>&& pred);

        // ========================== FOREACHes ==========================

        /**
         * @brief Iterate over every child and root recursively(BFS).
         * Can take a functions of next types:
         * 1. bool([const] TargetT*) - this function will work until it gets 'false'
         * in return
         * 2. void([const] TargetT*) - will iterate without stopping through all a
         * tree
         */
        template<IsComponentOrBase TargetT = BaseComponent, class FuncT>
        void forEach(FuncT&& callback)
        {
            impl_forEach_BFS<TargetT, false>(this, std::forward<decltype(callback)>(callback));
        }

        /**
         * @brief Iterate over every child and root recursively(BFS).
         * Can take a functions of next types:
         * 1. bool(const TargetT*) - this function will work until it gets 'false'
         * in return
         * 2. void(const TargetT*) - will iterate without stopping through all a
         * tree
         */
        template<IsComponentOrBase TargetT = BaseComponent, class FuncT>
        void forEach(FuncT&& callback) const
        {
            impl_forEach_BFS<TargetT, true>(this, std::forward<decltype(callback)>(callback));
        }

        /**
         * @brief Iterate over every child and root recursively(DFS).
         * Can take a functions of next types:
         * 1. bool([const] TargetT*) - this function will work until it gets 'false'
         * in return
         * 2. void([const] TargetT*) - will iterate without stopping through all a
         * tree
         */
        template<IsComponentOrBase TargetT = BaseComponent, class FuncT>
        void forEachDFS(FuncT&& callback)
        {
            impl_forEach_DFS<TargetT, false>(this, std::forward<decltype(callback)>(callback));
        }

        /**
         * @brief Iterate over every child and root recursively(DFS).
         * Can take a functions of next types:
         * 1. bool(const TargetT*) - this function will work until it gets 'false'
         * in return
         * 2. void(const TargetT*) - will iterate without stopping through all a
         * tree
         */
        template<IsComponentOrBase TargetT = BaseComponent, class FuncT>
        void forEachDFS(FuncT&& callback) const
        {
            impl_forEach_DFS<TargetT, true>(this, std::forward<decltype(callback)>(callback));
        }

        // ========================== MISC ==========================
        void clear() override;

    protected:
        [[nodiscard]] virtual bool onAddChildComponentValidation(const BaseComponent* newComponent)
        {
            return true;
        }
        virtual void onSuccessAddChildComponentValidation(BaseComponent* newComponent) {}

    protected:
        ChildrenT _children;
        bool _isEnabled = true;

    private:
        // ===================== PIMPLs =============================
        template<IsComponentOrBase TargetT, bool isConst, class FuncT>
        static void impl_forEach_BFS(AdaptiveRawPtr<isConst> me, FuncT&& callback);

        template<IsComponentOrBase TargetT, bool isConst, class FuncT>
        static void impl_forEach_DFS(AdaptiveRawPtr<isConst> me, FuncT&& callback);
    };

    //
    //   ______                    _____
    //   | ___ \                  /  __ \
    //   | |_/ /  __ _  ___   ___ | /  \/
    //   | ___ \ / _` |/ __| / _ \| |
    //   | |_/ /| (_| |\__ \|  __/| \__/\ _
    //   \____/  \__,_||___/ \___| \____/(_)
    class BaseComponent : public ComponentHolder
    {
    public:
        using Self = BaseComponent;
        template<bool isConst>
        using AdaptivePtr = boost::intrusive_ptr<std::conditional_t<isConst, const Self, Self>>;
        template<bool isConst>
        using AdaptiveRawPtr = std::conditional_t<isConst, const Self, Self>*;
        using Ptr = boost::intrusive_ptr<Self>;
        using CPtr = boost::intrusive_ptr<const Self>;

    public:
        ~BaseComponent() override = default;

        [[nodiscard]] bool operator==(const Self& other) const;

        // ========================== WORKING WITH NAME ==========================
        void setComponentName(const Core::StringAtom& name);
        [[nodiscard]] const Core::StringAtom& getComponentName() const noexcept { return _name; }
        [[nodiscard]] const Core::StringAtom& getComponentType() const noexcept { return *_type; }

        // ========================== WORKING WITH PARENT ==========================
        [[nodiscard]] const ComponentHolder* getParent() const noexcept { return _parent; }
        [[nodiscard]] ComponentHolder* getParent() noexcept { return _parent; }
        [[nodiscard]] bool hasParent() const noexcept { return _parent; }

        // ========================== MISC & TYPES ==========================
        void clear() override;
        [[nodiscard]] bool isValid() const;
        [[nodiscard]] std::size_t makeHash() const;

        template<IsComponent T>
        [[nodiscard]] bool isTypeOf() const
        {
            return *_type == T::componentType;
        }

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
        Core::StringAtom _name;
        const Core::StringAtom* const _type = nullptr;
        ComponentHolder* _parent = nullptr;
    };

    template<IsComponentOrBase TargetT, bool isConst, class FuncT>
    void ComponentHolder::impl_forEach_BFS(AdaptiveRawPtr<isConst> me, FuncT&& callback)
    {
        if (!Verify(me)) [[unlikely]]
        {
            return;
        }

        using HolderPtr = AdaptiveRawPtr<isConst>;
        using TargetPtr = typename TargetT::template AdaptiveRawPtr<isConst>;
        std::unordered_set<HolderPtr> visited;
        std::queue<HolderPtr> q;

        visited.emplace(me);
        q.push(me);

        while (!q.empty())
        {
            HolderPtr root = q.front();
            q.pop();

            if (!Verify(root)) [[unlikely]]
            {
                me->criticalLog("ComponentHolder::impl_forEach_BFS was got nullptr for root.");
                return;
            }

            if (auto target = dynamic_cast<TargetPtr>(root))
            {
                if constexpr (std::is_void_v<decltype(callback(target))>)
                {
                    std::invoke(std::forward<decltype(callback)>(callback), target);
                }
                else
                {
                    if (!std::invoke(std::forward<decltype(callback)>(callback), target))
                    {
                        return;
                    }
                }
            }

            for (auto comp : root->_children)
            {
                if (!visited.contains(comp.get()))
                {
                    visited.emplace(comp.get());
                    q.push(comp.get());
                }
            }
        }
    }

    template<IsComponentOrBase TargetT, bool isConst, class FuncT>
    void ComponentHolder::impl_forEach_DFS(AdaptiveRawPtr<isConst> me, FuncT&& callback)
    {
        if (!Verify(me)) [[unlikely]]
        {
            return;
        }

        using HolderPtr = AdaptiveRawPtr<isConst>;
        using TargetPtr = typename TargetT::template AdaptiveRawPtr<isConst>;
        std::unordered_set<HolderPtr> visited;
        std::stack<HolderPtr> s;

        s.push(me);

        while (!s.empty())
        {
            HolderPtr root = s.top();
            s.pop();

            if (!Verify(root)) [[unlikely]]
            {
                me->criticalLog("ComponentHolder::impl_forEach_DFS was got nullptr for root.");
                return;
            }

            if (visited.contains(root))
            {
                continue;
            }

            visited.emplace(root);

            if (auto target = dynamic_cast<TargetPtr>(root))
            {
                if constexpr (std::is_void_v<decltype(callback(target))>)
                {
                    std::invoke(std::forward<decltype(callback)>(callback), target);
                }
                else
                {
                    if (!std::invoke(std::forward<decltype(callback)>(callback), target))
                    {
                        return;
                    }
                }
            }

            for (auto i = root->_children.rbegin(); i != root->_children.rend(); ++i)
            {
                if (!visited.contains(i->get()))
                {
                    s.push(i->get());
                }
            }
        }
    }

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