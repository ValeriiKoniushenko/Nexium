// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Editor

#pragma once

#include "PropertyGrid.h"

#include <any>
#include <cstdint>
#include <deque>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

// The part of a property panel that FIELD() cannot describe, registered once per class:
//
//     customizations.For<NX::SceneObj::Rectangle>()
//         .section("Texture")
//         .property<bool>("blending", "Blending", getter, setter, { .category = "Rendering" })
//         .afterEdit("_atlasName", [](NX::SceneObj::Rectangle& r) { ... });
//
// It applies wherever the class appears in a target's chain. Field names are the declared names
// ("_atlasName"); a virtual row's name is the id given here and also its id in the grid.
//
// Sections: every class of the chain has a section titled `section(...)` or after the class
// name, and sections with the same title are merged. Rows with a category (R::Meta(Category =
// "..."), `category(...)` or RowOptions::category) go to sections after the class sections,
// in the order the categories first appear, or into the class section of that title.
// Within a class: fields in declaration order, then virtual rows in registration order; rows
// listed in `order(...)` come first, in that order.

namespace NX::Gui
{
    /// What a choice provider is asked for.
    struct ChoiceRequest
    {
        /// The object that owns the edited value (the field's class), or null.
        const void* owner = nullptr;
        /// Current text of the field named by R::Meta(DependsOn = ...); empty without one.
        Core::StringAtom dependency;
    };

    /// Lists of strings by kind, for rows edited with R::Meta(Editor = AssetRef, AssetType = kind).
    class PropertyChoices
    {
    public:
        using Provider = std::function<std::vector<Core::StringAtom>(const ChoiceRequest& request)>;

        /// The registry property grids use unless told otherwise.
        [[nodiscard]] static PropertyChoices& Global();

        /// Adds or replaces the provider of `kind`.
        void add(Core::StringAtom kind, Provider provider);
        [[nodiscard]] bool has(const Core::StringAtom& kind) const;
        /// The options for `kind`; empty when no provider is registered.
        [[nodiscard]] std::vector<Core::StringAtom> get(const Core::StringAtom& kind,
                                                        const ChoiceRequest& request) const;

    private:
        // Looked up only, never iterated.
        std::unordered_map<Core::StringAtom, Provider> _providers;
    };

    /// Options of a virtual row.
    struct RowOptions
    {
        // Every member has a default, so designated initializers may omit any of them.
        /// Section of the row; empty: the class's section.
        std::string_view category{};
        /// Entries the editors read, as R::Meta would give them: {"Editor", "AssetRef"}, ...
        std::vector<std::pair<std::string_view, std::string_view>> meta{};
        std::optional<double> min{};
        std::optional<double> max{};
        std::optional<double> step{};
        bool readOnly = false;
    };

    /// What a custom row's builder gets. It is valid only while the builder runs: callbacks must
    /// copy `owner` and `edited`, not capture the context.
    struct CustomRowContext
    {
        PropertyRow& row;
        /// The customized class's object now, or null when the target is gone.
        std::function<void*()> owner;
        /// Reports an edit made by the row: after-edit hooks, then PropertyGrid::onEdited.
        std::function<void()> edited;
        bool readOnly = false;
    };

    /// Everything registered for one class (type-erased; use `Customize<T>` to fill it).
    class ClassCustomization
    {
    public:
        enum class RowKind : std::uint8_t
        {
            /// Read-only value computed from the object.
            Computed,
            /// A value with a getter and a setter.
            Property,
            /// Widgets built by a callback.
            Custom,
        };

        /// A row that is not a field.
        struct VirtualRow
        {
            RowKind kind = RowKind::Computed;
            Core::StringAtom name;
            Core::StringAtom label;
            std::optional<Core::StringAtom> category;
            bool readOnly = false;
            /// Backing storage of the string views in `field`.
            std::deque<std::string> strings;
            /// Metadata the editors read (limits, R::Meta-like entries).
            RClassField field;
            /// Identity of the row in PropertyGrid::onEdited.
            Reflection::PropertyDesc desc;
            std::function<void(void* owner, std::any& value)> read;
            std::function<void(void* owner, const void* value)> write;
            std::function<void(CustomRowContext& context)> build;
        };

        struct RowRules
        {
            std::optional<Core::StringAtom> label;
            std::optional<Core::StringAtom> category;
            bool hidden = false;
            bool readOnly = false;
            std::function<bool(const void* owner)> enabledIf;
            std::vector<std::function<void(void* owner)>> afterEdit;
        };

        [[nodiscard]] const std::optional<Core::StringAtom>& sectionTitle() const noexcept
        {
            return _section;
        }
        [[nodiscard]] const RowRules* rules(std::string_view row) const;
        [[nodiscard]] const std::vector<std::unique_ptr<VirtualRow>>& virtualRows() const noexcept
        {
            return _virtualRows;
        }
        [[nodiscard]] const std::vector<Core::StringAtom>& order() const noexcept
        {
            return _order;
        }
        void runAfterEdit(std::string_view row, void* owner) const;

        void setSection(std::string_view title);
        RowRules& rulesFor(std::string_view row);
        void setOrder(std::initializer_list<std::string_view> rows);
        VirtualRow& addVirtualRow(RowKind kind, std::string_view name, std::string_view label,
                                  const RowOptions& options, const Reflection::TypeInfo& info,
                                  std::string_view typeName);

    private:
        std::optional<Core::StringAtom> _section;
        std::map<std::string, RowRules, std::less<>> _rules;
        std::vector<std::unique_ptr<VirtualRow>> _virtualRows;
        std::vector<Core::StringAtom> _order;
    };

    /// Typed front end of `ClassCustomization` for class `T`.
    template<class T>
    class Customize
    {
    public:
        explicit Customize(ClassCustomization& target) noexcept
            : _target(&target)
        {
        }

        /// Title of the class's section; classes with the same title share one section.
        Customize& section(std::string_view title)
        {
            _target->setSection(title);
            return *this;
        }

        Customize& hide(std::string_view field)
        {
            _target->rulesFor(field).hidden = true;
            return *this;
        }

        Customize& readOnly(std::string_view row)
        {
            _target->rulesFor(row).readOnly = true;
            return *this;
        }

        Customize& label(std::string_view row, std::string_view text)
        {
            _target->rulesFor(row).label = Core::StringAtom(text);
            return *this;
        }

        Customize& category(std::string_view row, std::string_view section)
        {
            _target->rulesFor(row).category = Core::StringAtom(section);
            return *this;
        }

        /// Rows listed here come first, in this order.
        Customize& order(std::initializer_list<std::string_view> rows)
        {
            _target->setOrder(rows);
            return *this;
        }

        /// The row is disabled while `predicate(const T&)` is false.
        template<class Predicate>
        Customize& enabledIf(std::string_view row, Predicate predicate)
        {
            _target->rulesFor(row).enabledIf = [predicate](const void* owner)
            { return static_cast<bool>(predicate(*static_cast<const T*>(owner))); };
            return *this;
        }

        /// `hook(T&)` runs after an edit of `row` was written, before PropertyGrid::onEdited.
        template<class Hook>
        Customize& afterEdit(std::string_view row, Hook hook)
        {
            _target->rulesFor(row).afterEdit.emplace_back(
                [hook](void* owner) { hook(*static_cast<T*>(owner)); });
            return *this;
        }

        /// A read-only row showing `getter(T&)`, a `V`.
        template<class V, class Getter>
        Customize& computed(std::string_view name, std::string_view label, Getter getter,
                            const RowOptions& options = {})
        {
            auto& row = _target->addVirtualRow(ClassCustomization::RowKind::Computed, name, label,
                                               options, Reflection::TypeInfoOf<V>(),
                                               typeid(V).name());
            row.read = MakeReader<V>(std::move(getter));
            return *this;
        }

        /// An editable row reading `getter(T&)` and writing `setter(T&, const V&)`.
        template<class V, class Getter, class Setter>
        Customize& property(std::string_view name, std::string_view label, Getter getter,
                            Setter setter, const RowOptions& options = {})
        {
            auto& row = _target->addVirtualRow(ClassCustomization::RowKind::Property, name, label,
                                               options, Reflection::TypeInfoOf<V>(),
                                               typeid(V).name());
            row.read = MakeReader<V>(std::move(getter));
            row.write = [setter = std::move(setter)](void* owner, const void* value)
            { setter(*static_cast<T*>(owner), *static_cast<const V*>(value)); };
            return *this;
        }

        /// A row whose widgets `builder(CustomRowContext&)` creates (and pulls, through
        /// `context.row.setPull`).
        template<class Builder>
        Customize& customRow(std::string_view name, std::string_view label, Builder builder,
                             const RowOptions& options = {})
        {
            auto& row = _target->addVirtualRow(ClassCustomization::RowKind::Custom, name, label,
                                               options, Reflection::TypeInfoOf<std::monostate>(),
                                               "custom");
            row.build = std::move(builder);
            return *this;
        }

    private:
        template<class V, class Getter>
        [[nodiscard]] static auto MakeReader(Getter getter)
        {
            return [getter = std::move(getter)](void* owner, std::any& value)
            {
                V current = getter(*static_cast<T*>(owner));
                if (auto* held = std::any_cast<V>(&value))
                {
                    *held = std::move(current);
                }
                else
                {
                    value = std::move(current);
                }
            };
        }

        ClassCustomization* _target;
    };

    /// All class customizations, by class.
    class PropertyCustomizations
    {
    public:
        /// The registry property grids use unless told otherwise.
        [[nodiscard]] static PropertyCustomizations& Global();

        /// The customization of `T`, created on first use.
        template<class T>
        Customize<T> For()
        {
            return Customize<T>(get(typeid(T)));
        }

        [[nodiscard]] const ClassCustomization* find(std::type_index type) const;

    private:
        ClassCustomization& get(std::type_index type);

        // Looked up only, never iterated.
        std::unordered_map<std::type_index, std::unique_ptr<ClassCustomization>> _classes;
    };
} // namespace NX::Gui
