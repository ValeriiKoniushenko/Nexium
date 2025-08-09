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

#include "ImGui/misc/cpp/imgui_stdlib.h"

#include <string>

namespace SW
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

    void InputTextRO(Core::StringAtom value, float size)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        ImGui::PushItemWidth(size);
        ImGui::InputText(("##" + value).c_str(), value.data(), value.size() + 1,
                         ImGuiInputTextFlags_ReadOnly);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor();
    }

    void LabelAndInputTextRO(Core::StringAtom label, Core::StringAtom value, float labelSize,
                             float fullSize)
    {
        FixedLabel(label.data(), labelSize);
        InputTextRO(std::move(value), fullSize - labelSize);
    }

    void LabelAndCheckboxRO(Core::StringAtom label, bool v, float labelSize)
    {
        FixedLabel(label.data(), labelSize);
        ImGui::BeginDisabled(true);
        ImGui::Checkbox(("##" + label).c_str(), &v);
        ImGui::EndDisabled();
    }

    bool VectorCombo(Core::StringAtom label, int* current, std::vector<Core::StringAtom>& data)
    {
        return ImGui::Combo(
            label.c_str(), current,
            [](void* vec, int idx, const char** out_text)
            {
                auto* data = reinterpret_cast<std::vector<Core::StringAtom>*>(vec);

                const auto i = static_cast<std::size_t>(idx);
                if (i >= data->size())
                {
                    return false;
                }

                *out_text = data->at(i).c_str();

                return true;
            },
            reinterpret_cast<void*>(&data), data.size());
    }

    void Vec3Control::draw(glm::vec3& _v, float availSpace)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ImGui::PushID(label.c_str());

        ImGui::AlignTextToFramePadding();

        FixedLabel(label.c_str(), labelWidth);
        availSpace -= labelWidth;

        float* vec[3] = { &_v.x, &_v.y, &_v.z };

        int i = 0;
        for (const auto& component : components)
        {
            const float textWidth = ImGui::CalcTextSize(component.text.c_str()).x;

            // 3 - just components number
            // 2 - because for last one we don't need a gap
            const float inputWidth
                = (availSpace - ((textWidth + afterTextGap) * 3.f + betweenInputsGap * 2.f)) / 3.f;

            ImGui::PushStyleColor(ImGuiCol_Text, component.color);
            ImGui::TextUnformatted(component.text.c_str());
            ImGui::PopStyleColor();

            ImGui::SameLine(0, afterTextGap);

            ImGui::PushID(i);
            ImGui::PushItemWidth(inputWidth);
            ImGui::DragFloat("", vec[i], floatStep, floatMin, floatMax, "%.2f", flags);
            ImGui::PopItemWidth();
            ImGui::SameLine(0, i == 2 ? 0 : betweenInputsGap);
            ImGui::PopID();

            ++i;
        }
        ImGui::PopID();

        ImGui::PopStyleVar();

        ImGui::Dummy(ImVec2(0, 0));
    }
} // namespace SW
