// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "PropertyCustomization.h"

#include <algorithm>

namespace NX::Gui
{
    // ===================== PropertyChoices =====================

    PropertyChoices& PropertyChoices::Global()
    {
        static PropertyChoices choices;
        return choices;
    }

    void PropertyChoices::add(Core::StringAtom kind, Provider provider)
    {
        _providers.insert_or_assign(std::move(kind), std::move(provider));
    }

    bool PropertyChoices::has(const Core::StringAtom& kind) const
    {
        return _providers.contains(kind);
    }

    std::vector<Core::StringAtom> PropertyChoices::get(const Core::StringAtom& kind,
                                                       const ChoiceRequest& request) const
    {
        const auto it = _providers.find(kind);
        return it != _providers.end() && it->second ? it->second(request)
                                                    : std::vector<Core::StringAtom>{};
    }

    // ===================== ClassCustomization =====================

    const ClassCustomization::RowRules* ClassCustomization::rules(std::string_view row) const
    {
        const auto it = _rules.find(row);
        return it != _rules.end() ? &it->second : nullptr;
    }

    void ClassCustomization::runAfterEdit(std::string_view row, void* owner) const
    {
        if (const auto* found = rules(row))
        {
            for (const auto& hook : found->afterEdit)
            {
                hook(owner);
            }
        }
    }

    void ClassCustomization::setSection(std::string_view title)
    {
        _section = Core::StringAtom(title);
    }

    ClassCustomization::RowRules& ClassCustomization::rulesFor(std::string_view row)
    {
        if (const auto it = _rules.find(row); it != _rules.end())
        {
            return it->second;
        }
        return _rules.emplace(std::string(row), RowRules{}).first->second;
    }

    void ClassCustomization::setOrder(std::initializer_list<std::string_view> rows)
    {
        _order.clear();
        for (const auto row : rows)
        {
            _order.push_back(Core::StringAtom::Intern(row));
        }
    }

    ClassCustomization::VirtualRow& ClassCustomization::addVirtualRow(
        RowKind kind, std::string_view name, std::string_view label, const RowOptions& options,
        const Reflection::TypeInfo& info, std::string_view typeName)
    {
        auto row = std::make_unique<VirtualRow>();
        row->kind = kind;
        row->name = Core::StringAtom::Intern(name);
        row->label = Core::StringAtom(label);
        if (!options.category.empty())
        {
            row->category = Core::StringAtom(options.category);
        }
        row->readOnly = options.readOnly || kind == RowKind::Computed;

        // A deque never moves its elements, so the views below stay valid.
        const auto keep = [&row](std::string_view text) -> std::string_view
        { return row->strings.emplace_back(text); };
        row->field.type = keep(typeName);
        row->field.name = keep(name);
        auto& attributes = row->field.attributes;
        if (options.min)
        {
            attributes.min = JRM::Number(*options.min);
        }
        if (options.max)
        {
            attributes.max = JRM::Number(*options.max);
        }
        if (options.step)
        {
            attributes.step = JRM::Number(*options.step);
        }
        for (const auto& [key, value] : options.meta)
        {
            attributes.meta.push_back({ .key = keep(key), .value = keep(value) });
        }

        row->desc.name = row->name;
        row->desc.field = &row->field;
        row->desc.type = info.type();
        row->desc.typeInfo = &info;
        row->desc.isConst = kind != RowKind::Property;
        row->desc.hasSetter = kind == RowKind::Property;

        return *_virtualRows.emplace_back(std::move(row));
    }

    // ===================== PropertyCustomizations =====================

    PropertyCustomizations& PropertyCustomizations::Global()
    {
        static PropertyCustomizations customizations;
        return customizations;
    }

    const ClassCustomization* PropertyCustomizations::find(std::type_index type) const
    {
        const auto it = _classes.find(type);
        return it != _classes.end() ? it->second.get() : nullptr;
    }

    ClassCustomization& PropertyCustomizations::get(std::type_index type)
    {
        auto& slot = _classes[type];
        if (!slot)
        {
            slot = std::make_unique<ClassCustomization>();
        }
        return *slot;
    }
} // namespace NX::Gui
