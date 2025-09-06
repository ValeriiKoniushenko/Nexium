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

#include "Core/Delegate.h"
#include "Widget.h"

namespace Core
{

    class Combo : public Widget
    {
        ECS_REGISTER_NEW_COMPONENT(Combo, Widget);

    public:
        [[nodiscard]] glm::vec2 getRealSize() const;

        [[nodiscard]] float getWidth() const override { return getRealSize().x; }
        [[nodiscard]] float getHeight() const override { return getRealSize().y; }
        void setWidth(float newWidth) override;
        void setHeight(float newHeight) override;

        void setData(std::vector<StringAtom>&& items) { _items = std::move(items); }
        void setData(const std::vector<StringAtom>& items) { _items = items; }
        const std::vector<StringAtom>& getData() const noexcept { return _items; }

    public: // Delegates
        Delegate<void(StringAtom)> onSelect;

    protected:
        void onDraw() override;
        void onInitialize() override;

    protected:
        std::vector<StringAtom> _items;
        glm::vec2 _size = glm::vec2(50.f, 0.f);
        std::size_t _currentItem = 0;
    };

} // namespace Core