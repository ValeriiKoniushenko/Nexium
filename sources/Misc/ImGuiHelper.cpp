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

#include "ImGuiHelper.h"

#include "Core/Math.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"

#include <string>

namespace Core
{
    void FixedLabel(const char* label, float size)
    {
#if defined(DEBUG)
        Assert(ImGui::CalcTextSize(label).x < size);
#endif
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0, 0);
        ImGui::Dummy(ImVec2(size - ImGui::CalcTextSize(label).x, 0));
        ImGui::SameLine(0, 0);
    }

    void InputTextRO(StringAtom value, float size)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        ImGui::PushItemWidth(size);
        ImGui::InputText(("##" + value).c_str(), value.data(), value.size() + 1,
                         ImGuiInputTextFlags_ReadOnly);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor();
    }

    void LabelAndInputTextRO(StringAtom label, StringAtom value, float labelSize, float fullSize)
    {
        ImGui::PushID(static_cast<int>(label.makeHash()));
        FixedLabel(label.data(), labelSize);
        InputTextRO(std::move(value), fullSize - labelSize);
        ImGui::PopID();
    }

    void LabelAndCheckboxRO(StringAtom label, bool v, float labelSize)
    {
        FixedLabel(label.data(), labelSize);
        ImGui::BeginDisabled(true);
        ImGui::Checkbox(("##" + label).c_str(), &v);
        ImGui::EndDisabled();
    }

    bool VectorCombo(const StringAtom& label, int* current, std::vector<StringAtom>& data)
    {
        return ImGui::Combo(
            label.c_str(), current,
            [](void* vec, int idx, const char** out_text)
            {
                const auto* data = reinterpret_cast<std::vector<StringAtom>*>(vec);

                const auto i = static_cast<std::size_t>(idx);
                if (i >= data->size())
                {
                    return false;
                }

                *out_text = data->at(i).c_str();

                return true;
            },
            &data, static_cast<int>(data.size()));
    }

    void LabelAndInputText(const StringAtom& label, std::string& originalString, float labelSize,
                           float fullSize)
    {
        std::string inputData = originalString;

        FixedLabel(label.data(), labelSize);
        InputText(label, inputData, fullSize - labelSize);

        if (originalString != inputData)
        {
            originalString = inputData;
        }
    }

    bool InputText(const StringAtom& label, std::string& value, float size, int flags)
    {
        ImGui::PushItemWidth(size);
        const auto out = ImGui::InputText(("##" + label).c_str(), &value, flags);
        ImGui::PopItemWidth();
        return out;
    }

    void LabelAndInputFloat(const StringAtom& label, float& value, float labelSize, float fullSize,
                            float step, float min, float max, const char* format)
    {
        FixedLabel(label.data(), labelSize);

        ImGui::PushItemWidth(fullSize - labelSize);
        if (ImGui::DragFloat(("##" + label).c_str(), &value, step, min, max, format))
        {
            if (!Math::IsZero(min) && !Math::IsZero(max))
            {
                value = std::clamp(value, min, max);
            }
        }
        ImGui::PopItemWidth();
    }

    bool ToggleButton(const char* label, bool cond, const ImVec4& onColor, const ImVec4& offColor)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, cond ? onColor : offColor);
        const bool isPressed = ImGui::Button(label);
        ImGui::PopStyleColor();

        return isPressed;
    }

    bool ToggleButton(const char* label, bool cond)
    {
        return ToggleButton(label, cond, ImGui::GetStyle().Colors[ImGuiCol_Button],
                            ImVec4(0, 0, 0, 0));
    }

    bool ButtonAndInputTextRO(StringAtom label, StringAtom value, float labelSize, float fullSize)
    {
        const auto& style = ImGui::GetStyle();

        labelSize -= style.FramePadding.x * 2.f;

        ImGui::PushID(static_cast<int>(label.makeHash()));
        const bool isPressed = ImGui::Button(label.data(), ImVec2(labelSize, 0.f));

        ImGui::SameLine(0, 8.f);

        InputTextRO(std::move(value), fullSize - labelSize - 8.f);
        ImGui::PopID();

        return isPressed;
    }

} // namespace Core
