// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Delegate.h"
#include "Widget.h"

namespace Core::Gui
{
    CLASS();
    class BaseList : public Widget
    {
        ECS_DECL(BaseList, Core::Gui::Widget);

    public:
        [[nodiscard]] glm::vec2 getRealSize() const;

        [[nodiscard]] float getWidth() const override { return getRealSize().x; }
        [[nodiscard]] float getHeight() const override { return getRealSize().y; }

        void setWidth(float newWidth) override;

        void setHeight(float newHeight) override;

        void resetRegexFilter() { _filter.clear(); }
        void setRegexFilter(const StringAtom& filter) { _filter = filter; }
        [[nodiscard]] const StringAtom& getFilter() const noexcept { return _filter; }

        void resetCurrentIndex() noexcept { _currentIndex = 0; }
        [[nodiscard]] std::size_t getCurrentIndex() const noexcept { return _currentIndex; }

        void setKeyboardFocusAtStart() noexcept { _needsFocus = true; }
        [[nodiscard]] bool hasKeyboardFocusAtStart() noexcept { return _needsFocus; }
        void resetKeyboardFocusAtStart() noexcept { _needsFocus = false; }

        virtual void setCurrentIndex(std::size_t i) = 0;

        virtual void resetListNavigation();

    protected:
        void onInitialize() override;

    protected:
        FIELD();
        Core::StringAtom _filter;

        FIELD();
        glm::vec2 _size = glm::vec2(100.f, 150.f);

        FIELD();
        std::size_t _currentIndex = 0;

        bool _needsFocus = false;
    };

    CLASS();
    class ListView : public BaseList
    {
        ECS_DECL(ListView, Core::Gui::BaseList);

    public:
        void setData(std::vector<StringAtom>&& items) { _items = std::move(items); }
        void setData(const std::vector<StringAtom>& items) { _items = items; }
        const std::vector<StringAtom>& getData() const noexcept { return _items; }

        void setCurrentIndex(std::size_t i) override
        {
            _currentIndex = std::min(i, _items.size() - 1);
        }

        [[nodiscard]] StringAtom getCurrentData() const { return _items.at(_currentIndex); }

    public: // Delegates
        Delegate<void(StringAtom)>::Ptr onSelect = Delegate<void(StringAtom)>::Create();
        Delegate<void(StringAtom)>::Ptr onDoubleClickSelect = Delegate<void(StringAtom)>::Create();

    protected:
        void onDraw() override;

    protected:
        std::vector<StringAtom> _items;
    };

    CLASS();
    class ListModelBased : public BaseList
    {
        ECS_DECL(ListModelBased, Core::Gui::BaseList);

    public:
        /// Set your own data provider. Just a function that will do a few things:
        /// 1. Get index of element, that want to be processed(by you)
        /// 2. Fill up second argument StringAtom with some string. It will be shown for user
        /// 3. Return a pointer to your data. It will be used only by you in the future. If you want
        /// to get 'an index' to your data, you can just fetch current pointer(index), and get the
        /// pointer to your data.
        void setDataProvider(const std::function<const void*(std::size_t, StringAtom&)>& callback);

        /// Set your own data provider. Just a function that will do a few things:
        /// 1. Get index of element, that want to be processed(by you)
        /// 2. Fill up second argument StringAtom with some string. It will be shown for user
        /// 3. Return a pointer to your data. It will be used only by you in the future. If you want
        /// to get 'an index' to your data, you can just fetch current pointer(index), and get the
        /// pointer to your data.
        void setSizeProvider(const std::function<std::size_t()>& callback);

        void setCurrentIndex(std::size_t i) override;

        [[nodiscard]] const void* getCurrentData() const noexcept { return _currentData; }

        [[nodiscard]] StringAtom tryGetCurrentDataAsString() const;

        void resetListNavigation() override;

    public: // Delegates
        Delegate<void(const void*, StringAtom)>::Ptr onSelect
            = Delegate<void(const void*, StringAtom)>::Create();
        Delegate<void(const void*, StringAtom)>::Ptr onDoubleClickSelect
            = Delegate<void(const void*, StringAtom)>::Create();

    protected:
        void onDraw() override;

    protected:
        std::function<const void*(std::size_t, StringAtom&)> _dataProvider;
        std::function<std::size_t()> _sizeProvider;
        std::vector<std::pair<const void*, StringAtom>> _cache;
        const void* _currentData = nullptr;
    };

} // namespace Core::Gui

#include "List.generated.h" // added by the code generator. Better don't move it.
