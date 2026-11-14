// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "PropertyGrid.h"

#include "CollapsingSection.h"
#include "ContainerEditors.h"
#include "EnumInputs.h"
#include "Input.h"
#include "InspectorStyle.h"

#include <cctype>
#include <spdlog/spdlog.h>
#include <format>
#include <tuple>
#include <unordered_set>
#include <utility>

namespace NX::Gui
{
    ECS_IMPL(PropertyRow);
    ECS_IMPL(PropertyGrid);

    namespace
    {
        using Reflection::ClassDesc;
        using Reflection::ObjectRef;
        using Reflection::PropertyDesc;
        using Reflection::TypeInfo;
        using Reflection::TypeKind;

        /// Nested structs deeper than this get a fallback row (guards recursive types).
        constexpr int MaxDepth = 8;

        /// The subscription lives as long as the widget that owns `delegate`, and so does the
        /// callback (and the binding it captures). Rows are owned by the grid, so a callback
        /// never outlives the grid it points to.
        template<class DelegatePtr, class F>
        void Listen(DelegatePtr& delegate, F&& callback)
        {
            std::ignore = delegate->subscribeAndGetID(std::forward<F>(callback));
        }

        void* OwnerInChain(const ClassDesc& root, const ClassDesc& owner, void* object)
        {
            if (&root == &owner || !object)
            {
                return object;
            }
            void* found = nullptr;
            root.forEachInChain(object, [&](const ClassDesc& cls, void* adjusted)
                                {
                                    if (&cls == &owner)
                                    {
                                        found = adjusted;
                                    }
                                });
            return found;
        }

        bool IsHidden(const PropertyDesc& property)
        {
            return property.field->attributes.hasMeta("Hidden");
        }

        Core::StringAtom LabelOf(const PropertyDesc& property)
        {
            if (const auto name = property.field->attributes.getMeta("DisplayName");
                name && !name->empty())
            {
                return Core::StringAtom(*name);
            }
            return LabelFromFieldName(property.name.toStdStringView());
        }

        void Line(std::string& out, int indent, std::string_view text)
        {
            out.append(static_cast<std::size_t>(indent) * 2, ' ');
            out += text;
            out += '\n';
        }

        void DumpChildren(const BaseComponent& node, int indent, std::string& out)
        {
            for (const auto& child : node.getChildren())
            {
                if (!child)
                {
                    continue;
                }

                if (const auto* section = child->tryCastTo<CollapsingSection>())
                {
                    std::string text = std::format("Section \"{}\"",
                                                   section->getTitle().toStdStringView());
                    if (const auto& id = section->getStableId())
                    {
                        text += std::format(" #{}", id->toStdStringView());
                    }
                    Line(out, indent, text);
                    DumpChildren(*section, indent + 1, out);
                }
                else if (const auto* row = child->tryCastTo<PropertyRow>())
                {
                    std::string text = "Row";
                    if (const auto* label = row->getLabel())
                    {
                        text += std::format(" \"{}\"", label->getText().toStdStringView());
                    }
                    if (const auto& id = row->getStableId())
                    {
                        text += std::format(" #{}", id->toStdStringView());
                    }
                    text += std::format(" {}", row->getEditorName().toStdStringView());
                    if (row->isDisabledWidget())
                    {
                        text += " readonly";
                    }
                    Line(out, indent, text);
                    DumpChildren(*row, indent + 1, out);
                }
                else if (const auto* array = child->tryCastTo<ArrayEditor>())
                {
                    const auto& rows = array->getElementRows();
                    for (std::size_t i = 0; i < rows.size(); ++i)
                    {
                        Line(out, indent, std::format("Element [{}]", i));
                        DumpChildren(*rows[i].row, indent + 1, out);
                    }
                }
                else if (const auto* map = child->tryCastTo<MapEditor>())
                {
                    for (const auto& entry : map->getEntryRows())
                    {
                        Line(out, indent, std::format("Entry \"{}\"", entry.key.toStdStringView()));
                        DumpChildren(*entry.row, indent + 1, out);
                    }
                }
                else
                {
                    DumpChildren(*child, indent, out);
                }
            }
        }

        const PropertyRow* FindRow(const BaseComponent& node, const Core::StringAtom& property)
        {
            for (const auto& child : node.getChildren())
            {
                if (!child)
                {
                    continue;
                }
                if (const auto* row = child->tryCastTo<PropertyRow>();
                    row && row->getStableId() == property)
                {
                    return row;
                }
                if (const auto* found = FindRow(*child, property))
                {
                    return found;
                }
            }
            return nullptr;
        }
    } // namespace

    // ===================== LabelFromFieldName =====================

    Core::StringAtom LabelFromFieldName(std::string_view name)
    {
        const auto isUpper = [](char c) { return std::isupper(static_cast<unsigned char>(c)) != 0; };
        const auto isLower = [](char c) { return std::islower(static_cast<unsigned char>(c)) != 0; };
        const auto isAlnum = [](char c) { return std::isalnum(static_cast<unsigned char>(c)) != 0; };

        // 1. Split into words: separators, lower->Upper, and the end of an acronym.
        std::vector<std::string> words;
        std::string current;
        const auto flush = [&]
        {
            if (!current.empty())
            {
                words.push_back(std::move(current));
                current.clear();
            }
        };

        for (std::size_t i = 0; i < name.size(); ++i)
        {
            const char c = name[i];
            if (!isAlnum(c))
            {
                flush();
                continue;
            }
            if (!current.empty() && isUpper(c))
            {
                const char prev = current.back();
                const bool nextIsLower = i + 1 < name.size() && isLower(name[i + 1]);
                // "helloWorld" -> hello|World, "HTTPServer" -> HTTP|Server
                if (isLower(prev) || (isUpper(prev) && nextIsLower))
                {
                    flush();
                }
            }
            current += c;
        }
        flush();

        // 2. Join in lower case, except acronyms (2+ letters, all upper case).
        std::string result;
        for (const std::string& word : words)
        {
            std::size_t letters = 0;
            bool allUpper = true;
            for (const char c : word)
            {
                if (std::isalpha(static_cast<unsigned char>(c)) != 0)
                {
                    ++letters;
                    allUpper = allUpper && isUpper(c);
                }
            }
            const bool isAcronym = allUpper && letters >= 2;

            if (!result.empty())
            {
                result += ' ';
            }
            for (const char c : word)
            {
                result += isAcronym ? c
                                    : static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
        }

        // 3. Capitalize the first letter.
        if (!result.empty())
        {
            result[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[0])));
        }
        return Core::StringAtom(std::string_view(result));
    }

    // ===================== PropertyBinding =====================

    PropertyBinding::PropertyBinding(const Reflection::TypeInfo& info, Resolve resolve, Write write)
        : _info(&info),
          _resolve(std::move(resolve)),
          _write(std::move(write))
    {
    }

    bool PropertyBinding::edit(const std::function<void(void* copy)>& mutate) const
    {
        void* current = resolve();
        if (!current || !canEditChildren())
        {
            return false;
        }
        std::any box = _info->copy(current);
        void* copy = _info->address(box);
        if (!copy)
        {
            return false;
        }
        mutate(copy);
        _write(copy);
        return true;
    }

    PropertyBinding PropertyBinding::field(const ClassDesc& root, const ClassDesc& owner,
                                           const PropertyDesc& property) const
    {
        Resolve resolve = [parent = *this, &root, &owner, &property]() -> void*
        {
            void* object = parent.resolve();
            void* adjusted = OwnerInChain(root, owner, object);
            return adjusted ? property.address(adjusted) : nullptr;
        };

        Write write;
        if (canEditChildren() && property.isEditable())
        {
            write = [parent = *this, &root, &owner, &property](const void* value)
            {
                parent.edit(
                    [&](void* copy)
                    {
                        if (void* adjusted = OwnerInChain(root, owner, copy))
                        {
                            property.assign(property, adjusted, value);
                        }
                    });
            };
        }
        return { *property.typeInfo, std::move(resolve), std::move(write) };
    }

    PropertyBinding PropertyBinding::element(std::size_t index) const
    {
        const TypeInfo& info = *_info;
        const TypeInfo& elementInfo = *info.element();

        Resolve resolve = [parent = *this, &info, index]() -> void*
        {
            void* sequence = parent.resolve();
            return sequence ? info.at(sequence, index) : nullptr;
        };

        Write write;
        if (canEditChildren() && elementInfo.isCopyable())
        {
            write = [parent = *this, &info, &elementInfo, index](const void* value)
            {
                parent.edit(
                    [&](void* copy)
                    {
                        if (void* target = info.at(copy, index))
                        {
                            elementInfo.assign(target, value);
                        }
                    });
            };
        }
        return { elementInfo, std::move(resolve), std::move(write) };
    }

    PropertyBinding PropertyBinding::optionalValue() const
    {
        const TypeInfo& info = *_info;
        const TypeInfo& valueInfo = *info.element();

        Resolve resolve = [parent = *this, &info]() -> void*
        {
            void* optional = parent.resolve();
            return optional ? info.value(optional) : nullptr;
        };

        Write write;
        if (canEditChildren() && valueInfo.isCopyable())
        {
            write = [parent = *this, &info, &valueInfo](const void* value)
            {
                parent.edit(
                    [&](void* copy)
                    {
                        if (void* target = info.value(copy))
                        {
                            valueInfo.assign(target, value);
                        }
                    });
            };
        }
        return { valueInfo, std::move(resolve), std::move(write) };
    }

    PropertyBinding PropertyBinding::mapValue(const Core::StringAtom& key) const
    {
        const TypeInfo& info = *_info;
        const TypeInfo& valueInfo = *info.element();

        Resolve resolve = [parent = *this, &info, key]() -> void*
        {
            void* map = parent.resolve();
            return map ? info.find(map, key) : nullptr;
        };

        Write write;
        if (canEditChildren() && valueInfo.isCopyable())
        {
            write = [parent = *this, &info, &valueInfo, key](const void* value)
            {
                parent.edit(
                    [&](void* copy)
                    {
                        if (void* target = info.find(copy, key))
                        {
                            valueInfo.assign(target, value);
                        }
                    });
            };
        }
        return { valueInfo, std::move(resolve), std::move(write) };
    }

    // ===================== PropertyEditors =====================

    void PropertyEditors::add(std::type_index type, Core::StringAtom name, Factory factory)
    {
        _entries.insert_or_assign(type, Entry{ .name = std::move(name),
                                               .factory = std::move(factory) });
    }

    const PropertyEditors::Entry* PropertyEditors::find(std::type_index type) const
    {
        const auto it = _entries.find(type);
        return it != _entries.end() ? &it->second : nullptr;
    }

    // ===================== PropertyRow =====================

    PropertyRow::PropertyRow(const StringAtom& label, float labelWidth)
        : HorizontalLayout(componentType, label.isEmpty() ? "PropertyRow"_atom : label)
    {
        setFlex(Flex::FlexWidth);
        setVerticalAlign(Align::Center);
        setHorizontalAlign(Align::Left);

        if (!label.isEmpty())
        {
            _label = addChildComponent<Label>("Label"_atom);
            _label->setText(label);
            if (labelWidth > 0.f)
            {
                _label->setWidth(labelWidth);
            }
        }
    }

    void PropertyRow::pull() const
    {
        if (_pull)
        {
            _pull();
        }
    }

    void PropertyRow::onDraw()
    {
        pull();
        HorizontalLayout::onDraw();
    }

    // ===================== PropertyGrid =====================

    PropertyGrid::PropertyGrid(const StringAtom& name)
        : VerticalLayout(componentType, name.isEmpty() ? "PropertyGrid"_atom : name),
          _editors(&PropertyEditors::Builtin()),
          _labelWidth(InspectorStyle::Default().labelWidth)
    {
        setFlex(Flex::FlexWidth);
        setVerticalAlign(Align::Top);
        setHorizontalAlign(Align::Left);
        setStableChildIds(true);
    }

    void PropertyGrid::setTarget(TargetResolver resolver)
    {
        _resolver = std::move(resolver);
    }

    void PropertyGrid::setTarget(BaseComponent& component)
    {
        _resolver = [weak = Core::WeakPtr<BaseComponent>(&component)]() -> ObjectRef
        {
            if (auto alive = weak.tryLoad())
            {
                return alive->reflect();
            }
            return {};
        };
    }

    void PropertyGrid::clearTarget()
    {
        _resolver = nullptr;
    }

    void PropertyGrid::setSections(PropertySections sections)
    {
        _sections = sections;
        requestRebuildIfBuilt();
    }

    void PropertyGrid::setReadOnly(bool value)
    {
        _isReadOnly = value;
        requestRebuildIfBuilt();
    }

    void PropertyGrid::setEditors(const PropertyEditors& editors)
    {
        _editors = &editors;
        requestRebuildIfBuilt();
    }

    void PropertyGrid::setLabelWidth(float width)
    {
        _labelWidth = width;
        requestRebuildIfBuilt();
    }

    PropertyRow* PropertyGrid::findRow(const Core::StringAtom& property) const
    {
        // Rows are the grid's own children: finding one does not change the grid.
        return const_cast<PropertyRow*>(FindRow(*this, property));
    }

    std::string PropertyGrid::dump() const
    {
        std::string out;
        DumpChildren(*this, 0, out);
        return out;
    }

    void PropertyGrid::onDraw()
    {
        const ObjectRef target = resolveTarget();
        if (!target)
        {
            if (_builtFor || hasChildren())
            {
                clearRows();
            }
            return;
        }

        // Rebuilt here, before any row is drawn, so no row is destroyed while it is drawing.
        if (_isRebuildRequested || target.desc != _builtFor)
        {
            build(*target.desc);
        }
        updateOwners(target);

        VerticalLayout::onDraw();
    }

    ObjectRef PropertyGrid::resolveTarget() const
    {
        return _resolver ? _resolver() : ObjectRef{};
    }

    void PropertyGrid::requestRebuildIfBuilt() noexcept
    {
        if (_builtFor)
        {
            _isRebuildRequested = true;
        }
    }

    void PropertyGrid::clearRows()
    {
        removeAllChildren();
        _builtFor = nullptr;
        _owners.clear();
        _ownersFrame = -1;
        _isRebuildRequested = false;
    }

    void PropertyGrid::build(const ClassDesc& desc)
    {
        clearRows();
        _builtFor = &desc;

        std::vector<const ClassDesc*> chain;
        desc.forEachInChain(nullptr, [&chain](const ClassDesc& cls, void*)
                            { chain.push_back(&cls); });

        for (std::size_t classIndex = 0; classIndex < chain.size(); ++classIndex)
        {
            const ClassDesc& cls = *chain[classIndex];
            const bool hasVisible = std::ranges::any_of(
                cls.properties, [](const PropertyDesc& property) { return !IsHidden(property); });
            if (!hasVisible)
            {
                continue;
            }

            VerticalLayout* container = this;
            if (_sections == PropertySections::PerClass)
            {
                auto* section
                    = addChildComponent<CollapsingSection>(LabelFromFieldName(cls.shortName));
                section->setStableId(cls.shortName);
                section->setHeaderFlags(InspectorStyle::Default().sectionFlags);
                container = &section->getContent();
            }

            for (const PropertyDesc& property : cls.properties)
            {
                if (IsHidden(property))
                {
                    continue;
                }
                const bool readOnly
                    = _isReadOnly || property.field->attributes.hasMeta("ReadOnly");
                buildProperty(*container, property, topBinding(classIndex, property, readOnly), 0,
                              readOnly);
            }
        }
    }

    void PropertyGrid::updateOwners(const ObjectRef& target) const
    {
        _owners.clear();
        target.desc->forEachInChain(target.object, [this](const ClassDesc&, void* owner)
                                    { _owners.push_back(owner); });
        _ownersFrame = ImGui::GetCurrentContext() ? ImGui::GetFrameCount() : -1;
    }

    void* PropertyGrid::ownerOf(std::size_t classIndex) const
    {
        // Owners are cached for the frame they were resolved in; an event outside that frame
        // resolves the target again, so it never reaches a destroyed object.
        const int frame = ImGui::GetCurrentContext() ? ImGui::GetFrameCount() : -1;
        if (frame != _ownersFrame || frame == -1)
        {
            const ObjectRef target = resolveTarget();
            if (!target || target.desc != _builtFor)
            {
                return nullptr;
            }
            updateOwners(target);
        }
        return classIndex < _owners.size() ? _owners[classIndex] : nullptr;
    }

    void PropertyGrid::writeProperty(std::size_t classIndex, const PropertyDesc& property,
                                     const void* value)
    {
        void* owner = ownerOf(classIndex);
        if (!owner || !property.assign)
        {
            return;
        }
        property.assign(property, owner, value);
        onEdited->trigger(property);
    }

    PropertyBinding PropertyGrid::topBinding(std::size_t classIndex, const PropertyDesc& property,
                                             bool readOnly)
    {
        PropertyBinding::Resolve resolve = [this, classIndex, &property]() -> void*
        {
            void* owner = ownerOf(classIndex);
            return owner ? property.address(owner) : nullptr;
        };

        PropertyBinding::Write write;
        if (!readOnly && property.isEditable())
        {
            write = [this, classIndex, &property](const void* value)
            { writeProperty(classIndex, property, value); };
        }
        return { *property.typeInfo, std::move(resolve), std::move(write) };
    }

    void PropertyGrid::buildProperty(VerticalLayout& container, const PropertyDesc& property,
                                     const PropertyBinding& binding, int depth, bool readOnly)
    {
        const TypeInfo& info = binding.info();
        const bool asSection = info.kind() == TypeKind::Class && depth < MaxDepth
                               && !_editors->find(info.type());
        if (asSection)
        {
            auto* section = container.addChildComponent<CollapsingSection>(LabelOf(property));
            section->setStableId(property.name);
            section->setHeaderFlags(InspectorStyle::Default().sectionFlags);
            buildClassRows(section->getContent(), binding, *info.classDesc(), depth + 1,
                           readOnly || !binding.isEditable());
            return;
        }

        auto* row = container.addChildComponent<PropertyRow>(LabelOf(property), _labelWidth);
        row->setStableId(property.name);
        buildValue(*row, binding, property.field, depth, readOnly);
    }

    void PropertyGrid::buildClassRows(VerticalLayout& container, const PropertyBinding& binding,
                                      const ClassDesc& desc, int depth, bool readOnly)
    {
        desc.forEachInChain(nullptr,
                            [&](const ClassDesc& owner, void*)
                            {
                                for (const PropertyDesc& property : owner.properties)
                                {
                                    if (IsHidden(property))
                                    {
                                        continue;
                                    }
                                    const bool rowReadOnly
                                        = readOnly || property.field->attributes.hasMeta("ReadOnly");
                                    buildProperty(container, property,
                                                  binding.field(desc, owner, property), depth,
                                                  rowReadOnly);
                                }
                            });
    }

    void PropertyGrid::buildValue(PropertyRow& row, const PropertyBinding& binding,
                                  const RClassField* field, int depth, bool readOnly)
    {
        readOnly = readOnly || !binding.isEditable();
        PropertyEditorContext context{
            .row = row, .binding = binding, .field = field, .readOnly = readOnly
        };
        const TypeInfo& info = binding.info();

        if (const auto* entry = _editors->find(info.type()))
        {
            row.setEditorName(entry->name);
            entry->factory(context);
        }
        else if (info.kind() == TypeKind::Enum && !info.enumEntries().empty())
        {
            buildEnum(context);
        }
        else if (info.kind() == TypeKind::Optional)
        {
            buildOptional(context, depth);
        }
        else if (info.kind() == TypeKind::Sequence)
        {
            buildSequence(context, depth);
        }
        else if (info.kind() == TypeKind::Map)
        {
            buildMap(context, depth);
        }
        else if (info.kind() == TypeKind::Class && depth < MaxDepth)
        {
            buildNestedClass(context, depth);
        }
        else
        {
            buildFallback(context);
            readOnly = true;
        }

        if (readOnly)
        {
            row.disableWidget(true);
        }
    }

    void PropertyGrid::buildEnum(PropertyEditorContext& context)
    {
        const TypeInfo& info = context.binding.info();
        std::vector<EnumOption> options;
        for (const auto& entry : info.enumEntries())
        {
            options.push_back({ .name = entry.name, .value = entry.bits });
        }

        const auto mutateTo = [binding = context.binding, &info](std::uint64_t bits)
        { binding.edit([&](void* copy) { info.setEnumBits(copy, bits); }); };

        if (context.field && context.field->attributes.hasMeta("Flags"))
        {
            context.row.setEditorName("Flags"_atom);
            auto* flags = context.row.addChildComponent<FlagsEditor>();
            flags->setFlex(Flex::FlexWidth);
            flags->setOptions(std::move(options));
            Listen(flags->onChange, mutateTo);
            context.row.setPull(
                [flags, binding = context.binding, &info]
                {
                    if (const void* value = binding.resolve())
                    {
                        flags->setValue(info.enumBits(value));
                    }
                });
            return;
        }

        context.row.setEditorName("Enum"_atom);
        auto* combo = context.row.addChildComponent<EnumCombo>();
        combo->setFlex(Flex::FlexWidth);
        combo->setOptions(std::move(options));
        Listen(combo->onSelect, mutateTo);
        context.row.setPull(
            [combo, binding = context.binding, &info]
            {
                if (const void* value = binding.resolve())
                {
                    combo->setValue(info.enumBits(value));
                }
            });
    }

    void PropertyGrid::buildOptional(PropertyEditorContext& context, int depth)
    {
        const TypeInfo& info = context.binding.info();
        context.row.setEditorName("Optional"_atom);

        auto* optional = context.row.addChildComponent<OptionalEditor>();
        optional->setFlex(Flex::FlexWidth);
        auto* value = optional->setValueWidget<PropertyRow>();
        buildValue(*value, context.binding.optionalValue(), nullptr, depth + 1, context.readOnly);

        Listen(optional->onEngagedChange,
               [binding = context.binding, &info](bool engaged)
               {
                   binding.edit(
                       [&](void* copy)
                       {
                           if (engaged)
                           {
                               info.emplace(copy);
                           }
                           else
                           {
                               info.reset(copy);
                           }
                       });
               });
        context.row.setPull(
            [optional, binding = context.binding, &info]
            {
                if (const void* value = binding.resolve())
                {
                    optional->setEngaged(info.hasValue(value));
                }
            });
    }

    void PropertyGrid::buildSequence(PropertyEditorContext& context, int depth)
    {
        const TypeInfo& info = context.binding.info();
        context.row.setEditorName("Array"_atom);

        auto* array = context.row.addChildComponent<ArrayEditor>();
        array->setFlex(Flex::FlexWidth);
        array->setFixedSize(info.isFixedSize() || context.readOnly);
        array->setSizeProvider(
            [binding = context.binding, &info]() -> std::size_t
            {
                const void* sequence = binding.resolve();
                return sequence ? info.size(sequence) : 0;
            });
        array->setElementFactory(
            [this, binding = context.binding, depth, readOnly = context.readOnly](
                HorizontalLayout& row, std::size_t index)
            {
                auto* value = row.addChildComponent<PropertyRow>();
                value->setFlex(Flex::FlexWidth);
                buildValue(*value, binding.element(index), nullptr, depth + 1, readOnly);
            });

        Listen(array->onAdd, [binding = context.binding, &info]
               { binding.edit([&](void* copy) { info.append(copy); }); });
        Listen(array->onRemove,
               [binding = context.binding, &info](std::size_t index)
               { binding.edit([&](void* copy) { info.erase(copy, index); }); });
        Listen(array->onMove,
               [binding = context.binding, &info](std::size_t from, std::size_t to)
               { binding.edit([&](void* copy) { info.move(copy, from, to); }); });
    }

    void PropertyGrid::buildMap(PropertyEditorContext& context, int depth)
    {
        const TypeInfo& info = context.binding.info();
        context.row.setEditorName("Map"_atom);

        auto* map = context.row.addChildComponent<MapEditor>();
        map->setFlex(Flex::FlexWidth);
        map->setKeysProvider(
            [binding = context.binding, &info]() -> std::vector<Core::StringAtom>
            {
                const void* value = binding.resolve();
                return value ? info.keys(value) : std::vector<Core::StringAtom>{};
            });
        map->setElementFactory(
            [this, binding = context.binding, depth, readOnly = context.readOnly](
                HorizontalLayout& row, const Core::StringAtom& key)
            {
                auto* value = row.addChildComponent<PropertyRow>();
                value->setFlex(Flex::FlexWidth);
                buildValue(*value, binding.mapValue(key), nullptr, depth + 1, readOnly);
            });

        Listen(map->onAdd,
               [binding = context.binding, &info](const Core::StringAtom& key)
               { binding.edit([&](void* copy) { info.insert(copy, key); }); });
        Listen(map->onRemove,
               [binding = context.binding, &info](const Core::StringAtom& key)
               { binding.edit([&](void* copy) { info.eraseKey(copy, key); }); });
    }

    void PropertyGrid::buildNestedClass(PropertyEditorContext& context, int depth)
    {
        const ClassDesc& desc = *context.binding.info().classDesc();
        context.row.setEditorName("Struct"_atom);

        auto* section
            = context.row.addChildComponent<CollapsingSection>(LabelFromFieldName(desc.shortName));
        section->setFlex(Flex::FlexWidth);
        section->setHeaderFlags(InspectorStyle::Default().sectionFlags);
        buildClassRows(section->getContent(), context.binding, desc, depth + 1, context.readOnly);
    }

    void PropertyGrid::buildFallback(PropertyEditorContext& context)
    {
        const TypeInfo& info = context.binding.info();
        const std::string_view type
            = context.field ? context.field->type : std::string_view(info.type().name());

        static std::unordered_set<std::type_index> logged;
        if (logged.insert(info.type()).second)
        {
            getLogger()->warn("PropertyGrid: no editor for '{}'; the row is read-only", type);
        }

        context.row.setEditorName("Fallback"_atom);
        auto* text = context.row.addChildComponent<TextInput>();
        text->setFlex(Flex::FlexWidth);
        text->setReadOnly(true);
        text->setInputtedData(std::format("{} (no editor)", type));
    }
} // namespace NX::Gui
