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

#include "Input.h"


#include "Input.generated.cpp.inl" // this line added by the code generator.

namespace Core::Gui
{
    ECS_COMPONENT_IMPL(BaseInput);
    ECS_COMPONENT_IMPL(TextInput);
    ECS_COMPONENT_IMPL(Color3Input);

    void BaseInput::setTextColor(const Color4& value)
    {
        _textColor = value;
    }

    void BaseInput::resetTextColor()
    {
        _textColor.reset();
    }

    void BaseInput::setBorderColor(const Color4& value)
    {
        _borderColor = value;
    }

    void BaseInput::resetBorderColor()
    {
        _borderColor.reset();
    }

    std::optional<Color4> BaseInput::getTextColor() const
    {
        return _textColor;
    }

    glm::vec2 BaseInput::getRealSize() const
    {
        return _size;
    }

    void BaseInput::setWidth(float newWidth)
    {
        _size.x = newWidth;
    }

    void BaseInput::setHeight(float newHeight)
    {
        _size.y = newHeight;
    }

    void TextInput::setReadOnly(bool value)
    {
        _flags |= ImGuiInputTextFlags_ReadOnly;
    }

    bool TextInput::isReadOnly() const noexcept
    {
        return _flags & ImGuiInputTextFlags_ReadOnly;
    }

    void BaseInput::onInitialize()
    {
        Widget::onInitialize();
        if (_name.isEmpty())
        {
            setComponentName("Input"_atom);
        }
        if (_size == glm::vec2(0))
        {
            _size.x = 100.f;
            _size.y = ImGui::CalcTextSize("X").y + style().FramePadding.y * 2.0f;
        }
    }

    int TextInput::InputTextCallback(ImGuiInputTextCallbackData* data)
    {
        const auto* user_data = static_cast<InputTextCallback_UserData*>(data->UserData);
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
        {
            std::string* str = user_data->Str;
            IM_ASSERT(data->Buf == str->c_str());
            str->resize(data->BufTextLen);
            data->Buf = str->data();
        }
        else if (user_data->ChainCallback)
        {
            // Forward to user callback, if any
            data->UserData = user_data->ChainCallbackUserData;
            return user_data->ChainCallback(data);
        }
        return 0;
    }

    void TextInput::onDraw()
    {
        int pushedStyles = 0;

        pushedStyles += ImGui::OptPushStyleColor(ImGuiCol_Text, _textColor);
        pushedStyles += ImGui::OptPushStyleColor(ImGuiCol_Border, _borderColor);

        const int flags = ImGuiInputTextFlags_CallbackResize | _flags;
        const bool isRO = isReadOnly();
        if (isRO)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }

        InputTextCallback_UserData cb_user_data;
        cb_user_data.Str = &_buffer;
        if (ImGui::InputTextEx("", _placeholder.c_str(), _buffer.data(),
                               static_cast<int>(_buffer.capacity() + 1), _size, flags,
                               InputTextCallback, &cb_user_data))
        {
            onInput->trigger(_buffer.c_str() ? _buffer.c_str() : "");
        }

        if (isRO)
        {
            ImGui::PopStyleVar();
        }

        ImGui::PopStyleColor(pushedStyles);
    }

    void Color3Input::setInputtedData(const Color3& data)
    {
        _buffer = data.toNorm();
        _stringBuffer = "{} {} {}"_f << data.r << data.g << data.b;
    }

    void Color3Input::onDraw()
    {
        int pushedStyles = 3;

        _textColor = NormColor4(glm::vec4(glm::vec3(1.f) - _buffer.toGlm(), 1.f)).toColor();
        pushedStyles += ImGui::OptPushStyleColor(ImGuiCol_Text, _textColor);

        ImGui::PushStyleColor(ImGuiCol_Button, glm::vec4(_buffer.toGlm(), 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, glm::vec4(_buffer.toGlm(), 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, glm::vec4(_buffer.toGlm(), 1.f));

        if (ImGui::Button(_stringBuffer.c_str(), _size))
        {
            ImGui::OpenPopup("ColorPickerPopup");
        }

        if (ImGui::BeginPopup("ColorPickerPopup"))
        {
            glm::vec3 input = _buffer;
            ImGui::ColorPicker3("##picker", &input.x,
                                ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview);

            if (input != _buffer)
            {
                setInputtedData(NormColor3(input));
                onInput->trigger(_buffer.toColor());
            }

            ImGui::EndPopup();
        }

        ImGui::PopStyleColor(pushedStyles);
    }

} // namespace Core::Gui