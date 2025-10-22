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

#pragma once

#include "Misc/BaseLog.h"
#include "Misc/DataStream.h"
#include "ModuleInfo.h"
#include "boost/intrusive_ptr.hpp"
#include "boost/smart_ptr/intrusive_ref_counter.hpp"

#include <queue>
#include <stack>
#include <typeindex>
#include <unordered_set>

// ===============================================================
//                         ECS INTERNAL
// ===============================================================
#define _ECS_COMPONENT_DECL(ClassName, TypeName, BaseComponentClass)                               \
public:                                                                                            \
    using Ptr = boost::intrusive_ptr<TypeName>;                                                    \
    using CPtr = boost::intrusive_ptr<const TypeName>;                                             \
    template<bool isConst>                                                                         \
    using AdaptivePtr                                                                              \
        = boost::intrusive_ptr<std::conditional_t<isConst, const TypeName, TypeName>>;             \
    template<bool isConst>                                                                         \
    using AdaptiveRawPtr = std::conditional_t<isConst, const TypeName, TypeName>*;                 \
                                                                                                   \
    static const StringAtom componentType;                                                         \
                                                                                                   \
public:                                                                                            \
    [[nodiscard]] BaseComponent::Ptr clone() const override                                        \
    {                                                                                              \
        auto out = static_cast<TypeName*>(_tryAllocateECSObject<TypeName>(this));                  \
        out->invalidate();                                                                         \
        return out;                                                                                \
    }                                                                                              \
    [[nodiscard]] static Ptr Create()                                                              \
    {                                                                                              \
        return static_cast<TypeName*>(_tryAllocateECSObject<TypeName>(nullptr));                   \
    }                                                                                              \
                                                                                                   \
protected:                                                                                         \
    explicit ClassName(const StringAtom& type, const StringAtom& name)                             \
        : BaseComponentClass(type, name)                                                           \
    {                                                                                              \
    }                                                                                              \
                                                                                                   \
public:

// ---------------------------------------------------------

#define _ECS_COMPONENT_IMPL(TypeName, Template, TypeNameAsStr)                                     \
    Template const StringAtom TypeName::componentType = []()                                       \
    {                                                                                              \
        auto newType = StringAtom::Intern(TypeNameAsStr);                                          \
        GetGlobalComponentFactory().registerNewType(                                               \
            newType,                                                                               \
            []() -> BaseComponent*                                                                 \
            {                                                                                      \
                return new std::conditional_t<std::is_abstract_v<TypeName>, InvalidComponent,      \
                                              TypeName>;                                           \
            });                                                                                    \
        return newType;                                                                            \
    }();

// ---------------------------------------------------------

#define _ECS_DEFAULT_PUBLIC_CONSTRUCTOR(ClassName, BaseComponentClass)                             \
    explicit ClassName(const StringAtom& name = ""_atom)                                           \
        : BaseComponentClass(componentType, name)                                                  \
    {                                                                                              \
    }

// ---------------------------------------------------------

#define BRACKETS(...) __VA_ARGS__

// ===============================================================

//
//
// ===============================================================
//             !NON! TEMPLATE COMPONENT REGISTRATION
// ===============================================================
//
//
/**
 * Use this macro inside your class body to declare new component.
 * @param ClassName only name of your class
 * @param BaseComponentClass put type of your parent here
 * @details It will register a component, and will be added new static field to your class
 * with name 'componentType'. You can refer to this static field to get type of you component.
 * Also, this macro will create default constructor which can optionally take one argument -
 * new component's name. Example:
 * @code{cpp}
 * // Foo.h
 * class Foo : public BaseComponent
 * {
 *      ECS_COMPONENT_DECL(Foo, BaseComponent);
 * };
 *
 * // If you want to implement a constructor by your own hands, use the macro:
 * // ECS_COMPONENT_DECL_NO_CNSTR - but look at the parent initialization
 * // you must pass at lest your component's type name.
 * class FooBar : public BaseComponent
 * {
 *      ECS_COMPONENT_DECL_NO_CNSTR(Foo, BaseComponent);
 *  public:
 *  FooBar(const StringAtom name = "") :
 *      BaseComponent(componentType, name)
 *      {
 *          std::cout << componentType; //> FooBar
 *      }
 * };
 * @endcode
 * Let's implement your new type. For that just put in the .cpp file for you class
 * the last macro: ECS_COMPONENT_IMPL and pass only class name.
 * @code{cpp}
 * // Foo.cpp
 * ECS_COMPONENT_IMPL(Foo);
 * ECS_COMPONENT_IMPL(FooBar);
 * @endcode
 * If you want to know why we need it:
 * It will register your component inside Entity Component System abstraction. I.g. using
 * GlobalComponentFactory you can create needed type known only name of your type.
 */
#define ECS_COMPONENT_DECL(ClassName, BaseComponentClass)                                          \
    _ECS_COMPONENT_DECL(ClassName, ClassName, BaseComponentClass)                                  \
    _ECS_DEFAULT_PUBLIC_CONSTRUCTOR(ClassName, BaseComponentClass)

#define ECS_COMPONENT_DECL_NO_CNSTR(ClassName, BaseComponentClass)                                 \
    _ECS_COMPONENT_DECL(ClassName, ClassName, BaseComponentClass)

#define ECS_COMPONENT_IMPL(ClassName) _ECS_COMPONENT_IMPL(ClassName, ;, #ClassName)

//
//
// ===============================================================
//                TEMPLATE COMPONENT REGISTRATION
// ===============================================================
//
//
/**
 * WARNING!!! Template ECS doesn't support any reflection due to type mangling by compiler!
 * Use this macro inside your class body to declare new component.
 * @param ClassName only name of your class
 * @param BaseComponentClass put type of your parent here
 * @param ... all you template identifiers, separated with comma
 * @details It will register a component, and will be added new static field to your class
 * with name 'componentType'. You can refer to this static field to get type of you component.
 * Also, this macro will create default constructor which can optionally take one argument -
 * new component's name. Example:
 * @code
 * template<class T>
 * class Foo : public BaseComponent
 * {
 *      ECS_TEMPLATE_COMPONENT_DECL(Foo, BaseComponent, T);
 * };
 * @endcode
 * That's only component declaration! To implement it just continue reading...
 * More complex example:
 * @code{cpp}
 * template<std::size_t Size, class T>
 * class FooBar : public SomeCounterComponent
 * {
 *      ECS_TEMPLATE_COMPONENT_DECL(FooBar, BaseComponent, Size, T);
 * };
 *
 * // Or if you want to use constructor use another macro *_NO_CNSTR
 * template<std::size_t Size, class T>
 * class FooBar : public SomeCounterComponent
 * {
 *      ECS_TEMPLATE_COMPONENT_DECL_NO_CNSTR(Foo, BaseComponent, Size, T);
 *  public:
 *  FooBar(const StringAtom name = "") :
 *      SomeCounterComponent(componentType, name)
 *      {
 *      }
 * };
 * @endcode
 * Let's implement your new type. For that just under your class put a macro
 * ECS_TEMPLATE_COMPONENT_IMPL and pass full class name with all template params,
 * and all parameters of your template (above your class). But, due to compiler
 * specific behavior you MUST wrap first and second argument with macro BRACKETS.
 * @code{cpp}
 * // declaration of you classed above! Now only implementations:
 * ECS_TEMPLATE_COMPONENT_IMPL(
 *      BRACKETS(Foo<T>),
 *      BRACKETS(class T)
 * );
 *
 * ECS_TEMPLATE_COMPONENT_IMPL(
 *      BRACKETS(FooBar<Size, T>),
 *      BRACKETS(std::size_t Size, class T)
 * );
 * @endcode
 * If you want to know why we need it:
 * It will register your component inside Entity Component System abstraction. I.g. using
 * GlobalComponentFactory you can create needed type known only name of your type.
 */
#define ECS_TEMPLATE_COMPONENT_DECL(ClassName, BaseComponentClass, ...)                            \
    _ECS_COMPONENT_DECL(ClassName, BRACKETS(ClassName<__VA_ARGS__>), BaseComponentClass)           \
    _ECS_DEFAULT_PUBLIC_CONSTRUCTOR(ClassName, BaseComponentClass)

#define ECS_TEMPLATE_COMPONENT_DECL_NO_CNSTR(ClassName, BaseComponentClass, ...)                   \
    _ECS_COMPONENT_DECL(ClassName, BRACKETS(ClassName<__VA_ARGS__>), BaseComponentClass)

#define ECS_TEMPLATE_COMPONENT_IMPL(TypeName, Template)                                            \
    _ECS_COMPONENT_IMPL(BRACKETS(TypeName), BRACKETS(template<Template>), typeid(TypeName).name())

namespace Core
{
    class BaseComponent;

    template<class T>
    void* _tryAllocateECSObject(const T* data)
    {
        if constexpr (std::is_abstract_v<T>)
        {
            Assert(false, "You are trying to create somewhere an abstract object");
            return nullptr;
        }
        else if constexpr (std::is_copy_constructible_v<T>)
        {
            if (data)
            {
                return new T(*data);
            }
            return new T;
        }
        else
        {
            return new T;
        }
    }

    // ========================= CONCEPTS =========================
    /**
     * Concept to identify valid components.
     * A type satisfies IsComponent if it inherits from BaseComponent and provides:
     *   - a member function getComponentType()
     *   - a static member componentType
     */
    template<class T>
    concept IsComponent = std::derived_from<T, BaseComponent> && requires(T t) {
        { t.getComponentType() };
        { T::componentType };
    };

    /**
     * Concept for either a BaseComponent-derived type or BaseComponent itself.
     */
    template<class T>
    concept IsComponentOrBase = IsComponent<T> || std::is_same_v<BaseComponent, T>;

    /**
     * Concept for either a BaseComponent-derived type or void.
     * Useful for template functions where void indicates "no component".
     */
    template<class T>
    concept IsComponentOrVoid = IsComponent<T> || std::is_void_v<T>;

    //
    //  _____                             ______               _
    // /  __ \                            |  ___|             | |
    // | /  \/  ___   _ __ ___   _ __     | |_     __ _   ___ | |_   ___   _ __  _   _
    // | |     / _ \ | '_ ` _ \ | '_ \    |  _|   / _` | / __|| __| / _ \ | '__|| | | |
    // | \__/\| (_) || | | | | || |_) | _ | |    | (_| || (__ | |_ | (_) || |   | |_| |
    //  \____/ \___/ |_| |_| |_|| .__/ (_)\_|     \__,_| \___| \__| \___/ |_|    \__, |
    //                          | |                                               __/ |
    //                          |_|                                              |___/
    /**
     * Global factory for creating and registering components by type.
     * Manages mapping between component type names and factory functions.
     * Provides logging and debug tracking of component types in DEBUG mode.
     * Try to don't use by your own hands, it will register new component
     * automatically. How? See first comment above class BaseComponent
     */
    class GlobalComponentFactory : public StrictSingleton<GlobalComponentFactory>, public BaseLog
    {
        SINGLETONS_FRIEND(GlobalComponentFactory);

    public:
        /**
         * Create a component by its registered type.
         * @param type The type identifier (StringAtom) of the component.
         * @return Pointer to a new BaseComponent instance, or nullptr if type not registered.
         */
        BaseComponent* create(const StringAtom& type);

        /**
         * Register a new component type in the factory.
         * @param type The type identifier (StringAtom).
         * @param callback Function that creates instances of this type.
         * @return True if registration succeeds, false if type already exists.
         */
        bool registerNewType(const StringAtom& type, std::function<BaseComponent*()> callback);

        [[nodiscard]] spdlog::logger* getLogger() const final { return Ecs::getLogger(); }

    private:
        std::unordered_map<StringAtom, std::function<BaseComponent*()>> _map;
    };

    /** Shortcut to access the global component factory singleton. */
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
    /**
     * Abstract base class for all components.
     * Provides lifecycle hooks, ticking mechanism, and JSON serialization.
     */
    class AbstractComponent :
        public BaseLog,
        public boost::intrusive_ref_counter<BaseComponent>,
        public IDataStreamBridge
    {
    public:
        AbstractComponent(const AbstractComponent&) = default;

        AbstractComponent(AbstractComponent&& other) noexcept;

        AbstractComponent& operator=(const AbstractComponent&) = default;

        AbstractComponent& operator=(AbstractComponent&& other) noexcept;

        ~AbstractComponent() override = default;

        /**
         * Call it in your main loop. After that if several conditions
         * will be matched(is initialized, is enabled, etc.) will be called
         * onTick. Inherit from onTick - and implement your own logic for
         * update and/or draw.
         */
        void tick(float delta);

        void ioFieldsUpdate(DataStream& stream) override;

        [[nodiscard]] spdlog::logger* getLogger() const final { return Ecs::getLogger(); }

        /** Reset the component to uninitialized state. */
        virtual void clear() { _isInitialized = false; }

        /** Safe cast to a derived component type. Asserts if cast fails. */
        template<IsComponent T>
        [[nodiscard]] T* castTo()
        {
            auto* casted = dynamic_cast<std::remove_reference_t<T>*>(this);
            Assert(casted);
            return casted;
        }

        /** Safe cast to a derived component type. Asserts if cast fails. */
        template<IsComponent T>
        [[nodiscard]] const T* castTo() const
        {
            auto* casted = dynamic_cast<const std::remove_reference_t<T>*>(this);
            Assert(casted);
            return casted;
        }

        /** Unsafe & fast cast to a derived component type. */
        template<IsComponent T>
        [[nodiscard]] T* unsafeCastTo() noexcept
        {
            return reinterpret_cast<std::remove_reference_t<T>*>(this);
        }

        /** Unsafe & fast cast to a derived component type. */
        template<IsComponent T>
        [[nodiscard]] const T* unsafeCastTo() const noexcept
        {
            return reinterpret_cast<const std::remove_reference_t<T>*>(this);
        }

        /** Attempt to cast to a derived component type. Returns nullptr if cast fails. */
        template<IsComponent T>
        [[nodiscard]] T* tryCastTo()
        {
            return dynamic_cast<std::remove_reference_t<T>*>(this);
        }

        /** Attempt to cast to a derived component type. Returns nullptr if cast fails. */
        template<IsComponent T>
        [[nodiscard]] const T* tryCastTo() const
        {
            return dynamic_cast<const std::remove_reference_t<T>*>(this);
        }

        [[nodiscard]] bool isInitialized() const noexcept { return _isInitialized; }
        [[nodiscard]] bool isEnabled() const noexcept { return _isEnabled; }
        void setEnabled(bool v) noexcept { _isEnabled = v; }

        void setNoTick(bool v) { _noTick = v; }
        [[nodiscard]] bool getNoTick() const noexcept { return _noTick; }

        /**
         * Call this function directly only if you sure in it.
         * It should be called only once per one component.
         */
        virtual void initialize()
        {
            if (!_isInitialized)
            {
                _isInitialized = true;
                onInitialize();
            }
        }

        /**
         * Call this function if you want to this object was
         * reinited later.
         */
        void invalidate() { _isInitialized = false; }

    protected:
        AbstractComponent() = default;

        /**
         * This method will be called automatically. Don't call it directly.
         */
        virtual void onInitialize() {}

        /**
         * This method will be called automatically. Don't call it directly.
         */
        virtual void onTick(float delta) {}

        bool _isEnabled = true;

        // if put 'true' means that the function 'tick' will not be called.
        bool _noTick = false;

    private:
        bool _isInitialized = false;
    };

    //
    //   ______                    _____
    //   | ___ \                  /  __ \
    //   | |_/ /  __ _  ___   ___ | /  \/
    //   | ___ \ / _` |/ __| / _ \| |
    //   | |_/ /| (_| |\__ \|  __/| \__/\ _
    //   \____/  \__,_||___/ \___| \____/(_)
    /**
     * Base class for all your custom components.
     * To create your own component you should do only several things:
     * 1. Create your component's class. I.e. MyNewComponent and inherit from BaseComponent
     * 2. Register as component: inside your new class put a macros ECS_COMPONENT_DECL
     * 3. Register as type: inside .cpp file put ECS_COMPONENT_IMPL
     * @code{cpp}
     * // MyNewComponent.h
     * class MyNewComponent : public BaseComponent {
     *      ECS_COMPONENT_DECL(MyNewComponent, BaseComponent);
     * };
     * @endcode
     * Implementation of new component:
     * @code{cpp}
     * // MyNewComponent.cpp
     * ECS_COMPONENT_IMPL(MyNewComponent);
     * @endcode
     * More complex example with MyParentComponent and MyChildComponent
     * @code{cpp}
     * // MyNewComponent.h
     * class MyParentComponent : public BaseComponent {
     *      ECS_COMPONENT_DECL(MyParentComponent, BaseComponent);
     * };
     * class MyChildComponent : public MyParentComponent {
     *      ECS_COMPONENT_DECL(MyChildComponent, MyParentComponent);
     * };
     * @endcode
     * This ECS_COMPONENT_DECL macro will create one default constructor. And
     * it can optionally take a component's name. If you want to create a component's
     * constructor by your own hand, use instead ECS_COMPONENT_DECL_NO_CNSTR.
     * To get more info about that, check the declaration of this macro.
     * Implementation of new components:
     * @code{cpp}
     * // MyNewComponent.cpp
     * ECS_COMPONENT_IMPL(MyParentComponent);
     * ECS_COMPONENT_IMPL(MyChildComponent);
     * @endcode
     * But, if you still want to create a class, but not usual but template - see
     * declaration (and comment above) of macro ECS_TEMPLATE_COMPONENT_DECL
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

        BaseComponent(const BaseComponent& other);

        BaseComponent& operator=(const BaseComponent& other);

        [[nodiscard]] virtual bool operator==(const Self& other) const;

        // ========================== WORKING WITH NAME ==========================
        void setComponentName(const StringAtom& name);

        void setComponentName(StringAtom&& name);

        [[nodiscard]] const StringAtom& getComponentName() const noexcept { return _name; }
        [[nodiscard]] const StringAtom& getComponentType() const noexcept { return _type; }

        // ========================== WORKING WITH PARENT ==========================
        [[nodiscard]] const BaseComponent* getParent() const noexcept { return _parent; }
        [[nodiscard]] BaseComponent* getParent() noexcept { return _parent; }
        [[nodiscard]] bool hasParent() const noexcept { return _parent; }

        [[nodiscard]] BaseComponent* getOwner() noexcept;

        template<IsComponent T>
        [[nodiscard]] const T* getParentAs() const noexcept
        {
            return _parent ? _parent->template tryCastTo<T>() : nullptr;
        }

        template<IsComponent T>
        [[nodiscard]] T* getParentAs() noexcept
        {
            return _parent ? _parent->template tryCastTo<T>() : nullptr;
        }

        [[nodiscard]] const BaseComponent* getOwner() const noexcept
        {
            return const_cast<Self*>(this)->getOwner();
        }

        // ========================== MISC & TYPES ==========================
        void clear() override;

        [[nodiscard]] virtual bool isValid() const;

        [[nodiscard]] std::size_t makeHash() const;

        virtual BaseComponent::Ptr clone() const { return nullptr; }

        template<IsComponent T>
        [[nodiscard]] bool isTypeOf() const
        {
            return _type == T::componentType;
        }

        /**
         * This method will be called automatically. Don't call it directly.
         */
        void onTick(float delta) override;

        void ioFieldsUpdate(DataStream& stream) override;

        // ========================== WORKING WITH CHILDREN ==========================
        [[nodiscard]] ChildT getFirstChild() { return _children.front(); }
        [[nodiscard]] ChildT getLastChild() { return _children.back(); }

        template<IsComponent T>
        [[nodiscard]] T::Ptr getFirstChildAs()
        {
            if (auto&& child = _children.front())
            {
                return child->castTo<T>();
            }
            return nullptr;
        }

        template<IsComponent T>
        [[nodiscard]] T::Ptr getLastChildAs()
        {
            if (auto&& child = _children.back())
            {
                return child->castTo<T>();
            }
            return nullptr;
        }

        [[nodiscard]] ChildT getChildAt(std::size_t i) { return _children.at(i); }
        [[nodiscard]] CChildT getChildAt(std::size_t i) const { return _children.at(i); }
        [[nodiscard]] const ChildrenT& getChildren() const noexcept { return _children; }
        [[nodiscard]] ChildrenT& getChildren() noexcept { return _children; }
        [[nodiscard]] std::size_t getChildrenCount() const noexcept { return _children.size(); }
        [[nodiscard]] bool hasChildren() const noexcept { return !_children.empty(); }

        template<IsComponent ComponentT>
        [[nodiscard]] bool hasChildrenAs() const noexcept
        {
            return findFirstChildOf<ComponentT>();
        }

        template<IsComponent ComponentT>
        [[nodiscard]] ComponentT* findFirstChildOf(const StringAtom& name = ""_atom)
        {
            return Impl_findFirstChildOf<ComponentT, false>(this, name);
        }

        template<IsComponent ComponentT>
        [[nodiscard]] const ComponentT* findFirstChildOf(const StringAtom& name = ""_atom) const
        {
            return Impl_findFirstChildOf<ComponentT, true>(this, name);
        }

        template<IsComponent ComponentT, class... Args>
        [[nodiscard]] ComponentT* getOrAddChildComponent(Args&&... args)
        {
            if (auto* found = findFirstChildOf<ComponentT>())
            {
                return found;
            }

            return addChildComponent<ComponentT>(std::forward<Args>(args)...);
        }

        template<IsComponent ComponentT, class... Args>
        ComponentT* addChildComponent(Args&&... args)
        {
            typename ComponentT::Ptr newOne = new ComponentT(std::forward<Args>(args)...);
            return static_cast<ComponentT*>(rawAddChildComponent(newOne.get()));
        }

        /**
         * attach existing child or just new one. Your child will be cloned
         * to this class/owner
         */
        BaseComponent* attachChild(const BaseComponent::Ptr& child);

        void detachChild(BaseComponent* child);

        void removeChild(const BaseComponent* child);

        void removeChild(const CChildT& child) { removeChild(child.get()); }

        void removeChildIf(const std::function<bool(const BaseComponent*)>& pred);

        template<IsComponent ComponentT>
        void removeChildOf(const StringAtom& name = ""_atom)
        {
            removeChildIf(
                [](auto* child)
                {
                    return child->template tryCastTo<ComponentT>() != nullptr;
                });
        }

        // ========================== FOR EACHes ==========================

        /**
         * Iterate over every child and root recursively(BFS).
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
         * Iterate over every child and root recursively(BFS).
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
         * Iterate over every child and root recursively(DFS).
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
         * Iterate over every child and root recursively(DFS).
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
        virtual bool addChildValidator(BaseComponent* newChild) { return true; }

        virtual void onAddChild(BaseComponent* newChild) {}

        virtual void onRemoveChild(BaseComponent* child) {}

        explicit BaseComponent(StringAtom type, StringAtom name)
            : _name{ std::move(name) },
              _type{ std::move(type) }
        {
#ifdef DEBUG
            Assert(_type.isStatic());
#endif
        }

        [[nodiscard]] StringAtom getCacheHash() const override;

    protected:
        ChildrenT _children;
        StringAtom _name;
        StringAtom _type;
        BaseComponent* _parent = nullptr;

    private:
        [[nodiscard]] BaseComponent* rawAddChildComponent(BaseComponent* newOne);

        // ===================== PIMPLs =============================
        template<IsComponentOrBase TargetT, bool isConst, class FuncT>
        static void Impl_forEach_BFS(AdaptiveRawPtr<isConst> me, FuncT&& callback);

        template<IsComponentOrBase TargetT, bool isConst, class FuncT>
        static void Impl_forEach_DFS(AdaptiveRawPtr<isConst> me, FuncT&& callback);

        template<IsComponent TargetT, bool isConst>
        [[nodiscard]] static TargetT::template AdaptiveRawPtr<isConst> Impl_findFirstChildOf(
            AdaptiveRawPtr<isConst> me, const StringAtom& name);
    };

    struct InvalidComponent : public BaseComponent
    {
        ECS_COMPONENT_DECL(InvalidComponent, BaseComponent);
    };

    template<IsComponentOrBase TargetT, bool isConst, class FuncT>
    void BaseComponent::Impl_forEach_BFS(AdaptiveRawPtr<isConst> me, FuncT&& callback)
    {
        if (!Verify(me)) [[unlikely]]
        {
            return;
        }

        using HolderPtr = AdaptiveRawPtr<isConst>;
        using TargetPtr = TargetT::template AdaptiveRawPtr<isConst>;
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
        using TargetPtr = TargetT::template AdaptiveRawPtr<isConst>;
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
    TargetT::template AdaptiveRawPtr<isConst> BaseComponent::Impl_findFirstChildOf(
        AdaptiveRawPtr<isConst> me, const StringAtom& name)
    {
        typename TargetT::template AdaptiveRawPtr<isConst> found = nullptr;

        me->forEach(
            [&found, &name](BaseComponent* comp)
            {
                Assert(comp->_type.isStatic());
                Assert(TargetT::componentType.isStatic());

                if (comp->_type == TargetT::componentType)
                {
#if defined(DEBUG)
                    found = dynamic_cast<TargetT*>(comp);
#else
                    found = static_cast<TargetT*>(comp);
#endif
                    if (!name.isEmpty() && found)
                    {
                        if (found->_name == name)
                        {
                            return false;
                        }
                        return true;
                    }

                    return false;
                }
                return true;
            });

        return found;
    }
} // namespace Core

template<>
struct std::hash<Core::BaseComponent>
{
    std::size_t operator()(const Core::BaseComponent& x) const noexcept { return x.makeHash(); }
};

template<>
struct std::hash<Core::BaseComponent::CPtr>
{
    std::size_t operator()(const Core::BaseComponent::CPtr& x) const noexcept
    {
        return x->makeHash();
    }
};

template<>
struct std::hash<Core::BaseComponent::Ptr>
{
    std::size_t operator()(const Core::BaseComponent::Ptr& x) const noexcept
    {
        return x->makeHash();
    }
};