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

#include "Core/Delegate.h"
#include "HorizontalLayout.h"
#include "Label.h"
#include "NxFundamental/Reflection/TypeDescriptor.h"
#include "VerticalLayout.h"

#include <any>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <vector>

// A property grid shows every FIELD() of a reflected object, built from its runtime descriptor
// (NX::Reflection::Describe / BaseComponent::reflect), with no per-type UI code.
//
// - Rows never hold pointers into the target. Each row has a PropertyBinding that resolves the
//   value when it is needed; a target that disappears empties the grid.
// - Every frame each visible row copies the model into its widgets ("pull"). The model is
//   written only from a widget's edit delegate, through the field's setter (R::Meta(Setter =
//   ...)) or a direct assignment, followed by the owner's onFieldEdited and onEdited here.
// - Nested values (struct fields, container elements, optional values) are edited through a
//   copy of the top-level field, which is then assigned as a whole, so the field's setter and
//   notification run for any edit inside it.
// - The tree depends only on the target's type, the field metadata and the grid settings, and
//   is rebuilt only when one of them changes. Row ids come from field names, so ImGui keeps
//   focus and open sections across rebuilds.
//
// Field metadata understood:
// - R::Meta(DisplayName = "..."), R::Meta(Hidden), R::Meta(ReadOnly);
// - R::Meta(Category = "...") puts the row into a named section after the class sections;
// - R::Meta(EnabledIf = boolField) disables the row while that field of the object is false;
// - R::Meta(Editor = Name) picks a named editor, e.g. AssetRef with R::Meta(AssetType = kind,
//   DependsOn = field) for a choice of names from PropertyChoices;
// - R::Meta(Flags) on an enum; R::Min / R::Max / R::Step on numbers.
// Classes the metadata cannot describe get a PropertyCustomization (PropertyCustomization.h).

namespace NX::Gui
{
    class ClassCustomization;
    class PropertyChoices;
    class PropertyCustomizations;
    class PropertyGrid;
    class PropertyRow;

    /// "_minLightStrength" -> "Min light strength", "HTTPServer" -> "HTTP server".
    [[nodiscard]] Core::StringAtom LabelFromFieldName(std::string_view name);

    /// Where a value lives inside the grid's target and how to change it.
    class PropertyBinding
    {
    public:
        /// The value's address, or null when it is not reachable now.
        using Resolve = std::function<void*()>;
        /// Replaces the whole value with `*value` (of exactly `info().type()`).
        using Write = std::function<void(const void* value)>;

        PropertyBinding(const Reflection::TypeInfo& info, Resolve resolve, Write write);

        [[nodiscard]] const Reflection::TypeInfo& info() const noexcept { return *_info; }
        [[nodiscard]] void* resolve() const { return _resolve ? _resolve() : nullptr; }
        [[nodiscard]] bool isEditable() const noexcept { return static_cast<bool>(_write); }

        /// Typed read; null when `T` is not the value type or the value is not reachable.
        template<class T>
        [[nodiscard]] const T* get() const
        {
            return _info->type() == typeid(T) ? static_cast<const T*>(resolve()) : nullptr;
        }

        /// Typed write; false when `T` is not the value type or the binding is read-only.
        template<class T>
        bool set(const T& value) const
        {
            if (_info->type() != typeid(T) || !_write)
            {
                return false;
            }
            _write(&value);
            return true;
        }

        /// Copies the value, lets `mutate` change the copy, and writes the copy back.
        bool edit(const std::function<void(void* copy)>& mutate) const;

        /// Field `property` (declared in `owner`, a class of `root`'s chain) of this class value.
        [[nodiscard]] PropertyBinding field(const Reflection::ClassDesc& root,
                                            const Reflection::ClassDesc& owner,
                                            const Reflection::PropertyDesc& property) const;
        /// Element `index` of this sequence.
        [[nodiscard]] PropertyBinding element(std::size_t index) const;
        /// The value of this optional.
        [[nodiscard]] PropertyBinding optionalValue() const;
        /// The value stored for `key` in this map.
        [[nodiscard]] PropertyBinding mapValue(const Core::StringAtom& key) const;

    private:
        [[nodiscard]] bool canEditChildren() const noexcept
        {
            return isEditable() && _info->isCopyable();
        }

        const Reflection::TypeInfo* _info;
        Resolve _resolve;
        Write _write;
    };

    /// The object a value belongs to, as seen from one of its rows.
    struct PropertyScope
    {
        /// The object that declares the field (for container elements: the container's
        /// owner), or null when it is not reachable now.
        std::function<void*()> owner;
        /// Another field of the same object, by declared name.
        std::function<std::optional<PropertyBinding>(const Core::StringAtom& field)> sibling;
    };

    /// What a property editor factory works with.
    struct PropertyEditorContext
    {
        /// Add the value widget(s) here, after the label; set the row's pull.
        PropertyRow& row;
        const PropertyBinding& binding;
        /// Field metadata; null for container elements, optional and map values.
        const RClassField* field = nullptr;
        bool readOnly = false;
        /// Choice lists for named choice editors (AssetRef). Never null inside a grid.
        const PropertyChoices* choices = nullptr;
        /// Never null inside a grid.
        const PropertyScope* scope = nullptr;
    };

    /// C++ type -> row editor. A type without an entry falls back to the structural editors
    /// (enum, optional, sequence, map, nested struct), then to a read-only "(no editor)" row.
    class PropertyEditors
    {
    public:
        using Factory = std::function<void(PropertyEditorContext& context)>;

        struct Entry
        {
            /// Shown in PropertyGrid::dump.
            Core::StringAtom name;
            Factory factory;
        };

        /// The editors for engine types (numbers, bool, strings, paths, glm vectors, colours).
        [[nodiscard]] static const PropertyEditors& Builtin();

        template<class T>
        void add(Core::StringAtom name, Factory factory)
        {
            add(typeid(T), std::move(name), std::move(factory));
        }

        /// Adds or replaces the editor of `type`.
        void add(std::type_index type, Core::StringAtom name, Factory factory);

        /// Adds or replaces an editor chosen by R::Meta(Editor = name) instead of by type.
        void addNamed(Core::StringAtom name, Factory factory);

        [[nodiscard]] const Entry* find(std::type_index type) const;
        [[nodiscard]] const Entry* findNamed(const Core::StringAtom& name) const;

    private:
        // Looked up only, never iterated: hash order cannot leak into the tree.
        std::unordered_map<std::type_index, Entry> _entries;
        std::unordered_map<Core::StringAtom, Entry> _named;
    };

    /// One property: [label] [value widgets]. Its pull runs right before it is drawn, so rows
    /// in closed sections and container rows that are not shown cost nothing.
    CLASS();
    class PropertyRow : public HorizontalLayout
    {
        ECS_DECL_NO_CNSTR(PropertyRow, NX::Gui::HorizontalLayout);

    public:
        /// An empty `label` gives a row without a label column (container elements).
        explicit PropertyRow(const StringAtom& label = ""_atom, float labelWidth = 0.f);
        PropertyRow(const PropertyRow&) = delete;
        PropertyRow(PropertyRow&&) = delete;
        PropertyRow& operator=(const PropertyRow&) = delete;
        PropertyRow& operator=(PropertyRow&&) = delete;
        ~PropertyRow() override = default;

        [[nodiscard]] Label* getLabel() const noexcept { return _label; }

        /// Copies the model into the row's widgets.
        void setPull(std::function<void()> pull) { _pull = std::move(pull); }
        void pull() const;

        void setEditorName(Core::StringAtom name) { _editorName = std::move(name); }
        [[nodiscard]] const Core::StringAtom& getEditorName() const noexcept
        {
            return _editorName;
        }

        /// A read-only row is always disabled.
        void setReadOnly(bool value) noexcept;
        [[nodiscard]] bool isReadOnly() const noexcept { return _isReadOnly; }

        /// Disables the row while `condition()` is false (checked before every draw). Several
        /// conditions must all hold.
        void addEnabledIf(std::function<bool()> condition)
        {
            _enabledIf.push_back(std::move(condition));
        }

        /// Storage for a value the row shows but the target does not hold (computed rows).
        [[nodiscard]] std::any& valueCache() noexcept { return _valueCache; }

    protected:
        void preDraw() override;
        void onDraw() override;

    protected:
        Label* _label = nullptr;
        std::function<void()> _pull;
        std::vector<std::function<bool()>> _enabledIf;
        std::any _valueCache;
        Core::StringAtom _editorName;
        bool _isReadOnly = false;
    };

    enum class PropertySections : std::uint8_t
    {
        /// One collapsing section per class of the target's chain (bases first).
        PerClass,
        /// All rows directly in the grid.
        Flat,
    };

    CLASS();
    class PropertyGrid : public VerticalLayout
    {
        ECS_DECL_NO_CNSTR(PropertyGrid, NX::Gui::VerticalLayout);

    public:
        /// Called every frame; an empty ObjectRef means "no target".
        using TargetResolver = std::function<Reflection::ObjectRef()>;

        explicit PropertyGrid(const StringAtom& name = ""_atom);
        PropertyGrid(const PropertyGrid&) = delete;
        PropertyGrid(PropertyGrid&&) = delete;
        PropertyGrid& operator=(const PropertyGrid&) = delete;
        PropertyGrid& operator=(PropertyGrid&&) = delete;
        ~PropertyGrid() override = default;

        /// A target of the same type as the current one is rebound without a rebuild.
        void setTarget(TargetResolver resolver);
        /// Shows `component` while it is alive (held weakly).
        void setTarget(BaseComponent& component);
        void clearTarget();

        void setSections(PropertySections sections);
        [[nodiscard]] PropertySections getSections() const noexcept { return _sections; }

        void setReadOnly(bool value);
        [[nodiscard]] bool isReadOnly() const noexcept { return _isReadOnly; }

        /// `editors` must outlive the grid. Default: PropertyEditors::Builtin().
        void setEditors(const PropertyEditors& editors);

        /// `customizations` must outlive the grid. Default: PropertyCustomizations::Global().
        void setCustomizations(const PropertyCustomizations& customizations);

        /// `choices` must outlive the grid. Default: PropertyChoices::Global().
        void setChoices(const PropertyChoices& choices);

        void setLabelWidth(float width);
        [[nodiscard]] float getLabelWidth() const noexcept { return _labelWidth; }

        /// Rebuilds the rows on the next draw.
        void rebuild() noexcept { _isRebuildRequested = true; }

        /// The row of a property (by field name), searching nested sections too; null if none.
        [[nodiscard]] PropertyRow* findRow(const Core::StringAtom& property) const;

        /// The class the rows were built for, or null.
        [[nodiscard]] const Reflection::ClassDesc* getBuiltClass() const noexcept
        {
            return _builtFor;
        }

        /// A canonical text form of the tree (sections, rows, ids, editors), one node per line.
        [[nodiscard]] std::string dump() const;

    public: // Delegates
        /// After every edit made through the grid, with the edited top-level property (for a
        /// customization's virtual row: a descriptor carrying the row's name).
        Delegate<void(const Reflection::PropertyDesc&)>::Ptr onEdited
            = Delegate<void(const Reflection::PropertyDesc&)>::Create();

    protected:
        struct RowPlan;

        void onDraw() override;

        [[nodiscard]] Reflection::ObjectRef resolveTarget() const;
        void requestRebuildIfBuilt() noexcept;
        void clearRows();
        void build(const Reflection::ClassDesc& desc);
        void updateOwners(const Reflection::ObjectRef& target) const;
        [[nodiscard]] void* ownerOf(std::size_t classIndex) const;
        void writeProperty(std::size_t classIndex, const Reflection::PropertyDesc& property,
                           const void* value);
        void reportEdit(std::size_t classIndex, const Reflection::PropertyDesc& property,
                        void* owner);

        [[nodiscard]] std::vector<RowPlan> planClassRows(std::size_t classIndex) const;
        void buildPlannedRow(VerticalLayout& container, const RowPlan& plan,
                             const PropertyScope& scope);
        [[nodiscard]] PropertyBinding topBinding(std::size_t classIndex,
                                                 const Reflection::PropertyDesc& property,
                                                 bool readOnly);
        [[nodiscard]] PropertyScope topScope(std::size_t classIndex);
        PropertyRow* buildProperty(VerticalLayout& container,
                                   const Reflection::PropertyDesc& property,
                                   const Core::StringAtom& label, const PropertyBinding& binding,
                                   const PropertyScope& scope, int depth, bool readOnly);
        void buildClassRows(VerticalLayout& container, const PropertyBinding& binding,
                            const Reflection::ClassDesc& desc, int depth, bool readOnly);
        void buildValue(PropertyRow& row, const PropertyBinding& binding,
                        const RClassField* field, const PropertyScope& scope, int depth,
                        bool readOnly);
        void applyEnabledIf(PropertyRow& row, const RClassField* field,
                            const PropertyScope& scope) const;
        void buildEnum(PropertyEditorContext& context);
        void buildOptional(PropertyEditorContext& context, int depth);
        void buildSequence(PropertyEditorContext& context, int depth);
        void buildMap(PropertyEditorContext& context, int depth);
        void buildNestedClass(PropertyEditorContext& context, int depth);
        void buildFallback(PropertyEditorContext& context);

    protected:
        TargetResolver _resolver;
        const PropertyEditors* _editors = nullptr;
        const PropertyCustomizations* _customizations = nullptr;
        const PropertyChoices* _choices = nullptr;
        PropertySections _sections = PropertySections::PerClass;
        float _labelWidth = 0.f;
        bool _isReadOnly = false;
        bool _isRebuildRequested = false;

        const Reflection::ClassDesc* _builtFor = nullptr;
        /// The built class chain (bases first) and the customization of each of its classes.
        std::vector<const Reflection::ClassDesc*> _chain;
        std::vector<const ClassCustomization*> _chainCustomizations;
        /// Owner pointer of every class of the built chain, refreshed once per frame.
        mutable std::vector<void*> _owners;
        mutable int _ownersFrame = -1;
    };
} // namespace NX::Gui

#include "PropertyGrid.generated.h" // added by the code generator. Better don't move it.
