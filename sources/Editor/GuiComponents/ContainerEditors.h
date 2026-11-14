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

#include "Button.h"
#include "CheckBox.h"
#include "Core/Delegate.h"
#include "HorizontalLayout.h"
#include "Input.h"
#include "InspectorStyle.h"
#include "Label.h"
#include "VerticalLayout.h"

#include <functional>
#include <optional>
#include <utility>
#include <vector>

// Container editors know nothing about the container they show. The caller supplies the
// shape (a size or a key list) and a factory that fills one row; the editor reports edits
// through delegates and never touches the data.
//
// Edits are reported after the editor has drawn all its rows, at most one per draw, so a
// handler may change the container (and with it the row count) safely. The rows are rebuilt
// on the next draw when the shape differs. A handler must not destroy the editor itself
// while it is being drawn; a property grid defers that kind of rebuild to the next frame.

namespace NX::Gui
{
    /// A checkbox ("has a value") and the value's editor. Without a value, the editor is
    /// replaced by `InspectorStyle::emptyValueText`.
    CLASS();
    class OptionalEditor : public HorizontalLayout
    {
        ECS_DECL_NO_CNSTR(OptionalEditor, NX::Gui::HorizontalLayout);

    public:
        explicit OptionalEditor(const StringAtom& name = ""_atom);
        OptionalEditor(const OptionalEditor&) = delete;
        OptionalEditor(OptionalEditor&&) = delete;
        OptionalEditor& operator=(const OptionalEditor&) = delete;
        OptionalEditor& operator=(OptionalEditor&&) = delete;
        ~OptionalEditor() override = default;

        /// Replaces the value editor.
        template<IsWidget W, class... Args>
        W* setValueWidget(Args&&... args)
        {
            if (_value)
            {
                removeChild(_value);
            }
            auto* widget = addChildComponent<W>(std::forward<Args>(args)...);
            widget->setFlex(Flex::FlexWidth);
            _value = widget;
            applyEngaged();
            return widget;
        }

        [[nodiscard]] Widget* getValueWidget() const noexcept { return _value; }
        [[nodiscard]] CheckBox* getToggle() const noexcept { return _toggle; }
        [[nodiscard]] Label* getEmptyLabel() const noexcept { return _empty; }

        /// Pull: shows whether the optional has a value, does not notify.
        void setEngaged(bool engaged);
        [[nodiscard]] bool isEngaged() const noexcept { return _engaged; }

    public: // Delegates
        /// The user asked to set (true) or reset (false) the value. The editor itself only
        /// changes when the next pull (`setEngaged`) says the model changed.
        Delegate<void(bool)>::Ptr onEngagedChange = Delegate<void(bool)>::Create();

    protected:
        void applyEngaged();

    protected:
        CheckBox* _toggle = nullptr;
        Label* _empty = nullptr;
        Widget* _value = nullptr;
        bool _engaged = false;
        DelegateSubscriberPoolGuard _subscriptions;
    };

    struct ArrayElementRow
    {
        HorizontalLayout* row = nullptr;
        Button* moveUp = nullptr;
        Button* moveDown = nullptr;
        Button* remove = nullptr;
    };

    /// An in-place editor for a sequence: a summary row with an add button, then one row per
    /// element ([index] [element editor] [up] [down] [remove]). Row ids are the element index,
    /// so a rebuilt row keeps its ImGui state.
    CLASS();
    class ArrayEditor : public VerticalLayout
    {
        ECS_DECL_NO_CNSTR(ArrayEditor, NX::Gui::VerticalLayout);

    public:
        using SizeProvider = std::function<std::size_t()>;
        /// Fills the middle of an element row.
        using ElementFactory = std::function<void(HorizontalLayout& row, std::size_t index)>;

    public:
        explicit ArrayEditor(const StringAtom& name = ""_atom);
        ArrayEditor(const ArrayEditor&) = delete;
        ArrayEditor(ArrayEditor&&) = delete;
        ArrayEditor& operator=(const ArrayEditor&) = delete;
        ArrayEditor& operator=(ArrayEditor&&) = delete;
        ~ArrayEditor() override = default;

        void setSizeProvider(SizeProvider provider) { _sizeProvider = std::move(provider); }
        void setElementFactory(ElementFactory factory) { _factory = std::move(factory); }

        /// For fixed-size containers (`std::array`): no add or remove, elements can still move.
        void setFixedSize(bool value);
        [[nodiscard]] bool isFixedSize() const noexcept { return _isFixedSize; }

        /// Rebuilds the element rows now. Drawing does it by itself when the size changes.
        void rebuild();

        [[nodiscard]] const std::vector<ArrayElementRow>& getElementRows() const noexcept
        {
            return _rows;
        }
        [[nodiscard]] Label* getSummaryLabel() const noexcept { return _summary; }
        [[nodiscard]] Button* getAddButton() const noexcept { return _add; }

    public: // Delegates
        Delegate<void()>::Ptr onAdd = Delegate<void()>::Create();
        Delegate<void(std::size_t)>::Ptr onRemove = Delegate<void(std::size_t)>::Create();
        Delegate<void(std::size_t, std::size_t)>::Ptr onMove
            = Delegate<void(std::size_t, std::size_t)>::Create();

    protected:
        struct PendingEdit
        {
            enum class Kind
            {
                Add,
                Remove,
                Move
            };

            Kind kind = Kind::Add;
            std::size_t index = 0;
            std::size_t to = 0;
        };

        void onDraw() override;
        void request(PendingEdit edit);
        void flushPendingEdit();
        void updateSummary();
        void applyFixedSize();

    protected:
        SizeProvider _sizeProvider;
        ElementFactory _factory;
        HorizontalLayout* _header = nullptr;
        Label* _summary = nullptr;
        Button* _add = nullptr;
        std::vector<ArrayElementRow> _rows;
        std::optional<PendingEdit> _pending;
        bool _isBuilt = false;
        bool _isFixedSize = false;
        DelegateSubscriberPoolGuard _headerSubscriptions;
        DelegateSubscriberPoolGuard _rowSubscriptions;
    };

    struct MapEntryRow
    {
        Core::StringAtom key;
        HorizontalLayout* row = nullptr;
        Button* remove = nullptr;
    };

    /// An editor for a keyed container: a row to add a key, then one row per key, sorted by
    /// key so the order never depends on hashing ([key] [value editor] [remove]). Row ids
    /// are the key, so a rebuilt row keeps its ImGui state.
    CLASS();
    class MapEditor : public VerticalLayout
    {
        ECS_DECL_NO_CNSTR(MapEditor, NX::Gui::VerticalLayout);

    public:
        using KeysProvider = std::function<std::vector<Core::StringAtom>()>;
        /// Fills the middle of an entry row.
        using ElementFactory
            = std::function<void(HorizontalLayout& row, const Core::StringAtom& key)>;

    public:
        explicit MapEditor(const StringAtom& name = ""_atom);
        MapEditor(const MapEditor&) = delete;
        MapEditor(MapEditor&&) = delete;
        MapEditor& operator=(const MapEditor&) = delete;
        MapEditor& operator=(MapEditor&&) = delete;
        ~MapEditor() override = default;

        void setKeysProvider(KeysProvider provider) { _keysProvider = std::move(provider); }
        void setElementFactory(ElementFactory factory) { _factory = std::move(factory); }

        /// Rebuilds the entry rows now. Drawing does it by itself when the key set changes.
        void rebuild();

        [[nodiscard]] const std::vector<MapEntryRow>& getEntryRows() const noexcept
        {
            return _rows;
        }
        [[nodiscard]] TextInput* getNewKeyInput() const noexcept { return _newKey; }
        [[nodiscard]] Button* getAddButton() const noexcept { return _add; }

    public: // Delegates
        Delegate<void(const Core::StringAtom&)>::Ptr onAdd
            = Delegate<void(const Core::StringAtom&)>::Create();
        Delegate<void(const Core::StringAtom&)>::Ptr onRemove
            = Delegate<void(const Core::StringAtom&)>::Create();

    protected:
        struct PendingEdit
        {
            bool isAdd = true;
            Core::StringAtom key;
        };

        void onDraw() override;
        void request(PendingEdit edit);
        void requestAddFromInput();
        void flushPendingEdit();
        [[nodiscard]] std::vector<Core::StringAtom> sortedKeys() const;
        [[nodiscard]] bool hasKey(std::string_view key) const;

    protected:
        KeysProvider _keysProvider;
        ElementFactory _factory;
        TextInput* _newKey = nullptr;
        Button* _add = nullptr;
        std::vector<MapEntryRow> _rows;
        std::optional<PendingEdit> _pending;
        bool _isBuilt = false;
        DelegateSubscriberPoolGuard _headerSubscriptions;
        DelegateSubscriberPoolGuard _rowSubscriptions;
    };
} // namespace NX::Gui

#include "ContainerEditors.generated.h" // added by the code generator. Better don't move it.
