// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Fundamental::Reflection

#pragma once

#include "Core/String.h"
#include "TypeInfo.h"

#include <JustReflectMe/Adapter.h>
#include <algorithm>
#include <type_traits>
#include <typeindex>
#include <vector>

/// Runtime, type-erased descriptions of JRM-reflected types.
///
/// A descriptor is built once per type from the generated `R<T>` visitors and lives for the whole
/// program. It lets code that only has a pointer (for example an editor holding a
/// `BaseComponent*`) list, read and edit every `FIELD()` of the object's whole class chain.
///
/// Pointer convention: every `owner` pointer passed to a `PropertyDesc` must point to the class
/// that declares the property. `ClassDesc::forEachInChain` and `ObjectRef` hand out correctly
/// adjusted pointers.
namespace NX::Reflection
{
    struct PropertyDesc;

    /// Implemented by types that want to react to edits made through `PropertyDesc::set`
    /// (dirty flags, cache invalidation, ...). Raw writes through `get` do not notify.
    template<class T>
    concept HasOnFieldEdited
        = requires(T& object, const PropertyDesc& property) { object.onFieldEdited(property); };

    /// One reflected field.
    struct PropertyDesc
    {
        /// Field name as declared, e.g. `_fov`.
        Core::StringAtom name;
        /// Generated metadata (type spelling, limits, flags, `R::Meta`). Never null.
        const RClassField* field = nullptr;
        /// Field type without cv-qualifiers.
        std::type_index type = typeid(void);
        /// Structure of `type` and type-erased operations on it. Never null.
        const TypeInfo* typeInfo = nullptr;
        /// Declared `const`: readable, editable only through a setter.
        bool isConst = false;
        /// Declared `R::Meta(Setter = ...)`: `set` calls that member function.
        bool hasSetter = false;

        /// Returns the field inside `owner`.
        void* (*address)(void* owner) = nullptr;
        /// Assigns `*value` (of exactly `type`) through the setter or directly, then notifies the
        /// owner. Null when the property cannot be edited.
        void (*assign)(const PropertyDesc& self, void* owner, const void* value) = nullptr;

        [[nodiscard]] bool isEditable() const noexcept { return assign != nullptr; }

        template<class V>
        [[nodiscard]] bool holds() const noexcept
        {
            return type == typeid(V);
        }

        /// Mutable access; null when `V` is not the field type or the field is const.
        template<class V>
        [[nodiscard]] V* get(void* owner) const noexcept
        {
            return holds<V>() && !isConst ? static_cast<V*>(address(owner)) : nullptr;
        }

        /// Read-only access; null when `V` is not the field type.
        template<class V>
        [[nodiscard]] const V* get(const void* owner) const noexcept
        {
            // address() does not modify the object; it only computes a member's location.
            return holds<V>() ? static_cast<const V*>(address(const_cast<void*>(owner)))
                              : nullptr;
        }

        /// Edits the field the way an editor should: through the declared setter or a direct
        /// assignment, followed by `owner.onFieldEdited(*this)` when the owner provides it.
        /// @return false (and nothing changes) when `V` is not the field type or the property is
        /// not editable.
        template<class V>
        bool set(void* owner, const V& value) const
        {
            if (!holds<V>() || !assign)
            {
                return false;
            }
            assign(*this, owner, &value);
            return true;
        }
    };

    struct ClassDesc;

    template<class T>
    [[nodiscard]] const ClassDesc& Describe();

    /// A reflected base listed in `R_FRIEND`, resolved lazily.
    struct BaseLink
    {
        const ClassDesc& (*describe)() = nullptr;
        /// Converts a pointer to the derived class into a pointer to this base.
        void* (*upcast)(void* derived) = nullptr;
    };

    /// One reflected class or struct: its own properties and its reflected bases.
    struct ClassDesc
    {
        Core::StringAtom name;      // R<T>::FullName(), e.g. "NX::PerspectiveCamera"
        Core::StringAtom shortName; // R<T>::Name(), e.g. "PerspectiveCamera"
        std::type_index type = typeid(void);
        /// Fields declared directly in this class, in declaration order.
        std::vector<PropertyDesc> properties;
        /// Bases listed in `R_FRIEND`, in that order.
        std::vector<BaseLink> bases;

        /// Property declared directly in this class, or null.
        [[nodiscard]] const PropertyDesc* findOwnProperty(const Core::StringAtom& name) const;

        /// Calls `fn(const ClassDesc&, void* owner)` for every class of the chain: bases first
        /// (depth-first, in `R_FRIEND` order), then this class. A base reachable through several
        /// paths is visited once. `object` must point to this class.
        template<class F>
        void forEachInChain(void* object, const F& fn) const
        {
            std::vector<const ClassDesc*> visited;
            forEachInChainImpl(object, fn, visited);
        }

    private:
        template<class F>
        void forEachInChainImpl(void* object, const F& fn,
                                std::vector<const ClassDesc*>& visited) const
        {
            if (std::ranges::find(visited, this) != visited.end())
            {
                return;
            }
            visited.push_back(this);

            for (const BaseLink& base : bases)
            {
                base.describe().forEachInChainImpl(base.upcast(object), fn, visited);
            }
            fn(*this, object);
        }
    };

    /// A property together with the (adjusted) object that owns it.
    struct PropertyRef
    {
        const PropertyDesc* property = nullptr;
        void* owner = nullptr;

        [[nodiscard]] explicit operator bool() const noexcept { return property && owner; }

        template<class V>
        [[nodiscard]] V* get() const noexcept
        {
            return property ? property->get<V>(owner) : nullptr;
        }

        template<class V>
        bool set(const V& value) const
        {
            return property && property->set(owner, value);
        }
    };

    /// A type-erased reference to a reflected object: its most-derived descriptor and address.
    struct ObjectRef
    {
        const ClassDesc* desc = nullptr;
        void* object = nullptr;

        ObjectRef() = default;

        template<class T>
            requires(!std::is_const_v<T> && !std::same_as<T, ObjectRef>)
        explicit ObjectRef(T& obj)
            : desc(&Describe<T>()),
              object(&obj)
        {
        }

        [[nodiscard]] explicit operator bool() const noexcept { return desc && object; }

        /// Calls `fn(const ClassDesc&, PropertyRef)` for every property of the whole chain, in
        /// `ClassDesc::forEachInChain` order and declaration order within each class.
        template<class F>
        void forEachProperty(const F& fn) const
        {
            if (!*this)
            {
                return;
            }
            desc->forEachInChain(object,
                                 [&fn](const ClassDesc& cls, void* owner)
                                 {
                                     for (const PropertyDesc& property : cls.properties)
                                     {
                                         fn(cls, PropertyRef{ &property, owner });
                                     }
                                 });
        }

        /// Finds a property by name; the most-derived declaration wins when names repeat.
        [[nodiscard]] PropertyRef findProperty(const Core::StringAtom& name) const;
    };

    /// Read-only counterpart of `ObjectRef`.
    struct ConstObjectRef
    {
        const ClassDesc* desc = nullptr;
        const void* object = nullptr;

        ConstObjectRef() = default;
        ConstObjectRef(const ObjectRef& ref) noexcept
            : desc(ref.desc),
              object(ref.object)
        {
        }

        [[nodiscard]] explicit operator bool() const noexcept { return desc && object; }
    };

    // =======================================================
    //                  IMPLEMENTATION
    // =======================================================

    namespace Internal
    {
        template<class T, class Setter, auto Member>
        [[nodiscard]] PropertyDesc MakePropertyDesc(const RClassField& field)
        {
            using Declared = std::remove_reference_t<decltype(std::declval<T&>().*Member)>;
            using Value = std::remove_cv_t<Declared>;
            constexpr bool hasSetter = !std::is_same_v<Setter, RNoSetter>;

            PropertyDesc out;
            out.name = Core::StringAtom::Intern(field.name);
            out.field = &field;
            out.type = typeid(Value);
            out.typeInfo = &TypeInfoOf<Value>();
            out.isConst = std::is_const_v<Declared>;
            out.hasSetter = hasSetter;
            out.address = [](void* owner) -> void*
            { return const_cast<Value*>(&(static_cast<T*>(owner)->*Member)); };

            constexpr bool assignable
                = hasSetter
                  || (!std::is_const_v<Declared> && IsCopyAssignableDeep<Value>::value);
            if constexpr (assignable)
            {
                out.assign = [](const PropertyDesc& self, void* owner, const void* value)
                {
                    T& object = *static_cast<T*>(owner);
                    const Value& typed = *static_cast<const Value*>(value);
                    if constexpr (hasSetter)
                    {
                        Setter{}(object, typed);
                    }
                    else
                    {
                        object.*Member = typed;
                    }

                    if constexpr (HasOnFieldEdited<T>)
                    {
                        object.onFieldEdited(self);
                    }
                };
            }
            return out;
        }

        template<class T>
        [[nodiscard]] ClassDesc BuildClassDesc()
        {
            ClassDesc out;
            out.name = Core::StringAtom::Intern(R<T>::FullName());
            out.shortName = Core::StringAtom::Intern(R<T>::Name());
            out.type = typeid(T);

            out.properties.reserve(R<T>::GetFieldNumbers());
            R<T>::ForEachMember(
                [&out](const RClassField& field, auto member, auto setter)
                {
                    out.properties.push_back(
                        MakePropertyDesc<T, decltype(setter), decltype(member)::value>(field));
                });

            R<T>::ForEachBase(
                [&out](auto tag)
                {
                    using Base = decltype(tag)::type;
                    out.bases.push_back(BaseLink{
                        .describe = &Describe<Base>,
                        .upcast = [](void* derived) -> void*
                        { return static_cast<Base*>(static_cast<T*>(derived)); },
                    });
                });
            return out;
        }
    } // namespace Internal

    /// Descriptor of `T`, built on first use from `R<T>` and kept for the program's lifetime.
    template<class T>
    const ClassDesc& Describe()
    {
        static const ClassDesc desc = Internal::BuildClassDesc<std::remove_cv_t<T>>();
        return desc;
    }
} // namespace NX::Reflection
