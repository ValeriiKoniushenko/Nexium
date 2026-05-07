/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "../../Misc/Configs.h"
#include "HorizontalLayout.h"
#include "Input.h"
#include "Label.h"
#include "Widget.h"

namespace Core::Gui
{
    template<std::size_t Size, Utils::IsArithmetic Type>
    class VecNumInput : public HorizontalLayout
    {
        ECS_TEMPLATE_COMPONENT_DECL(VecNumInput, HorizontalLayout, Size, Type);

        static_assert(Size >= 1 && Size <= 4, "Size must >= 0 && <= 4");

    public:
        std::array<NumInput<Type>*, Size> inputs;
        std::array<Label*, Size> labels;

        void setLabelText(std::array<char, Size> newText)
        {
            for (std::size_t i = 0; i < Size; ++i)
            {
                if (Verify(labels[i])) [[likely]]
                {
                    StringAtom text;
                    text.resize(2);
                    text[0] = newText[i];
                    text[1] = ':';
                    text.toUpperCase();
                    labels[i]->setText(text);
                }
            }
        }

        void setLabelColor(std::array<Color4, Size> data)
        {
            for (std::size_t i = 0; i < Size; ++i)
            {
                if (Verify(labels[i])) [[likely]]
                {
                    labels[i]->setTextColor(data[i]);
                }
            }
        }

        void setReadOnly(bool val)
        {
            for (std::size_t i = 0; i < Size; ++i)
            {
                if (Verify(inputs[i])) [[likely]]
                {
                    inputs[i]->disableWidget(val);
                }
            }
        }

        void setInputtedData(const glm::vec<Size, Type>& data)
        {
            auto* raw = reinterpret_cast<const Type*>(&data);
            for (std::size_t i = 0; i < Size; ++i)
            {
                if (Verify(inputs[i])) [[likely]]
                {
                    inputs[i]->setInputtedData(raw[i]);
                }
            }
        }

        [[nodiscard]] glm::vec<Size, Type> getInputtedData() const
        {
            glm::vec<Size, Type> result = {};
            auto* raw = reinterpret_cast<Type*>(&result);
            for (std::size_t i = 0; i < Size; ++i)
            {
                if (Verify(inputs[i])) [[likely]]
                {
                    raw[i] = inputs[i]->getInputtedData();
                }
            }

            return result;
        }

        [[nodiscard]] bool isValid() const override
        {
            for (std::size_t i = 0; i < Size; ++i)
            {
                if (!inputs[i])
                {
                    return false;
                }
            }

            return true;
        }

    public: // Delegates
        Delegate<void(glm::vec<Size, Type>)>::Ptr onInput
            = Delegate<void(glm::vec<Size, Type>)>::Create();

    protected:
        void onInitialize() override
        {
            HorizontalLayout::onInitialize();

            const std::vector<std::pair<Color4, StringAtom>> defaults = {
                { Config::ColorRed, "X:"_atom },
                { Config::ColorGreen, "Y:"_atom },
                { Config::ColorBlue, "Z:"_atom },
                { Config::ColorYellow, "W:"_atom },
            };

            _children.clear();

            setFlex(Flex::FlexWidth);
            setHorizontalAlign(Align::SpaceBetween);
            for (std::size_t i = 0; i < Size; ++i)
            {
                auto* hLayout = addChildComponent<HorizontalLayout>();
                labels[i] = hLayout->template addChildComponent<Label>();
                labels[i]->setTextColor(defaults[i].first);
                labels[i]->setText(defaults[i].second);
                labels[i]->setTruncateLongText(false);
                labels[i]->setWidth(10.f);
                hLayout->setHorizontalAlign(Align::SpaceBetween);

                inputs[i] = hLayout->template addChildComponent<NumInput<Type>>();
                inputs[i]->setFlex(Flex::FlexWidth);
                _subscriptionPool << inputs[i]->onInput->subscribeAndGetID(
                    [this](auto) { onInput->trigger(getInputtedData()); });
            }
        }

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;
    };

    ECS_TEMPLATE_COMPONENT_IMPL(BRACKETS(VecNumInput<Size, Type>),
                                BRACKETS(std::size_t Size, Utils::IsArithmetic Type)

    )

    using Int4Input = VecNumInput<4, int>;
    using Float4Input = VecNumInput<4, float>;
    using Double4Input = VecNumInput<4, double>;

    using Int3Input = VecNumInput<3, int>;
    using Float3Input = VecNumInput<3, float>;
    using Double3Input = VecNumInput<3, double>;

    using Int2Input = VecNumInput<2, int>;
    using Float2Input = VecNumInput<2, float>;
    using Double2Input = VecNumInput<2, double>;
} // namespace Core::Gui

#include "VecInput.generated.inl" // added by the code generator. Better don't move it.
