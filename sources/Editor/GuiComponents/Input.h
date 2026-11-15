// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Editor

#pragma once

#include "Core/Delegate.h"
#include "Editor/ForwardDeclarations.h"
#include "Utils/Concepts.h"
#include "Widget.h"

#include <cstdint>
#include <filesystem>
#include <limits>

namespace NX::Gui
{
    /// The ImGui data type that stores a `T` without conversion.
    template<IsScalarNumber T>
    [[nodiscard]] consteval ImGuiDataType ImGuiDataTypeOf() noexcept
    {
        if constexpr (std::is_same_v<T, float>)
        {
            return ImGuiDataType_Float;
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            return ImGuiDataType_Double;
        }
        else
        {
            static_assert(std::is_integral_v<T> && sizeof(T) <= 8);
            constexpr bool isSigned = std::is_signed_v<T>;
            if constexpr (sizeof(T) == 1)
            {
                return isSigned ? ImGuiDataType_S8 : ImGuiDataType_U8;
            }
            else if constexpr (sizeof(T) == 2)
            {
                return isSigned ? ImGuiDataType_S16 : ImGuiDataType_U16;
            }
            else if constexpr (sizeof(T) == 4)
            {
                return isSigned ? ImGuiDataType_S32 : ImGuiDataType_U32;
            }
            else
            {
                return isSigned ? ImGuiDataType_S64 : ImGuiDataType_U64;
            }
        }
    }

    CLASS();
    class BaseInput : public Widget
    {
        ECS_DECL(BaseInput, NX::Gui::Widget);

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
        ECS_DECL(TextInput, NX::Gui::BaseInput);

    public:
        void setInputtedData(std::string data) { _buffer = std::move(data); }
        [[nodiscard]] const std::string& getInputtedData() const noexcept { return _buffer; }

        void setPlaceholder(StringAtom placeholder) { _placeholder = std::move(placeholder); }
        [[nodiscard]] const StringAtom& getPlaceholder() const noexcept { return _placeholder; }

        void setReadOnly(bool value);
        void requestFocus() noexcept { _needFocus = true; }
        void requestSelectAll() noexcept { _needSelectAll = true; }

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

        /// Called after the user changed the text, right after `onInput`.
        virtual void onTextEdited() {}

    protected:
        FIELD();
        std::string _buffer;
        FIELD();
        Core::StringAtom _placeholder;
        // ImGui flags: an implementation detail, not editable as a number.
        FIELD(R::Meta(Hidden));
        int _flags = ImGuiInputTextFlags_None;
        FIELD();
        bool _needFocus = false;
        FIELD();
        bool _needSelectAll = false;
    };

    /// A drag box for any number type. The value is edited in its own type: no conversion
    /// through `int` or `float`, so `std::size_t` or `int64_t` values are never truncated.
    template<IsScalarNumber Type>
    class NumInput : public BaseInput
    {
        ECS_TEMPLATE_COMPONENT_DECL(NumInput, BaseInput, Type)

    public:
        using ValueType = Type;

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
            // Integers use ImGui's default format for their data type ("%d", "%llu", ...).
            const char* format = std::is_floating_point_v<Type> ? _precisionStr.c_str() : nullptr;
            ImGui::DragScalar("", ImGuiDataTypeOf<Type>(), &_buffer, _step, &_min, &_max, format,
                              ImGuiSliderFlags_None);

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

    ECS_TEMPLATE_COMPONENT_IMPL(BRACKETS(NumInput<Type>), BRACKETS(IsScalarNumber Type))

    CLASS();
    class Color3Input : public BaseInput
    {
        ECS_DECL(Color3Input, NX::Gui::BaseInput);

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

    CLASS();
    class Color4Input : public BaseInput
    {
        ECS_DECL(Color4Input, NX::Gui::BaseInput);

    public:
        void setInputtedData(const Color4& data);
        void setInputtedData(const NormColor4& data) { setInputtedData(data.toColor()); }
        [[nodiscard]] Color4 getInputtedData() const noexcept { return _buffer.toColor(); }

    public: // Delegates
        Delegate<void(const Color4&)>::Ptr onInput = Delegate<void(const Color4&)>::Create();

    protected:
        void onDraw() override;

    protected:
        FIELD();
        Core::NormColor4 _buffer = Core::NormColor4(0.f, 0.f, 0.f, 1.f);

        FIELD();
        Core::StringAtom _stringBuffer = "0 0 0 255";
    };

    /// A text row for `std::filesystem::path`. Shows and stores the generic form ('/'
    /// separators) so the text does not depend on the platform.
    CLASS();
    class PathInput : public TextInput
    {
        ECS_DECL(PathInput, NX::Gui::TextInput);

    public:
        void setPath(const std::filesystem::path& path) { setInputtedData(path.generic_string()); }
        [[nodiscard]] std::filesystem::path getPath() const { return { getInputtedData() }; }

    public: // Delegates
        Delegate<void(const std::filesystem::path&)>::Ptr onPathInput
            = Delegate<void(const std::filesystem::path&)>::Create();

    protected:
        void onTextEdited() override { onPathInput->trigger(getPath()); }
    };

    using DoubleInput = NumInput<double>;
    using FloatInput = NumInput<float>;
    using IntInput = NumInput<int>;

} // namespace NX::Gui

#include "Input.generated.h" // added by the code generator. Better don't move it.
