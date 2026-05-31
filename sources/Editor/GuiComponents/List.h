/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "Core/Delegate.h"
#include "Widget.h"

namespace Core::Gui
{
    CLASS();
    class BaseList : public Widget
    {
        R_FRIEND_DECL(Core::Gui::BaseList, Core::Gui::Widget);
        ECS_COMPONENT_DECL(BaseList, Widget);

    public:
        [[nodiscard]] glm::vec2 getRealSize() const;

        [[nodiscard]] float getWidth() const override { return getRealSize().x; }
        [[nodiscard]] float getHeight() const override { return getRealSize().y; }

        void setWidth(float newWidth) override;

        void setHeight(float newHeight) override;

        void setRegexFilter(const StringAtom& filter) { _filter = filter; }
        [[nodiscard]] const StringAtom& getFilter() const noexcept { return _filter; }

    protected:
        void onInitialize() override;

    protected:
        FIELD();
        Core::StringAtom _filter;

        FIELD();
        glm::vec2 _size = glm::vec2(100.f, 150.f);
    };

    CLASS();
    class ListView : public BaseList
    {
        R_FRIEND_DECL(Core::Gui::ListView, Core::Gui::BaseList);
        ECS_COMPONENT_DECL(ListView, BaseList);

    public:
        void setData(std::vector<StringAtom>&& items) { _items = std::move(items); }
        void setData(const std::vector<StringAtom>& items) { _items = items; }
        const std::vector<StringAtom>& getData() const noexcept { return _items; }

        void setCurrentIndex(std::size_t i) noexcept
        {
            _currentItem = std::min(i, _items.size() - 1);
        }

        [[nodiscard]] std::size_t getCurrentIndex() const noexcept { return _currentItem; }
        [[nodiscard]] StringAtom getCurrentData() const { return _items.at(_currentItem); }

    public: // Delegates
        Delegate<void(StringAtom)>::Ptr onSelect = Delegate<void(StringAtom)>::Create();

    protected:
        void onDraw() override;

    protected:
        std::vector<StringAtom> _items;
        std::size_t _currentItem = 0;
    };

    CLASS();
    class ListModelBased : public BaseList
    {
        R_FRIEND_DECL(Core::Gui::ListModelBased, Core::Gui::BaseList);
        ECS_COMPONENT_DECL(ListModelBased, BaseList);

    public:
        /**
         * Set your own data provider. Just a function that will do a few things:
         * 1. Get index of element, that want to be processed(by you)
         * 2. Fill up second argument StringAtom with some string. It will be shown for user
         * 3. Return a pointer to your data. It will be used only by you in the future. If you want
         * to get 'an index' to your data, you can just fetch current pointer(index), and get the
         * pointer to your data.
         */
        void setDataProvider(const std::function<void*(std::size_t, StringAtom&)>& callback);

        /**
         * Set your own data provider. Just a function that will do a few things:
         * 1. Get index of element, that want to be processed(by you)
         * 2. Fill up second argument StringAtom with some string. It will be shown for user
         * 3. Return a pointer to your data. It will be used only by you in the future. If you want
         * to get 'an index' to your data, you can just fetch current pointer(index), and get the
         * pointer to your data.
         */
        void setSizeProvider(const std::function<std::size_t()>& callback);

        void setCurrentIndex(std::size_t i) noexcept;

        [[nodiscard]] std::size_t getCurrentIndex() const noexcept { return _currentIndex; }
        [[nodiscard]] void* getCurrentData() const noexcept { return _currentData; }

    public: // Delegates
        Delegate<void(void*, StringAtom)>::Ptr onSelect
            = Delegate<void(void*, StringAtom)>::Create();

    protected:
        void onDraw() override;

    protected:
        std::function<void*(std::size_t, StringAtom&)> _dataProvider;
        std::function<std::size_t()> _sizeProvider;
        std::vector<std::pair<void*, StringAtom>> _cache;
        void* _currentData = nullptr;
        std::size_t _currentIndex = 0;
    };
} // namespace Core::Gui

#include "List.generated.h" // added by the code generator. Better don't move it.
