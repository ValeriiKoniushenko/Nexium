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
#include "Editor/Configs.h"
#include "HorizontalLayout.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "Label.h"
#include "Widget.h"

#include <limits>

namespace Core
{

    class BaseInput : public Widget
    {
        ECS_COMPONENT_DECL(BaseInput, Widget);

    public:
        void setTextColor(const Color4& value);
        void resetTextColor();
        [[nodiscard]] std::optional<Color4> getTextColor() const;

        [[nodiscard]] glm::vec2 getRealSize() const;

        [[nodiscard]] float getWidth() const override { return getRealSize().x; }
        [[nodiscard]] float getHeight() const override { return getRealSize().y; }
        void setWidth(float newWidth) override;
        void setHeight(float newHeight) override;

    protected:
        void onInitialize() override;

    protected:
        std::optional<Color4> _textColor;
        glm::vec2 _size = {};
    };

    class TextInput : public BaseInput
    {
        ECS_COMPONENT_DECL(TextInput, BaseInput);

    public:
        void setInputtedData(std::string data) { _buffer = std::move(data); }
        [[nodiscard]] const std::string& getInputtedData() const noexcept { return _buffer; }

        void setPlaceholder(StringAtom placeholder) { _placeholder = std::move(placeholder); }
        [[nodiscard]] const StringAtom& getPlaceholder() const noexcept { return _placeholder; }

        void setReadOnly(bool value);
        [[nodiscard]] bool isReadOnly() const noexcept;

    public: // Delegates
        Delegate<void(const char*)> onInput;

    protected:
        struct InputTextCallback_UserData
        {
            std::string* Str;
            ImGuiInputTextCallback ChainCallback = nullptr;
            void* ChainCallbackUserData = nullptr;
        };

        static int InputTextCallback(ImGuiInputTextCallbackData* data);

        void onDraw() override;

    protected:
        std::string _buffer;
        StringAtom _placeholder = ""_atom;
        int _flags = ImGuiInputTextFlags_None;
    };

    template<Utils::IsArithmetic Type>
    class NumInput : public BaseInput
    {
        ECS_TEMPLATE_COMPONENT_DECL(NumInput, BaseInput, Type)

    public:
        void setInputtedData(Type data) { _buffer = data; }
        [[nodiscard]] Type getInputtedData() const noexcept { return _buffer; }

        void setStep(Type value) noexcept { _step = value; }
        [[nodiscard]] Type getStep() const noexcept { return _step; }
        void setMin(Type value) noexcept { _min = value; }
        [[nodiscard]] Type getMin() const noexcept { return _min; }
        void setMax(Type value) noexcept { _max = value; }
        [[nodiscard]] Type getMax() const noexcept { return _max; }

        void setPrecision(int p) noexcept { _precisionStr[2] = std::clamp(p, 0, 7); }

        [[nodiscard]] int getPrecision() const noexcept { return _precisionStr[2] - '0'; }

    public: // Delegates
        Delegate<void(Type)> onInput;

    protected:
        void onDraw() override
        {
            ImGui::PushItemWidth(_size.x);

            const auto old = _buffer;
            if constexpr (std::is_same_v<float, Type>)
            {
                ImGui::DragFloat("", &_buffer, _step, _min, _max, _precisionStr.c_str(),
                                 ImGuiInputTextFlags_None);
            }
            else if constexpr (std::is_same_v<int, Type>)
            {
                ImGui::DragInt("", &_buffer, _step, _min, _max, "%d", ImGuiInputTextFlags_None);
            }

            if (old != _buffer)
            {
                onInput.trigger(_buffer);
            }

            ImGui::PopItemWidth();
        }

    protected:
        StringAtom _precisionStr = "%.2f";
        Type _buffer = 0;
        float _step = 0.1f;
        Type _min = std::numeric_limits<Type>::min();
        Type _max = std::numeric_limits<Type>::max();
    };

    template<std::size_t Size, Utils::IsArithmetic Type>
    class VecNumInput : public HorizontalLayout
    {
        ECS_TEMPLATE_COMPONENT_DECL(VecNumInput, HorizontalLayout, Size, Type);

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
            glm::vec<Size, Type> result;
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
        Delegate<void(glm::vec<Size, Type>)> onInput;

    protected:
        void onInitialize() override
        {
            HorizontalLayout::onInitialize();

            const std::vector<std::pair<Color4, StringAtom>> defaults = {
                { Config::ColorRed, "X"_atom },
                { Config::ColorGreen, "Y"_atom },
                { Config::ColorBlue, "Z"_atom },
                { Config::ColorYellow, "W"_atom },
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
                labels[i]->setWidth(10.f);

                inputs[i] = hLayout->template addChildComponent<NumInput<Type>>();
                inputs[i]->setFlex(Flex::FlexWidth);
                inputs[i]->onInput.subscribe(
                    [this](auto)
                    {
                        onInput.trigger(getInputtedData());
                    });
            }
        }
    };

    ECS_TEMPLATE_COMPONENT_IMPL(BRACKETS(VecNumInput<Size, Type>),
                                BRACKETS(std::size_t Size, Utils::IsArithmetic Type))
    ECS_TEMPLATE_COMPONENT_IMPL(BRACKETS(NumInput<Type>), BRACKETS(Utils::IsArithmetic Type))

    using DoubleInput = NumInput<double>;
    using FloatInput = NumInput<float>;
    using IntInput = NumInput<int>;

    using Int4Input = VecNumInput<4, int>;
    using Float4Input = VecNumInput<4, float>;
    using Double4Input = VecNumInput<4, double>;

    using Int3Input = VecNumInput<3, int>;
    using Float3Input = VecNumInput<3, float>;
    using Double3Input = VecNumInput<3, double>;

    using Int2Input = VecNumInput<2, int>;
    using Float2Input = VecNumInput<2, float>;
    using Double2Input = VecNumInput<2, double>;

} // namespace Core