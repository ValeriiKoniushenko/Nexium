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
#include <queue>
#include <unordered_set>

#define ECS_REGISTER_NEW_COMPONENT(CurrentClass, BaseComponentClass)                               \
public:                                                                                            \
    using Ptr = boost::intrusive_ptr<CurrentClass>;                                                \
    using CPtr = boost::intrusive_ptr<const CurrentClass>;                                         \
    template<bool isConst>                                                                         \
    using AdaptivePtr                                                                              \
        = boost::intrusive_ptr<std::conditional_t<isConst, const CurrentClass, CurrentClass>>;     \
    template<bool isConst>                                                                         \
    using AdaptiveRawPtr = std::conditional_t<isConst, const CurrentClass, CurrentClass>*;         \
                                                                                                   \
    inline static const auto componentType = Core::StringAtom::Intern(#CurrentClass);              \
    CurrentClass(const Core::StringAtom& name = ""_atom)                                           \
        : BaseComponentClass(componentType, name)                                                  \
    {                                                                                              \
    }                                                                                              \
                                                                                                   \
protected:                                                                                         \
    explicit CurrentClass(const Core::StringAtom& type, const Core::StringAtom& name)              \
        : BaseComponentClass(type, name)                                                           \
    {                                                                                              \
    }                                                                                              \
                                                                                                   \
public:

#define ECS_REGISTER_NEW_COMPONENT_TYPE(ClassName)                                                 \
    const bool _##ClassName##_type_registration = SW::GetGlobalComponentFactory().registerNewType( \
        ClassName::componentType,                                                                  \
        []() -> SW::BaseComponent*                                                                 \
        {                                                                                          \
            return new std::conditional_t<std::is_abstract_v<ClassName>, SW::InvalidComponent,     \
                                          ClassName>;                                              \
        });

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

    class GlobalComponentFactory : public Core::StrictSingleton<GlobalComponentFactory>
    {
        SINGLETONS_FRIEND(GlobalComponentFactory);

    public:
        BaseComponent* create(const Core::StringAtom& type);
        bool registerNewType(Core::StringAtom type, std::function<BaseComponent*()>);

    private:
        std::unordered_map<Core::StringAtom, std::function<BaseComponent*()>> _map;
    };

    inline GlobalComponentFactory& GetGlobalComponentFactory()
    {
        return GlobalComponentFactory::instance();
    }

    //
    //     ___   _           _                       _     _____
    //    / _ \ | |         | |                     | |   /  __ \
    //   / /_\ \| |__   ___ | |_  _ __   __ _   ___ | |_  | /  \/
    //   |  _  || '_ \ / __|| __|| '__| / _` | / __|| __| | |
    //   | | | || |_) |\__ \| |_ | |   | (_| || (__ | |_  | \__/\ _
    //   \_| |_/|_.__/ |___/ \__||_|    \__,_| \___| \__|  \____/(_)
    class AbstractComponent :
        public BaseLog,
        public boost::intrusive_ref_counter<BaseComponent>,
        public virtual JsonAdapter
    {
    public:
        AbstractComponent(const AbstractComponent&) = default;
        AbstractComponent(AbstractComponent&& other) noexcept;
        AbstractComponent& operator=(const AbstractComponent&) = default;
        AbstractComponent& operator=(AbstractComponent&& other) noexcept;

        ~AbstractComponent() override = default;

        /**
         * @brief Call it in your main loop. After that if several conditions
         * will be matched(is initialized, is enabled, etc) will be called
         * onTick. Inherit from onTick - and implement your own logic for
         * update and\or draw.
         */
        void tick();

        [[nodiscard]] spdlog::logger* getLogger() const override final { return Ecs::getLogger(); }

        virtual void clear() { _isInited = false; }

        template<IsComponent T>
        [[nodiscard]] T* castTo()
        {
            auto* casted = dynamic_cast<T*>(this);
            Assert(casted);
            return casted;
        }

        template<IsComponent T>
        [[nodiscard]] T* tryCastTo()
        {
            auto* casted = dynamic_cast<T*>(this);
            return casted;
        }

        [[nodiscard]] bool isInited() const noexcept { return _isInited; }
        [[nodiscard]] bool isEnabled() const noexcept { return _isEnabled; }
        void setEnabled(bool v) noexcept { _isEnabled = v; }

        void setNoTick(bool v) { _noTick = v; }
        [[nodiscard]] bool getNoTick() const noexcept { return _noTick; }

        /**
         * @brief Call this function directly only if you sure in it.
         * It should be called only once per one component.
         */
        void initialize()
        {
            if (!_isInited)
            {
                onInit();
                _isInited = true;
            }
        }

        [[nodiscard]] nlohmann::json toJson() const override;
        void fromJson(const nlohmann::json& json, bool isIgnoreChildren = false) override;

    protected:
        AbstractComponent() = default;

        /**
         * @brief This method will be called automatically. Don't call it directly.
         */
        virtual void onInit() {}

        /**
         * @brief This method will be called automatically. Don't call it directly.
         */
        virtual void onTick() {}

        bool _isEnabled = true;

        // if put 'true' means that the function 'tick' will not be called.
        bool _noTick = false;

    private:
        bool _isInited = false;
    };

    //
    //   ______                    _____
    //   | ___ \                  /  __ \
    //   | |_/ /  __ _  ___   ___ | /  \/
    //   | ___ \ / _` |/ __| / _ \| |
    //   | |_/ /| (_| |\__ \|  __/| \__/\ _
    //   \____/  \__,_||___/ \___| \____/(_)
    /**
     * @brief Base class for all your custom components.
     * To create your own component you should do only several things:
     * 1. Create your component's class. I.e. MyNewComponent and inherit from BaseComponent
     * 2. Register as component: inside this class put a macros ECS_REGISTER_NEW_COMPONENT
     * 3. Register as type: inside .cpp file put ECS_REGISTER_NEW_COMPONENT_TYPE
     *
     * @code{cpp}
     * // MyNewComponent.h
     * class MyNewComponent : public BaseComponent {
     *      ECS_REGISTER_NEW_COMPONENT(MyNewComponent, BaseComponent);
     * };
     * @endcode
     * @code{cpp}
     * // MyNewComponent.cpp
     * ECS_REGISTER_NEW_COMPONENT_TYPE(MyNewComponent);
     * @endcode
     *
     * More complex example with MyParentComponent and MyChildComponent
     * @code{cpp}
     * // MyNewComponent.h
     * class MyParentComponent : public BaseComponent {
     *      ECS_REGISTER_NEW_COMPONENT(MyParentComponent, BaseComponent);
     * };
     * class MyChildComponent : public MyParentComponent {
     *      ECS_REGISTER_NEW_COMPONENT(MyChildComponent, MyParentComponent);
     * };
     * @endcode
     * @code{cpp}
     * // MyNewComponent.cpp
     * ECS_REGISTER_NEW_COMPONENT_TYPE(MyParentComponent);
     * ECS_REGISTER_NEW_COMPONENT_TYPE(MyChildComponent);
     * @endcode
     */
    class BaseComponent : public AbstractComponent
    {
    public:
        using Self = BaseComponent;
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
        ~BaseComponent() override = default;

        BaseComponent(BaseComponent&& other) noexcept;
        BaseComponent& operator=(BaseComponent&& other) noexcept;

        [[nodiscard]] bool operator==(const Self& other) const;

        // ========================== WORKING WITH NAME ==========================
        void setComponentName(const Core::StringAtom& name);
        [[nodiscard]] const Core::StringAtom& getComponentName() const noexcept { return _name; }
        [[nodiscard]] const Core::StringAtom& getComponentType() const noexcept { return _type; }

        // ========================== WORKING WITH PARENT ==========================
        [[nodiscard]] const BaseComponent* getParent() const noexcept { return _parent; }
        [[nodiscard]] BaseComponent* getParent() noexcept { return _parent; }
        [[nodiscard]] bool hasParent() const noexcept { return _parent; }

        // ========================== MISC & TYPES ==========================
        void clear() override;
        [[nodiscard]] bool isValid() const;
        [[nodiscard]] std::size_t makeHash() const;

        template<IsComponent T>
        [[nodiscard]] bool isTypeOf() const
        {
            return _type == T::componentType;
        }

        [[nodiscard]] nlohmann::json toJson() const override;
        void fromJson(const nlohmann::json& json, bool isIgnoreChildren = false) override;
        // ========================== WORKING WITH CHILDREN ==========================
        [[nodiscard]] ChildT getChildAt(std::size_t i) { return _children.at(i); }
        [[nodiscard]] CChildT getChildAt(std::size_t i) const { return _children.at(i); }
        [[nodiscard]] const ChildrenT& getChildren() const noexcept { return _children; }
        [[nodiscard]] ChildrenT& getChildren() noexcept { return _children; }
        [[nodiscard]] std::size_t getChildrenCount() const noexcept { return _children.size(); }
        [[nodiscard]] bool hasChildren() const noexcept { return !_children.empty(); }

        template<IsComponent ComponentT>
        [[nodiscard]] ComponentT* findFirstChildOf()
        {
            return Impl_findFirstChildOf<ComponentT, false>(this);
        }

        template<IsComponent ComponentT>
        [[nodiscard]] const ComponentT* findFirstChildOf() const
        {
            return Impl_findFirstChildOf<ComponentT, true>(this);
        }

        template<IsComponent ComponentT, class... Args>
        [[nodiscard]] ComponentT* addChildComponent(Args&&... args)
        {
            typename ComponentT::Ptr newOne = new ComponentT(std::forward<Args>(args)...);
            return static_cast<ComponentT*>(rawAddChildComponent(newOne.get()));
        }

        [[nodiscard]] BaseComponent* rawAddChildComponent(BaseComponent* newOne)
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

        bool removeChild(const BaseComponent* child);
        bool removeChild(const CChildT& child) { return removeChild(child.get()); }
        bool removeChildIf(std::function<bool(const BaseComponent*)>&& pred);

        // ========================== FOREACHes ==========================

        /**
         * @brief Iterate over every child and root recursively(BFS).
         * Can take a functions of next types:
         * 1. bool([const] BaseComponent*) - this function will work until it gets 'false'
         * in return
         * 2. void([const] BaseComponent*) - will iterate without stopping through all a
         * tree
         */
        template<class FuncT>
        void forEach(FuncT&& callback)
        {
            Impl_forEach_BFS<BaseComponent, false>(this,
                                                   std::forward<decltype(callback)>(callback));
        }

        /**
         * @brief Iterate over every child and root recursively(BFS).
         * Can take a functions of next types:
         * 1. bool(const BaseComponent*) - this function will work until it gets 'false'
         * in return
         * 2. void(const BaseComponent*) - will iterate without stopping through all a
         * tree
         */
        template<class FuncT>
        void forEach(FuncT&& callback) const
        {
            Impl_forEach_BFS<BaseComponent, true>(this, std::forward<decltype(callback)>(callback));
        }

        /**
         * @brief Iterate over every child and root recursively(DFS).
         * Can take a functions of next types:
         * 1. bool([const] BaseComponent*) - this function will work until it gets 'false'
         * in return
         * 2. void([const] BaseComponent*) - will iterate without stopping through all a
         * tree
         */
        template<class FuncT>
        void forEachDFS(FuncT&& callback)
        {
            Impl_forEach_DFS<BaseComponent, false>(this,
                                                   std::forward<decltype(callback)>(callback));
        }

        /**
         * @brief Iterate over every child and root recursively(DFS).
         * Can take a functions of next types:
         * 1. bool(const BaseComponent*) - this function will work until it gets 'false'
         * in return
         * 2. void(const BaseComponent*) - will iterate without stopping through all a
         * tree
         */
        template<class FuncT>
        void forEachDFS(FuncT&& callback) const
        {
            Impl_forEach_DFS<BaseComponent, true>(this, std::forward<decltype(callback)>(callback));
        }

    protected:
        [[nodiscard]] virtual bool onAddChildComponentValidation(const BaseComponent* newComponent)
        {
            return true;
        }
        virtual void onSuccessAddChildComponentValidation(BaseComponent* newComponent);

        explicit BaseComponent(const Core::StringAtom& type, const Core::StringAtom& name)
            : _name{ name },
              _type{ type }
        {
#ifdef DEBUG
            Assert(_type.isStatic());
#endif
        }

    protected:
        ChildrenT _children;
        Core::StringAtom _name;
        const Core::StringAtom _type;
        BaseComponent* _parent = nullptr;

    private:
        // ===================== PIMPLs =============================
        template<IsComponentOrBase TargetT, bool isConst, class FuncT>
        static void Impl_forEach_BFS(AdaptiveRawPtr<isConst> me, FuncT&& callback);

        template<IsComponentOrBase TargetT, bool isConst, class FuncT>
        static void Impl_forEach_DFS(AdaptiveRawPtr<isConst> me, FuncT&& callback);

        template<IsComponent TargetT, bool isConst>
        [[nodiscard]] static typename TargetT::template AdaptiveRawPtr<isConst>
            Impl_findFirstChildOf(AdaptiveRawPtr<isConst> me);
    };

    struct InvalidComponent : public BaseComponent
    {
        ECS_REGISTER_NEW_COMPONENT(InvalidComponent, BaseComponent);
    };

    template<IsComponentOrBase TargetT, bool isConst, class FuncT>
    void BaseComponent::Impl_forEach_BFS(AdaptiveRawPtr<isConst> me, FuncT&& callback)
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
                me->criticalLog("BaseComponent::Impl_forEach_BFS was got nullptr for root.");
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
    void BaseComponent::Impl_forEach_DFS(AdaptiveRawPtr<isConst> me, FuncT&& callback)
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
                me->criticalLog("BaseComponent::Impl_forEach_DFS was got nullptr for root.");
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

    template<IsComponent TargetT, bool isConst>
    typename TargetT::template AdaptiveRawPtr<isConst> BaseComponent::Impl_findFirstChildOf(
        BaseComponent::AdaptiveRawPtr<isConst> me)
    {
        typename TargetT::template AdaptiveRawPtr<isConst> found = nullptr;
        me->forEach(
            [&found](BaseComponent* comp)
            {
                Assert(comp->_type.isStatic());
                Assert(TargetT::componentType.isStatic());

                if (comp->_type == TargetT::componentType)
                {
#if defined(DEBUG)
                    found = dynamic_cast<TargetT*>(comp);
#else
                    found = reinterpret_cast<TargetT*>(comp);
#endif
                    return false;
                }
                return true;
            });

        return found;
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