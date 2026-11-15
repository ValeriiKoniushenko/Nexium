// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "EnumInputs.h"

#include "InspectorStyle.h"

#include <algorithm>
#include <bit>
#include <format>
#include <utility>

namespace NX::Gui
{
    ECS_IMPL(EnumCombo);
    ECS_IMPL(FlagsEditor);
    ECS_IMPL(ChoiceCombo);

    // ===================== EnumCombo =====================

    void EnumCombo::setOptions(std::vector<EnumOption> options)
    {
        _options = std::move(options);
        updatePreview();
    }

    void EnumCombo::setValue(std::uint64_t value)
    {
        if (_value != value || _preview.empty())
        {
            _value = value;
            updatePreview();
        }
    }

    void EnumCombo::select(std::size_t index)
    {
        if (!Verify(index < _options.size()))
        {
            return;
        }
        _value = _options[index].value;
        updatePreview();
        onSelect->trigger(_value);
    }

    void EnumCombo::updatePreview()
    {
        const auto it = std::ranges::find(_options, _value, &EnumOption::value);
        _preview = it != _options.end() ? it->name.toStdString() : std::format("<{}>", _value);
    }

    void EnumCombo::onDraw()
    {
        ImGui::PushItemWidth(_size.x);
        if (ImGui::BeginCombo("", _preview.c_str()))
        {
            drawPopupContent();
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
    }

    void EnumCombo::drawPopupContent()
    {
        for (std::size_t i = 0; i < _options.size(); ++i)
        {
            const bool isSelected = _options[i].value == _value;
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::Selectable(_options[i].name.c_str(), isSelected))
            {
                select(i);
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
            ImGui::PopID();
        }
    }

    // ===================== FlagsEditor =====================

    void FlagsEditor::setOptions(std::vector<EnumOption> options)
    {
        _flags.clear();
        _emptyName = "None"_atom;
        for (auto& option : options)
        {
            if (option.value == 0)
            {
                _emptyName = option.name;
            }
            else if (std::has_single_bit(option.value))
            {
                _flags.push_back(std::move(option));
            }
        }
        updatePreview();
    }

    void FlagsEditor::setValue(std::uint64_t value)
    {
        if (_value != value || _preview.empty())
        {
            _value = value;
            updatePreview();
        }
    }

    void FlagsEditor::toggle(std::size_t index)
    {
        if (!Verify(index < _flags.size()))
        {
            return;
        }
        _value ^= _flags[index].value;
        updatePreview();
        onChange->trigger(_value);
    }

    void FlagsEditor::updatePreview()
    {
        if (_value == 0)
        {
            _preview = _emptyName.toStdString();
            return;
        }

        _preview.clear();
        std::uint64_t named = 0;
        for (const auto& flag : _flags)
        {
            if ((_value & flag.value) != 0)
            {
                if (!_preview.empty())
                {
                    _preview += " | ";
                }
                _preview += flag.name.toStdStringView();
                named |= flag.value;
            }
        }

        if (const std::uint64_t unnamed = _value & ~named; unnamed != 0)
        {
            if (!_preview.empty())
            {
                _preview += " | ";
            }
            _preview += std::format("0x{:x}", unnamed);
        }
    }

    void FlagsEditor::onDraw()
    {
        ImGui::PushItemWidth(_size.x);
        if (ImGui::BeginCombo("", _preview.c_str()))
        {
            drawPopupContent();
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
    }

    void FlagsEditor::drawPopupContent()
    {
        for (std::size_t i = 0; i < _flags.size(); ++i)
        {
            bool isSet = (_value & _flags[i].value) != 0;
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::Checkbox(_flags[i].name.c_str(), &isSet))
            {
                toggle(i);
            }
            ImGui::PopID();
        }
    }

    // ===================== ChoiceCombo =====================

    const std::vector<Core::StringAtom>& ChoiceCombo::refreshOptions()
    {
        _options = _provider ? _provider() : std::vector<Core::StringAtom>{};
        return _options;
    }

    void ChoiceCombo::setValue(Core::StringAtom value)
    {
        if (_value != value)
        {
            _value = std::move(value);
        }
    }

    std::string_view ChoiceCombo::getPreviewText() const noexcept
    {
        return _value.isEmpty() ? InspectorStyle::Default().emptyValueText
                                : _value.toStdStringView();
    }

    void ChoiceCombo::select(std::size_t index)
    {
        if (!Verify(index < _options.size()))
        {
            return;
        }
        _value = _options[index];
        onSelect->trigger(_value);
    }

    void ChoiceCombo::onDraw()
    {
        const std::string preview(getPreviewText());
        ImGui::PushItemWidth(_size.x);
        if (ImGui::BeginCombo("", preview.c_str()))
        {
            if (!_wasOpen)
            {
                refreshOptions();
                _wasOpen = true;
            }
            drawPopupContent();
            ImGui::EndCombo();
        }
        else
        {
            _wasOpen = false;
        }
        ImGui::PopItemWidth();
    }

    void ChoiceCombo::drawPopupContent()
    {
        for (std::size_t i = 0; i < _options.size(); ++i)
        {
            const bool isSelected = _options[i] == _value;
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::Selectable(_options[i].c_str(), isSelected))
            {
                select(i);
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
            ImGui::PopID();
        }
    }
} // namespace NX::Gui
