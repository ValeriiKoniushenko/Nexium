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
    class BaseCombo : public Widget
    {
        ECS_DECL(BaseCombo, Core::Gui::Widget);

    public:
        [[nodiscard]] glm::vec2 getRealSize() const;

        [[nodiscard]] float getWidth() const override { return getRealSize().x; }
        [[nodiscard]] float getHeight() const override { return getRealSize().y; }

        void setWidth(float newWidth) override;

        void setHeight(float newHeight) override;

    protected:
        void onInitialize() override;

    protected:
        FIELD();
        glm::vec2 _size = glm::vec2(50.f, 0.f);
    };

    CLASS();
    class ComboView : public BaseCombo
    {
        ECS_DECL(ComboView, Core::Gui::BaseCombo);

    public:
        void setData(std::vector<StringAtom>&& items) { _items = std::move(items); }
        void setData(const std::vector<StringAtom>& items) { _items = items; }
        const std::vector<StringAtom>& getData() const noexcept { return _items; }

        void setCurrentIndex(std::size_t i) noexcept
        {
            _currentItem = std::min(i, _items.size() - 1);
        }

        [[nodiscard]] std::size_t getCurrentIndex() const noexcept { return _currentItem; }

        [[nodiscard]] const StringAtom& getSelectedString() const
        {
            return _items.at(_currentItem);
        }

    public: // Delegates
        Delegate<void(StringAtom)>::Ptr onSelect = Delegate<void(StringAtom)>::Create();

    protected:
        void onDraw() override;

    protected:
        std::vector<StringAtom> _items;
        std::size_t _currentItem = 0;
    };

    CLASS();
    class ComboModelBased : public BaseCombo
    {
        ECS_DECL(ComboModelBased, Core::Gui::BaseCombo);

    public:
        /// Set your own data provider. Just a function that will do a few things:
        /// 1. Get index of an element that wants to be processed (by you)
        /// 2. Fill up the second argument StringAtom with some string. It will be shown for user
        /// 3. Return a pointer to your data. It will be used only by you in the future. If you want
        /// to get 'an index' to your data, you can just fetch the current pointer (index) and get
        /// the pointer to your data.
        void setDataProvider(const std::function<const void*(std::size_t, StringAtom&)>& callback);

        /// Set your own data provider. Just a function that will do a few things:
        /// 1. Get index of an element that wants to be processed (by you)
        /// 2. Fill up the second argument StringAtom with some string. It will be shown for user
        /// 3. Return a pointer to your data. It will be used only by you in the future. If you want
        /// to get 'an index' to your data, you can just fetch the current pointer (index) and get
        /// the pointer to your data.
        void setSizeProvider(const std::function<std::size_t()>& callback);

        void setCurrentIndex(std::size_t i) noexcept;

        [[nodiscard]] std::size_t getCurrentIndex() const noexcept { return _currentIndex; }
        [[nodiscard]] const void* getCurrentData() const noexcept { return _currentData; }

        [[nodiscard]] StringAtom tryGetCurrentDataAsString() const;

    public: // Delegates
        Delegate<void(const void*)>::Ptr onSelect = Delegate<void(const void*)>::Create();

    protected:
        void onDraw() override;

    protected:
        std::function<const void*(std::size_t, StringAtom&)> _dataProvider;
        std::function<std::size_t()> _sizeProvider;
        std::vector<std::pair<const void*, StringAtom>> _cache;
        const void* _currentData = nullptr;
        std::size_t _currentIndex = 0;
    };
} // namespace Core::Gui

#include "Combo.generated.h" // added by the code generator. Better don't move it.
