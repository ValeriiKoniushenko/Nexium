// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "Button.h"
#include "HorizontalLayout.h"
#include "Input.h"
#include "Label.h"
#include "Misc/IconsFontAwesome.h"
#include "VerticalLayout.h"

#include <Core/Delegate.h>

namespace Core::Gui
{
    class Button;
    class Label;

    class ArrayCell : public HorizontalLayout
    {
        ECS_COMPONENT_DECL_NO_CNSTR(ArrayCell, HorizontalLayout);

    public:
        Label* label = nullptr;
        HorizontalLayout* content = nullptr;
        Button* deleteButton = nullptr;

    public:
        explicit ArrayCell(const StringAtom& name = "")
            : HorizontalLayout(componentType, name)
        {
            if (name.isEmpty())
            {
                setComponentName("ArrayCell"_atom);
            }
            setFlex(Flex::FlexWidth);
            setVerticalAlign(Align::Center);
            setHorizontalAlign(Align::Left);

            label = addChildComponent<Label>();

            content = addChildComponent<HorizontalLayout>();
            content->setFlex(Flex::FlexWidth);
            content->setHorizontalAlign(Align::SpaceBetween);
            content->setVerticalAlign(Align::Center);

            deleteButton = addChildComponent<Button>();
            deleteButton->setText(ICON_FA_TRASH);
        }

        void setIndexText(std::size_t i)
        {
            if (Verify(label))
            {
                label->setText(StringAtom::MakeFrom(i) + "#"_atom);
            }
        }
    };

    template<class T, class ArrayCellViewerFunc>
    class BaseArray : public VerticalLayout
    {
        ECS_TEMPLATE_COMPONENT_DECL(BaseArray, VerticalLayout, T, ArrayCellViewerFunc);

    public:
        constexpr static bool isConst = std::is_const_v<T>;

        Delegate<void()> onChange;
        Delegate<void(std::size_t, T&)> onAdd;
        Delegate<void(std::size_t)> onEraseAt;

    public:
        void eraseAt(std::size_t i)
        {
            if (Verify(i < _data.size()))
            {
                _data.erase(_data.begin() + i);
                onEraseAt.trigger(i);
                makeDirty();
            }
        }

        void addEmpty()
        {
            makeDirty();
            _data.emplace_back();
            onAdd.trigger(_data.size() - 1, _data.back());
        }

        void add(const T& data)
        {
            _data.emplace_back(data);
            makeDirty();
            onAdd.trigger(_data.size() - 1, _data.back());
        }

        [[nodiscard]] virtual std::size_t size() const { return _data.size(); }

        [[nodiscard]] const T& operator[](std::size_t i) const { return _data.at(i); }
        [[nodiscard]] T& operator[](std::size_t i) { return _data.at(i); }

        [[nodiscard]] const std::vector<T>& getData() const { return _data; }
        void setData(const std::vector<T>& data)
        {
            _data = data;
            makeDirty();
        }
        void setData(std::vector<T>&& data)
        {
            _data = std::move(data);
            makeDirty();
        }

        void setReadOnly(bool value)
        {
            _isReadOnly = value;
            makeDirty();
        }
        [[nodiscard]] bool isReadOnly() const { return _isReadOnly; }

    protected:
        void onTick(float delta) override
        {
            VerticalLayout::onTick(delta);

            if (_isDirty)
            {
                recreate();
                _isDirty = false;
            }
        }

        void onInitialize() override
        {
            VerticalLayout::onInitialize();

            setComponentName("Array"_atom);
            setVerticalAlign(Align::Top);
            setHorizontalAlign(Align::Center);

            recreate();
        }

        void recreate()
        {
            _children.clear();

            // Children creating
            for (std::size_t i = 0; i < size(); ++i)
            {
                auto* cell = addChildComponent<ArrayCell>();
                cell->setIndexText(i);
                *cell->content = std::move(*(ArrayCellViewerFunc{}(_data.at(i))));
                cell->content->setIsAutoDraw(false);

                if (_isReadOnly)
                {
                    cell->deleteButton->disableWidget(_isReadOnly);
                }
                else
                {
                    cell->deleteButton->onClick.subscribe(
                        [this, i](auto*)
                        {
                            eraseAt(i);
                        });
                }
            }

            // Add button creating
            auto* addButton = addChildComponent<Button>();
            addButton->setText("Add new item");
            addButton->setFlex(Flex::FlexWidth);
            if (_isReadOnly)
            {
                addButton->disableWidget(_isReadOnly);
            }
            else
            {
                addButton->onClick.subscribe(
                    [this](auto*)
                    {
                        addEmpty();
                    });
            }
        }

        void makeDirty()
        {
            onChange.trigger();
            _isDirty = true;
        }

    protected:
        std::vector<T> _data;

    private:
        bool _isDirty = true;
        bool _isReadOnly = false;
    };

    ECS_TEMPLATE_COMPONENT_IMPL(BRACKETS(BaseArray<T, ArrayCellViewerFunc>),
                                BRACKETS(class T, class ArrayCellViewerFunc))

    using StringArray = BaseArray<StringAtom, decltype([](const StringAtom& str) -> HorizontalLayout::Ptr
    {
        auto l = HorizontalLayout::Create();
        auto label = l->addChildComponent<Label>();
        label->setText(str);
        return l;
    })>;

} // namespace Core::Gui