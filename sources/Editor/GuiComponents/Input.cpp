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

#include "ImGui/imgui_internal.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "Misc/ImGuiHelper.h"

namespace
{

    struct InputTextCallback_UserData
    {
        std::string* Str;
        ImGuiInputTextCallback ChainCallback = nullptr;
        void* ChainCallbackUserData = nullptr;
    };

    int InputTextCallback(ImGuiInputTextCallbackData* data)
    {
        const InputTextCallback_UserData* user_data
            = static_cast<InputTextCallback_UserData*>(data->UserData);
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
} // namespace

namespace Core
{

    ECS_REGISTER_NEW_COMPONENT_TYPE(Input);

    Input& Input::setTextColor(const Color4& value)
    {
        _textColor = value;
        return *this;
    }

    Input& Input::resetTextColor()
    {
        _textColor.reset();
        return *this;
    }

    std::optional<Color4> Input::getTextColor() const
    {
        return _textColor;
    }

    glm::vec2 Input::getRealSize() const
    {
        return _size;
    }

    void Input::onDraw()
    {
        int pushedStyles = 0;

        pushedStyles += ImGui::OptPushStyleColor(ImGuiCol_Text, _textColor);

        int flags = ImGuiInputTextFlags_CallbackResize;

        InputTextCallback_UserData cb_user_data;
        cb_user_data.Str = &_buffer;
        ImGui::InputTextEx("", nullptr, _buffer.data(), _buffer.capacity() + 1, _size, flags,
                           InputTextCallback, &cb_user_data, [this](const char* newText)
                           {
                               onInput.trigger(newText);
                           });

        ImGui::PopStyleColor(pushedStyles);
    }

    void Input::onInitialize()
    {
        Widget::onInitialize();
        if (_name.isEmpty())
        {
            setComponentName("Input");
        }
        if (_size == glm::vec2(0))
        {
            _size.x = 100.f;
            _size.y = ImGui::CalcTextSize("X").y + style().FramePadding.y * 2.0f;
        }
    }
} // namespace Core