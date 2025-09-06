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

#include "Input.h"

namespace Core
{

    ECS_REGISTER_NEW_COMPONENT_TYPE(BaseInput);
    ECS_REGISTER_NEW_COMPONENT_TYPE(TextInput);

    void BaseInput::setTextColor(const Color4& value)
    {
        _textColor = value;
    }

    void BaseInput::resetTextColor()
    {
        _textColor.reset();
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

        int flags = ImGuiInputTextFlags_CallbackResize;

        InputTextCallback_UserData cb_user_data;
        cb_user_data.Str = &_buffer;
        ImGui::InputTextEx("", nullptr, _buffer.data(), _buffer.capacity() + 1, _size, flags,
                           InputTextCallback, &cb_user_data,
                           [this](const char* newText)
                           {
                               onInput.trigger(newText);
                           });

        ImGui::PopStyleColor(pushedStyles);
    }

} // namespace Core