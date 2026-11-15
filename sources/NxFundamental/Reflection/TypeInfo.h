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

#include <JustReflectMe/Adapter.h>
#include <algorithm>
#include <any>
#include <array>
#include <cstdint>
#include <map>
#include <optional>
#include <span>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

// Include through "TypeDescriptor.h": class-kind infos call Describe<T>(), defined there.

/// What a field's type is, structurally, with the operations an editor needs to change a value
/// it only knows by address: copy it, and edit enums, optionals, sequences and maps in place.
/// One `TypeInfo` exists per type (`TypeInfoOf<T>()`); `PropertyDesc::typeInfo` points to it.
namespace NX::Reflection
{
    struct ClassDesc;

    template<class T>
    [[nodiscard]] const ClassDesc& Describe();

    enum class TypeKind : std::uint8_t
    {
        /// Edited as a whole by a registered editor (numbers, strings, vectors, colours, ...).
        Value,
        /// An enum; JRM-reflected enums list their enumerators.
        Enum,
        /// A JRM-reflected class or struct: edited field by field.
        Class,
        /// `std::optional<T>`.
        Optional,
        /// `std::vector<T>` (resizable) or `std::array<T, N>` (fixed size).
        Sequence,
        /// `std::map` / `std::unordered_map` keyed by `Core::StringAtom` or `std::string`.
        Map,
    };

    struct EnumEntry
    {
        Core::StringAtom name;
        /// The underlying value's bits, so any enum round-trips through `static_cast`.
        std::uint64_t bits = 0;
    };

    /// Type-erased operations on values of one type. Every `void*` points to a value of exactly
    /// `type()`. Operations that do not apply to the kind are no-ops returning empty results.
    class TypeInfo
    {
    public:
        TypeInfo(const TypeInfo&) = delete;
        TypeInfo(TypeInfo&&) = delete;
        TypeInfo& operator=(const TypeInfo&) = delete;
        TypeInfo& operator=(TypeInfo&&) = delete;
        virtual ~TypeInfo() = default;

        [[nodiscard]] std::type_index type() const noexcept { return _type; }
        [[nodiscard]] TypeKind kind() const noexcept { return _kind; }

        /// Copy-constructible and copy-assignable, including container elements.
        [[nodiscard]] bool isCopyable() const noexcept { return _isCopyable; }

        /// A copy of `*value`, or an empty `std::any` when the type is not copyable.
        [[nodiscard]] virtual std::any copy(const void* value) const = 0;
        /// The value inside a `std::any` made by `copy`, or null.
        [[nodiscard]] virtual void* address(std::any& box) const = 0;
        /// `*target = *source`; false (and nothing changes) when the type is not copyable.
        virtual bool assign(void* target, const void* source) const = 0;

        // ---- Enum ----
        [[nodiscard]] virtual std::span<const EnumEntry> enumEntries() const { return {}; }
        [[nodiscard]] virtual std::uint64_t enumBits(const void* /*value*/) const { return 0; }
        virtual void setEnumBits(void* /*value*/, std::uint64_t /*bits*/) const {}

        // ---- Class ----
        [[nodiscard]] virtual const ClassDesc* classDesc() const { return nullptr; }

        // ---- Optional, Sequence, Map ----
        /// The contained, element or mapped type.
        [[nodiscard]] virtual const TypeInfo* element() const { return nullptr; }

        // ---- Optional ----
        [[nodiscard]] virtual bool hasValue(const void* /*value*/) const { return false; }
        /// The contained value, or null when empty.
        [[nodiscard]] virtual void* value(void* /*value*/) const { return nullptr; }
        /// Sets a default-constructed value.
        virtual void emplace(void* /*value*/) const {}
        virtual void reset(void* /*value*/) const {}

        // ---- Sequence ----
        /// No `append`/`erase` (std::array, or elements that cannot be default-constructed).
        [[nodiscard]] virtual bool isFixedSize() const { return true; }
        [[nodiscard]] virtual std::size_t size(const void* /*value*/) const { return 0; }
        /// Element `index`, or null when out of range.
        [[nodiscard]] virtual void* at(void* /*value*/, std::size_t /*index*/) const
        {
            return nullptr;
        }
        /// Appends a default-constructed element.
        virtual void append(void* /*value*/) const {}
        virtual void erase(void* /*value*/, std::size_t /*index*/) const {}
        /// Moves element `from` to position `to`, shifting the ones in between.
        virtual void move(void* /*value*/, std::size_t /*from*/, std::size_t /*to*/) const {}

        // ---- Map ----
        /// Keys in the container's own order (unordered for hash maps).
        [[nodiscard]] virtual std::vector<Core::StringAtom> keys(const void* /*value*/) const
        {
            return {};
        }
        /// The value stored for `key`, or null.
        [[nodiscard]] virtual void* find(void* /*value*/, const Core::StringAtom& /*key*/) const
        {
            return nullptr;
        }
        /// Adds `key` with a default-constructed value; false when it is already there.
        virtual bool insert(void* /*value*/, const Core::StringAtom& /*key*/) const { return false; }
        virtual bool eraseKey(void* /*value*/, const Core::StringAtom& /*key*/) const
        {
            return false;
        }

    protected:
        TypeInfo(std::type_index type, TypeKind kind, bool isCopyable) noexcept
            : _type(type),
              _kind(kind),
              _isCopyable(isCopyable)
        {
        }

    private:
        std::type_index _type;
        TypeKind _kind;
        bool _isCopyable;
    };

    template<class T>
    [[nodiscard]] const TypeInfo& TypeInfoOf();

    // =======================================================
    //                  IMPLEMENTATION
    // =======================================================

    namespace Internal
    {
        /// `std::is_copy_assignable` reports true for standard containers of move-only elements,
        /// because their `operator=` is not constrained. This also inspects element types.
        template<class T>
        struct IsCopyAssignableDeep : std::is_copy_assignable<T>
        {
        };

        template<class T>
            requires requires {
                typename T::value_type;
                typename T::iterator;
            } && (!requires { typename T::mapped_type; })
        struct IsCopyAssignableDeep<T>
            : std::bool_constant<std::is_copy_assignable_v<T>
                                 && IsCopyAssignableDeep<typename T::value_type>::value>
        {
        };

        // Maps hold pair<const Key, Mapped>, which is never assignable; check the parts instead.
        template<class T>
            requires requires {
                typename T::key_type;
                typename T::mapped_type;
            }
        struct IsCopyAssignableDeep<T>
            : std::bool_constant<std::is_copy_assignable_v<T>
                                 && IsCopyAssignableDeep<typename T::key_type>::value
                                 && IsCopyAssignableDeep<typename T::mapped_type>::value>
        {
        };

        template<class T>
        inline constexpr bool IsCopyableValue
            = IsCopyAssignableDeep<T>::value && std::is_copy_constructible_v<T>;

        template<class T, template<class...> class Template>
        struct IsSpecializationOf : std::false_type
        {
        };

        template<template<class...> class Template, class... Args>
        struct IsSpecializationOf<Template<Args...>, Template> : std::true_type
        {
        };

        template<class T>
        struct IsStdArray : std::false_type
        {
        };

        template<class U, std::size_t N>
        struct IsStdArray<std::array<U, N>> : std::true_type
        {
        };

        template<class T>
        concept IsOptional = IsSpecializationOf<T, std::optional>::value;

        template<class T>
        concept IsSequence = IsSpecializationOf<T, std::vector>::value || IsStdArray<T>::value;

        template<class K>
        concept IsStringKey = std::same_as<K, Core::StringAtom> || std::same_as<K, std::string>;

        template<class T>
        concept IsStringKeyedMap
            = (IsSpecializationOf<T, std::map>::value
               || IsSpecializationOf<T, std::unordered_map>::value)
              && IsStringKey<typename T::key_type>;

        template<class T>
        concept IsReflectedClass = std::is_class_v<T> && requires {
            R<T>::FullName();
            R<T>::GetFieldNumbers();
        };

        template<class T>
        concept IsReflectedEnum = std::is_enum_v<T> && requires {
            R<T>::ToArrayC();
            R<T>::ToArrayN();
        };

        template<class T>
        [[nodiscard]] consteval TypeKind KindOf() noexcept
        {
            if constexpr (std::is_enum_v<T>)
            {
                return TypeKind::Enum;
            }
            else if constexpr (IsOptional<T>)
            {
                return TypeKind::Optional;
            }
            else if constexpr (IsSequence<T>)
            {
                return TypeKind::Sequence;
            }
            else if constexpr (IsStringKeyedMap<T>)
            {
                return TypeKind::Map;
            }
            else if constexpr (IsReflectedClass<T>)
            {
                return TypeKind::Class;
            }
            else
            {
                return TypeKind::Value;
            }
        }

        template<class K>
        [[nodiscard]] K KeyFromAtom(const Core::StringAtom& key)
        {
            if constexpr (std::same_as<K, std::string>)
            {
                return key.toStdString();
            }
            else
            {
                return key;
            }
        }

        template<class K>
        [[nodiscard]] Core::StringAtom AtomFromKey(const K& key)
        {
            if constexpr (std::same_as<K, std::string>)
            {
                return Core::StringAtom(std::string_view(key));
            }
            else
            {
                return key;
            }
        }

        template<class T>
        class TypeInfoImpl final : public TypeInfo
        {
        public:
            static constexpr TypeKind kind = KindOf<T>();

            TypeInfoImpl()
                : TypeInfo(typeid(T), kind, IsCopyableValue<T>)
            {
                if constexpr (IsReflectedEnum<T>)
                {
                    const auto& values = R<T>::ToArrayC();
                    const auto& names = R<T>::ToArrayN();
                    _entries.reserve(values.size());
                    for (std::size_t i = 0; i < values.size(); ++i)
                    {
                        _entries.push_back({ .name = Core::StringAtom::Intern(names[i]),
                                             .bits = ToBits(values[i]) });
                    }
                }
            }

            [[nodiscard]] std::any copy(const void* value) const override
            {
                if constexpr (IsCopyableValue<T>)
                {
                    return std::any(std::in_place_type<T>, *static_cast<const T*>(value));
                }
                else
                {
                    return {};
                }
            }

            [[nodiscard]] void* address(std::any& box) const override
            {
                if constexpr (IsCopyableValue<T>)
                {
                    return std::any_cast<T>(&box);
                }
                else
                {
                    return nullptr;
                }
            }

            bool assign(void* target, const void* source) const override
            {
                if constexpr (IsCopyableValue<T>)
                {
                    *static_cast<T*>(target) = *static_cast<const T*>(source);
                    return true;
                }
                else
                {
                    return false;
                }
            }

            // ---- Enum ----

            [[nodiscard]] std::span<const EnumEntry> enumEntries() const override
            {
                return _entries;
            }

            [[nodiscard]] std::uint64_t enumBits(const void* value) const override
            {
                if constexpr (std::is_enum_v<T>)
                {
                    return ToBits(*static_cast<const T*>(value));
                }
                else
                {
                    return 0;
                }
            }

            void setEnumBits(void* value, std::uint64_t bits) const override
            {
                if constexpr (std::is_enum_v<T>)
                {
                    *static_cast<T*>(value)
                        = static_cast<T>(static_cast<std::underlying_type_t<T>>(bits));
                }
            }

            // ---- Class ----

            [[nodiscard]] const ClassDesc* classDesc() const override
            {
                if constexpr (kind == TypeKind::Class)
                {
                    return &Describe<T>();
                }
                else
                {
                    return nullptr;
                }
            }

            // ---- Optional, Sequence, Map ----

            [[nodiscard]] const TypeInfo* element() const override
            {
                if constexpr (kind == TypeKind::Optional || kind == TypeKind::Sequence)
                {
                    return &TypeInfoOf<typename T::value_type>();
                }
                else if constexpr (kind == TypeKind::Map)
                {
                    return &TypeInfoOf<typename T::mapped_type>();
                }
                else
                {
                    return nullptr;
                }
            }

            // ---- Optional ----

            [[nodiscard]] bool hasValue(const void* value) const override
            {
                if constexpr (kind == TypeKind::Optional)
                {
                    return static_cast<const T*>(value)->has_value();
                }
                else
                {
                    return false;
                }
            }

            [[nodiscard]] void* value(void* value) const override
            {
                if constexpr (kind == TypeKind::Optional)
                {
                    auto& optional = *static_cast<T*>(value);
                    return optional ? static_cast<void*>(&*optional) : nullptr;
                }
                else
                {
                    return nullptr;
                }
            }

            void emplace(void* value) const override
            {
                if constexpr (CanEmplace)
                {
                    static_cast<T*>(value)->emplace();
                }
            }

            void reset(void* value) const override
            {
                if constexpr (kind == TypeKind::Optional)
                {
                    static_cast<T*>(value)->reset();
                }
            }

            // ---- Sequence ----

            [[nodiscard]] bool isFixedSize() const override { return !IsResizable; }

            [[nodiscard]] std::size_t size(const void* value) const override
            {
                if constexpr (kind == TypeKind::Sequence)
                {
                    return static_cast<const T*>(value)->size();
                }
                else
                {
                    return 0;
                }
            }

            [[nodiscard]] void* at(void* value, std::size_t index) const override
            {
                if constexpr (kind == TypeKind::Sequence)
                {
                    auto& sequence = *static_cast<T*>(value);
                    return index < sequence.size() ? static_cast<void*>(&sequence[index])
                                                   : nullptr;
                }
                else
                {
                    return nullptr;
                }
            }

            void append(void* value) const override
            {
                if constexpr (IsResizable)
                {
                    static_cast<T*>(value)->emplace_back();
                }
            }

            void erase(void* value, std::size_t index) const override
            {
                if constexpr (IsResizable)
                {
                    auto& sequence = *static_cast<T*>(value);
                    if (index < sequence.size())
                    {
                        sequence.erase(sequence.begin() + static_cast<std::ptrdiff_t>(index));
                    }
                }
            }

            void move(void* value, std::size_t from, std::size_t to) const override
            {
                if constexpr (CanMove)
                {
                    auto& sequence = *static_cast<T*>(value);
                    if (from >= sequence.size() || to >= sequence.size() || from == to)
                    {
                        return;
                    }
                    const auto begin = sequence.begin();
                    const auto f = static_cast<std::ptrdiff_t>(from);
                    const auto t = static_cast<std::ptrdiff_t>(to);
                    if (from < to)
                    {
                        std::rotate(begin + f, begin + f + 1, begin + t + 1);
                    }
                    else
                    {
                        std::rotate(begin + t, begin + f, begin + f + 1);
                    }
                }
            }

            // ---- Map ----

            [[nodiscard]] std::vector<Core::StringAtom> keys(const void* value) const override
            {
                std::vector<Core::StringAtom> out;
                if constexpr (kind == TypeKind::Map)
                {
                    const auto& map = *static_cast<const T*>(value);
                    out.reserve(map.size());
                    for (const auto& entry : map)
                    {
                        out.push_back(AtomFromKey(entry.first));
                    }
                }
                return out;
            }

            [[nodiscard]] void* find(void* value, const Core::StringAtom& key) const override
            {
                if constexpr (kind == TypeKind::Map)
                {
                    auto& map = *static_cast<T*>(value);
                    const auto it = map.find(KeyFromAtom<typename T::key_type>(key));
                    return it != map.end() ? static_cast<void*>(&it->second) : nullptr;
                }
                else
                {
                    return nullptr;
                }
            }

            bool insert(void* value, const Core::StringAtom& key) const override
            {
                if constexpr (CanInsert)
                {
                    auto& map = *static_cast<T*>(value);
                    return map.try_emplace(KeyFromAtom<typename T::key_type>(key)).second;
                }
                else
                {
                    return false;
                }
            }

            bool eraseKey(void* value, const Core::StringAtom& key) const override
            {
                if constexpr (kind == TypeKind::Map)
                {
                    auto& map = *static_cast<T*>(value);
                    return map.erase(KeyFromAtom<typename T::key_type>(key)) != 0;
                }
                else
                {
                    return false;
                }
            }

        private:
            static constexpr bool CanEmplace = [] {
                if constexpr (kind == TypeKind::Optional)
                {
                    return std::is_default_constructible_v<typename T::value_type>;
                }
                else
                {
                    return false;
                }
            }();

            static constexpr bool CanMove = [] {
                if constexpr (kind == TypeKind::Sequence)
                {
                    return std::is_swappable_v<typename T::value_type>;
                }
                else
                {
                    return false;
                }
            }();

            static constexpr bool CanInsert = [] {
                if constexpr (kind == TypeKind::Map)
                {
                    return std::is_default_constructible_v<typename T::mapped_type>;
                }
                else
                {
                    return false;
                }
            }();

            static constexpr bool IsResizable = [] {
                if constexpr (IsSpecializationOf<T, std::vector>::value)
                {
                    return std::is_default_constructible_v<typename T::value_type>;
                }
                else
                {
                    return false;
                }
            }();

            template<class E>
            [[nodiscard]] static std::uint64_t ToBits(E value) noexcept
            {
                return static_cast<std::uint64_t>(static_cast<std::underlying_type_t<E>>(value));
            }

            std::vector<EnumEntry> _entries;
        };
    } // namespace Internal

    /// The type info of `T` (cv-qualifiers ignored), created on first use.
    template<class T>
    const TypeInfo& TypeInfoOf()
    {
        if constexpr (!std::is_same_v<T, std::remove_cv_t<T>>)
        {
            return TypeInfoOf<std::remove_cv_t<T>>();
        }
        else
        {
            static const Internal::TypeInfoImpl<T> info;
            return info;
        }
    }
} // namespace NX::Reflection
