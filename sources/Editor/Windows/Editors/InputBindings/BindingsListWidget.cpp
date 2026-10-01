// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "BindingsListWidget.h"

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/List.h"
#include "Editor/IconsFontAwesome.h"
#include "ImGui/imgui_internal.h"
#include "NxWorld/Framework/GameInstance.h"

namespace NX
{

    void BindingsListWidget::draw()
    {
        drawBindingsListWidget();
    }

    void BindingsListWidget::initialize()
    {
        constexpr float panelPadding = 8.f;

        _mainLayout.setPaddings(panelPadding, panelPadding, panelPadding, panelPadding);
        _mainLayout.setFlex(Gui::Flex::FlexWidthAndHeight);

        initializeToolbar();
        initializeBindingsList();
    }

    void BindingsListWidget::initializeToolbar()
    {
        constexpr float rowHeight = 40.f;
        constexpr float addButtonWidth = 155.f;

        auto* h = _mainLayout.addChildComponent<Gui::HorizontalLayout>();
        h->setHeight(rowHeight);
        h->setPaddings(0.f, 0.f, 0.f, 0.f);

        _searchTextInput = h->addChildComponent<Gui::TextInput>();
        _searchTextInput->setPlaceholder("Search..."_atom);
        _searchTextInput->setFlex(Gui::Flex::FlexWidth);
        _searchTextInput->setHeight(rowHeight);

        _addNewBindingBtn = h->addChildComponent<Gui::Button>();
        _addNewBindingBtn->setText(ICON_FA_PLUS "  Add binding"_atom);
        _addNewBindingBtn->setWidth(addButtonWidth);
        _addNewBindingBtn->setHeight(rowHeight);
        _addNewBindingBtn->setButtonColor(Color4(82, 48, 159, 255));
        _addNewBindingBtn->setButtonHoverColor(Color4(104, 63, 197, 255));
        _addNewBindingBtn->setButtonActiveColor(Color4(65, 38, 130, 255));
        _addNewBindingBtn->setBorderColor(Color4(128, 88, 220, 255));
        _addNewBindingBtn->setBorderWidth(1.f);
        _addNewBindingBtn->setBorderRound(6.f);

        _addBindingSubscription
            = _addNewBindingBtn->onClick->subscribeAndGetID([this] { addBinding(); });

        _searchSubscription = _searchTextInput->onInput->subscribeAndGetID(
            [this](const char* text) { applySearchFilter(text); });
    }

    void BindingsListWidget::initializeBindingsList()
    {
        _bindingsList = _mainLayout.addChildComponent<Gui::ListView>();
        _bindingsList->setFlex(Gui::Flex::FlexWidthAndHeight);
        _bindingsList->setData(_bindings);
    }

    void BindingsListWidget::drawBindingsListWidget()
    {
        pushPanelStyle();
        if (ImGui::BeginChild("BindingsListWidget", { 420.f, 0.f }, ImGuiChildFlags_Border))
        {
            _mainLayout.tick(GetWorld()->getTimeDelta());
        }
        ImGui::EndChild();
        popPanelStyle();
    }

    void BindingsListWidget::addBinding()
    {
        const auto nextIndex = _bindings.size() + 1;
        _bindings.emplace_back("New binding {}"_f << nextIndex);

        if (_bindingsList)
        {
            _bindingsList->setData(_bindings);
            _bindingsList->setCurrentIndex(_bindings.size() - 1);
        }
    }

    void BindingsListWidget::applySearchFilter(const char* text) const
    {
        if (!_bindingsList)
        {
            return;
        }

        StringAtom filter = text ? text : "";
        filter.toLowerCase();

        if (filter.isEmpty())
        {
            _bindingsList->resetRegexFilter();
            return;
        }

        _bindingsList->setRegexFilter("(?i){}"_f << filter);
    }

    void BindingsListWidget::pushPanelStyle()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 10.f, 10.f });

        ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.055f, 0.062f, 0.090f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_Border, { 0.18f, 0.20f, 0.29f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.075f, 0.085f, 0.125f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, { 0.095f, 0.105f, 0.155f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, { 0.105f, 0.115f, 0.175f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, { 0.55f, 0.56f, 0.72f, 1.f });
    }

    void BindingsListWidget::popPanelStyle()
    {
        ImGui::PopStyleColor(6);
        ImGui::PopStyleVar(5);
    }
} // namespace NX
