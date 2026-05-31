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
#include "Utils/Concepts.h"
#include "Widget.h"

#include <limits>

namespace Core::Gui
{
    CLASS();
    class BaseInput : public Widget
    {
        R_FRIEND_DECL(Core::Gui::BaseInput, Core::Gui::Widget);
        ECS_COMPONENT_DECL(BaseInput, Widget);

    public:
        void setTextColor(const Color4& value);
        void resetTextColor();

        void setBorderColor(const Color4& value);
        void resetBorderColor();

        [[nodiscard]] std::optional<Color4> getTextColor() const;

        [[nodiscard]] glm::vec2 getRealSize() const;

        [[nodiscard]] float getWidth() const override { return getRealSize().x; }
        [[nodiscard]] float getHeight() const override { return getRealSize().y; }

        void setWidth(float newWidth) override;

        void setHeight(float newHeight) override;

    protected:
        void onInitialize() override;

    protected:
        FIELD();
        std::optional<Core::Color4> _textColor;
        FIELD();
        std::optional<Core::Color4> _borderColor;
        FIELD();
        glm::vec2 _size = glm::vec2{};
    };

    CLASS();
    class TextInput : public BaseInput
    {
        R_FRIEND_DECL(Core::Gui::TextInput, Core::Gui::BaseInput);
        ECS_COMPONENT_DECL(TextInput, BaseInput);

    public:
        void setInputtedData(std::string data) { _buffer = std::move(data); }
        [[nodiscard]] const std::string& getInputtedData() const noexcept { return _buffer; }

        void setPlaceholder(StringAtom placeholder) { _placeholder = std::move(placeholder); }
        [[nodiscard]] const StringAtom& getPlaceholder() const noexcept { return _placeholder; }

        void setReadOnly(bool value);

        [[nodiscard]] bool isReadOnly() const noexcept;

    public: // Delegates
        Delegate<void(const char*)>::Ptr onInput = Delegate<void(const char*)>::Create();

    protected:
        struct InputTextCallback_UserData
        {
            std::string* Str = nullptr;
            ImGuiInputTextCallback ChainCallback = nullptr;
            void* ChainCallbackUserData = nullptr;
        };

        static int InputTextCallback(ImGuiInputTextCallbackData* data);

        void onDraw() override;

    protected:
        FIELD();
        std::string _buffer;
        FIELD();
        Core::StringAtom _placeholder;
        FIELD();
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
        Delegate<void(Type)>::Ptr onInput = Delegate<void(Type)>::Create();

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
                onInput->trigger(_buffer);
            }

            ImGui::PopItemWidth();
        }

    protected:
        StringAtom _precisionStr = "%.2f";
        Type _buffer = 0;
        float _step = 0.1f;
        Type _min = std::numeric_limits<Type>::lowest();
        Type _max = std::numeric_limits<Type>::max();
    };

    ECS_TEMPLATE_COMPONENT_IMPL(BRACKETS(NumInput<Type>), BRACKETS(Utils::IsArithmetic Type))

    CLASS();
    class Color3Input : public BaseInput
    {
        R_FRIEND_DECL(Core::Gui::Color3Input, Core::Gui::BaseInput);
        ECS_COMPONENT_DECL(Color3Input, BaseInput);

    public:
        void setInputtedData(const Color3& data);
        void setInputtedData(const NormColor3& data) { setInputtedData(data.toColor()); }
        [[nodiscard]] Color3 getInputtedData() const noexcept { return _buffer.toColor(); }

    public: // Delegates
        Delegate<void(const Color3&)>::Ptr onInput = Delegate<void(const Color3&)>::Create();

    protected:
        void onDraw() override;

    protected:
        FIELD();
        Core::NormColor3 _buffer = Core::NormColor3(0.f, 0.f, 0.f);

        FIELD();
        Core::StringAtom _stringBuffer = "0 0 0";
        int _flags = ImGuiInputTextFlags_None;
    };

    using DoubleInput = NumInput<double>;
    using FloatInput = NumInput<float>;
    using IntInput = NumInput<int>;

} // namespace Core::Gui

#include "Input.generated.h" // added by the code generator. Better don't move it.
