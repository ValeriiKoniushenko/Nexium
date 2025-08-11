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

#include "Core/StringHelper.h"
#include "ImGui/imgui.h"
#include "glm/glm.hpp"

#include <array>
#include <expected>

namespace Core
{

    void FixedLabel(const char* label, float size);
    void InputTextRO(StringAtom value, float size);
    void InputText(const StringAtom& label, std::string& value, float size);

    void LabelAndInputTextRO(StringAtom label, StringAtom value, float labelSize, float fullSize);

    void LabelAndInputText(const StringAtom& label, std::string& value, float labelSize,
                           float fullSize);

    void LabelAndInputFloat(const StringAtom& label, float& value, float labelSize, float fullSize,
                            float flostep = 0, float min = 0, float max = 0,
                            const char* format = "%.2f");

    template<class T>
    void LabelAndInputTextRO(StringAtom label, T value, float labelSize, float fullSize)
    {
        LabelAndInputTextRO(std::move(label), StringAtom::MakeFrom(value), labelSize, fullSize);
    }

    void LabelAndCheckboxRO(StringAtom label, bool v, float labelSize);

    bool VectorCombo(StringAtom label, int* current, std::vector<StringAtom>& data);

    bool ToggleButton(const char* lablel, bool cond);

    template<int VecCount>
    struct VecControl
    {
        struct Component
        {
            StringAtom text = ""_atom;
            ImVec4 color = ImVec4(1.f, 1.f, 1.f, 1.f);
        };

        using VecT = typename glm::vec<VecCount, float>;

        ImGuiSliderFlags flags = ImGuiSliderFlags_None;

        float afterTextGap = 4.f;
        float betweenInputsGap = 8.f;
        float labelWidth = 80.f;
        float floatStep = 1.f;
        float floatMin = 0.f;
        float floatMax = 0.f;
        bool readOnly = false;

        StringAtom label = ""_atom;
        std::array<Component, VecCount> components;

        std::expected<VecT, bool> drawAndProcess(VecT v, float availSpace)
        {
            if (!_isInited)
            {
                _firstValue = v;
                _isInited = true;
            }

            const auto originalValue = v;

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

            ImGui::PushID(label.c_str());

            ImGui::AlignTextToFramePadding();

            FixedLabel(label.c_str(), labelWidth);
            availSpace -= labelWidth;

            float* vec = reinterpret_cast<float*>(&v);

            int i = 0;
            for (const auto& component : components)
            {
                const float textWidth = ImGui::CalcTextSize(component.text.c_str()).x;

                // 3 - just components number
                // 2 - because for last one we don't need a gap
                constexpr float parties = static_cast<float>(VecCount);
                constexpr float partiesWithoutGap = static_cast<float>(VecCount) - 1.f;

                const float inputWidth = (availSpace
                                          - ((textWidth + afterTextGap) * parties
                                             + betweenInputsGap * partiesWithoutGap))
                                         / parties;

                ImGui::PushStyleColor(ImGuiCol_Text, component.color);
                ImGui::TextUnformatted(component.text.c_str());
                ImGui::PopStyleColor();

                ImGui::SameLine(0, afterTextGap);

                ImGui::PushID(i);
                ImGui::PushItemWidth(inputWidth);

                if (readOnly)
                {
                    ImGui::BeginDisabled(true);
                }
                ImGui::DragFloat("", vec + i, floatStep, floatMin, floatMax, "%.2f", flags);
                if (readOnly)
                {
                    ImGui::EndDisabled();
                }

                ImGui::PopItemWidth();
                ImGui::SameLine(0, i == 2 ? 0 : betweenInputsGap);
                ImGui::PopID();

                ++i;
            }
            ImGui::PopID();

            ImGui::PopStyleVar();

            ImGui::Dummy(ImVec2(0, 0));

            if (originalValue == v)
            {
                return std::unexpected(false);
            }

            return std::expected<VecT, bool>(v);
        }

    private:
        bool _isInited = false;
        VecT _firstValue = {};
    };

    using Vec4Control = VecControl<4>;
    using Vec3Control = VecControl<3>;
    using Vec2Control = VecControl<2>;

} // namespace Core
