// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Delegate.h"
#include "Editor/GuiComponents/VerticalLayout.h"

#include <vector>

namespace NX
{
    namespace Gui
    {
        class Button;
        class ListView;
        class TextInput;
    } // namespace Gui

    class BindingsListWidget
    {
    public:
        void draw();

        void initialize();

        void setBindings(const std::vector<StringAtom>& bindings);
        void setCurrentIndex(std::size_t index);

        Core::Delegate<void(std::size_t)>::Ptr onSelect
            = Core::Delegate<void(std::size_t)>::Create();
        Core::Delegate<void()>::Ptr onAddBinding = Core::Delegate<void()>::Create();

    private:
        void initializeToolbar();
        void initializeBindingsList();

        void drawBindingsListWidget();

        void addBinding();
        void applySearchFilter(const char* text) const;

        void pushPanelStyle();
        void popPanelStyle();

    private:
        Gui::VerticalLayout _mainLayout;

        Gui::ListView* _bindingsList = nullptr;
        Gui::Button* _addNewBindingBtn = nullptr;
        Gui::TextInput* _searchTextInput = nullptr;

        Core::DelegateSubscriber _addBindingSubscription;
        Core::DelegateSubscriber _searchSubscription;
        Core::DelegateSubscriber _selectionSubscription;

        std::vector<StringAtom> _bindings;
        std::size_t _currentIndex = 0;
    };
} // namespace NX
