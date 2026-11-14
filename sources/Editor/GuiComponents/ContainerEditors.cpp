// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "ContainerEditors.h"

#include "Editor/IconsFontAwesome.h"

#include <algorithm>
#include <format>
#include <string>

namespace NX::Gui
{
    ECS_IMPL(OptionalEditor);
    ECS_IMPL(ArrayEditor);
    ECS_IMPL(MapEditor);

    namespace
    {
        Core::StringAtom MakeText(std::string_view text)
        {
            return Core::StringAtom(text);
        }
    } // namespace

    // ===================== OptionalEditor =====================

    OptionalEditor::OptionalEditor(const StringAtom& name)
        : HorizontalLayout(componentType, name.isEmpty() ? "OptionalEditor"_atom : name)
    {
        setFlex(Flex::FlexWidth);
        setVerticalAlign(Align::Center);
        setHorizontalAlign(Align::Left);

        _toggle = addChildComponent<CheckBox>("HasValue"_atom);
        _empty = addChildComponent<Label>("Empty"_atom);
        _empty->setText(MakeText(InspectorStyle::Default().emptyValueText));

        _subscriptions << _toggle->onChange->subscribeAndGetID(
            [this](bool engaged) { onEngagedChange->trigger(engaged); });

        applyEngaged();
    }

    void OptionalEditor::setEngaged(bool engaged)
    {
        _engaged = engaged;
        _toggle->setValue(engaged);
        applyEngaged();
    }

    void OptionalEditor::applyEngaged()
    {
        _empty->setEnabled(!_engaged);
        if (_value)
        {
            _value->setEnabled(_engaged);
        }
    }

    // ===================== ArrayEditor =====================

    ArrayEditor::ArrayEditor(const StringAtom& name)
        : VerticalLayout(componentType, name.isEmpty() ? "ArrayEditor"_atom : name)
    {
        setFlex(Flex::FlexWidth);
        setVerticalAlign(Align::Top);
        setHorizontalAlign(Align::Left);
        setStableChildIds(true);

        _header = addChildComponent<HorizontalLayout>("Header"_atom);
        _summary = _header->addChildComponent<Label>("Summary"_atom);
        _add = _header->addChildComponent<Button>(ICON_FA_PLUS);
        _headerSubscriptions << _add->onClick->subscribeAndGetID(
            [this] { request({ .kind = PendingEdit::Kind::Add }); });

        updateSummary();
    }

    void ArrayEditor::setFixedSize(bool value)
    {
        _isFixedSize = value;
        applyFixedSize();
    }

    void ArrayEditor::rebuild()
    {
        for (const auto& row : _rows)
        {
            removeChild(row.row);
        }
        _rows.clear();
        _rowSubscriptions.clearAndReleaseAll();

        const std::size_t size = _sizeProvider ? _sizeProvider() : 0;
        _rows.reserve(size);
        for (std::size_t i = 0; i < size; ++i)
        {
            auto* row = addChildComponent<HorizontalLayout>("Element"_atom);
            auto* index = row->addChildComponent<Label>("Index"_atom);
            index->setText(MakeText(std::to_string(i)));
            index->setWidth(InspectorStyle::Default().indexWidth);

            if (_factory)
            {
                _factory(*row, i);
            }

            ArrayElementRow& element = _rows.emplace_back();
            element.row = row;
            element.moveUp = row->addChildComponent<Button>(ICON_FA_ARROW_UP);
            element.moveDown = row->addChildComponent<Button>(ICON_FA_ARROW_DOWN);
            element.remove = row->addChildComponent<Button>(ICON_FA_TRASH);
            element.moveUp->disableWidget(i == 0);
            element.moveDown->disableWidget(i + 1 == size);

            using Kind = PendingEdit::Kind;
            _rowSubscriptions << element.moveUp->onClick->subscribeAndGetID(
                [this, i] { request({ .kind = Kind::Move, .index = i, .to = i - 1 }); });
            _rowSubscriptions << element.moveDown->onClick->subscribeAndGetID(
                [this, i] { request({ .kind = Kind::Move, .index = i, .to = i + 1 }); });
            _rowSubscriptions << element.remove->onClick->subscribeAndGetID(
                [this, i] { request({ .kind = Kind::Remove, .index = i }); });
        }

        _isBuilt = true;
        updateSummary();
        applyFixedSize();
    }

    void ArrayEditor::onDraw()
    {
        const std::size_t size = _sizeProvider ? _sizeProvider() : 0;
        if (!_isBuilt || size != _rows.size())
        {
            rebuild();
        }

        VerticalLayout::onDraw();
        flushPendingEdit();
    }

    void ArrayEditor::request(PendingEdit edit)
    {
        // A move past either end is not an edit (`to` wraps around for "up" on row 0).
        if (edit.kind == PendingEdit::Kind::Move && edit.to >= _rows.size())
        {
            return;
        }
        // Indices of later requests in the same draw refer to the old layout: keep the first.
        if (!_pending)
        {
            _pending = edit;
        }
    }

    void ArrayEditor::flushPendingEdit()
    {
        if (!_pending)
        {
            return;
        }

        const PendingEdit edit = *_pending;
        _pending.reset();
        switch (edit.kind)
        {
            case PendingEdit::Kind::Add:
                onAdd->trigger();
                break;
            case PendingEdit::Kind::Remove:
                onRemove->trigger(edit.index);
                break;
            case PendingEdit::Kind::Move:
                onMove->trigger(edit.index, edit.to);
                break;
        }
    }

    void ArrayEditor::updateSummary()
    {
        const std::size_t size = _rows.size();
        _summary->setText(MakeText(std::format("{} {}", size, size == 1 ? "element" : "elements")));
    }

    void ArrayEditor::applyFixedSize()
    {
        _add->setEnabled(!_isFixedSize);
        for (const auto& row : _rows)
        {
            row.remove->setEnabled(!_isFixedSize);
        }
    }

    // ===================== MapEditor =====================

    MapEditor::MapEditor(const StringAtom& name)
        : VerticalLayout(componentType, name.isEmpty() ? "MapEditor"_atom : name)
    {
        setFlex(Flex::FlexWidth);
        setVerticalAlign(Align::Top);
        setHorizontalAlign(Align::Left);
        setStableChildIds(true);

        auto* header = addChildComponent<HorizontalLayout>("Header"_atom);
        _newKey = header->addChildComponent<TextInput>("NewKey"_atom);
        _newKey->setFlex(Flex::FlexWidth);
        _newKey->setPlaceholder("New key"_atom);
        _add = header->addChildComponent<Button>(ICON_FA_PLUS);
        _headerSubscriptions << _add->onClick->subscribeAndGetID([this] { requestAddFromInput(); });
    }

    void MapEditor::rebuild()
    {
        for (const auto& row : _rows)
        {
            removeChild(row.row);
        }
        _rows.clear();
        _rowSubscriptions.clearAndReleaseAll();

        auto keys = sortedKeys();
        _rows.reserve(keys.size());
        for (auto& key : keys)
        {
            auto* row = addChildComponent<HorizontalLayout>("Entry"_atom);
            row->setStableId(key);
            auto* keyLabel = row->addChildComponent<Label>("Key"_atom);
            keyLabel->setText(key);
            keyLabel->setWidth(InspectorStyle::Default().labelWidth);

            if (_factory)
            {
                _factory(*row, key);
            }

            MapEntryRow& entry = _rows.emplace_back();
            entry.row = row;
            entry.remove = row->addChildComponent<Button>(ICON_FA_TRASH);
            _rowSubscriptions << entry.remove->onClick->subscribeAndGetID(
                [this, key] { request({ .isAdd = false, .key = key }); });
            entry.key = std::move(key);
        }

        _isBuilt = true;
    }

    void MapEditor::onDraw()
    {
        if (!_isBuilt || !std::ranges::equal(sortedKeys(), _rows, {}, {}, &MapEntryRow::key))
        {
            rebuild();
        }

        VerticalLayout::onDraw();
        flushPendingEdit();
    }

    void MapEditor::request(PendingEdit edit)
    {
        if (!_pending)
        {
            _pending = std::move(edit);
        }
    }

    void MapEditor::requestAddFromInput()
    {
        const std::string& key = _newKey->getInputtedData();
        if (key.empty() || hasKey(key))
        {
            return;
        }

        request({ .isAdd = true, .key = MakeText(key) });
        _newKey->setInputtedData({});
    }

    void MapEditor::flushPendingEdit()
    {
        if (!_pending)
        {
            return;
        }

        const PendingEdit edit = std::move(*_pending);
        _pending.reset();
        if (edit.isAdd)
        {
            onAdd->trigger(edit.key);
        }
        else
        {
            onRemove->trigger(edit.key);
        }
    }

    std::vector<Core::StringAtom> MapEditor::sortedKeys() const
    {
        std::vector<Core::StringAtom> keys
            = _keysProvider ? _keysProvider() : std::vector<Core::StringAtom>{};
        std::ranges::sort(keys, {},
                          [](const Core::StringAtom& key) { return key.toStdStringView(); });
        return keys;
    }

    bool MapEditor::hasKey(std::string_view key) const
    {
        return std::ranges::any_of(sortedKeys(), [key](const Core::StringAtom& existing)
                                   { return existing == key; });
    }
} // namespace NX::Gui
