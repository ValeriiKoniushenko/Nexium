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

namespace SW
{

    struct Vec3Control
    {
        struct Component
        {
            Core::StringAtom text = ""_atom;
            ImVec4 color = ImVec4(1.f, 1.f, 1.f, 1.f);
        };

        ImGuiSliderFlags flags = ImGuiSliderFlags_None;

        float afterTextGap = 4.f;
        float betweenInputsGap = 8.f;
        float labelWidth = 80.f;
        float floatStep = 1.f;
        float floatMin = 0.f;
        float floatMax = 0.f;
        bool readOnly = false;

        Core::StringAtom label = ""_atom;
        std::array<Component, 3> components = { Component(), Component(), Component() };

        void draw(glm::vec3& v, float availSpace);
    };

    void FixedLabel(const char* label, float size);
    void InputTextRO(Core::StringAtom value, float size);
    void LabelAndInputTextRO(Core::StringAtom label, Core::StringAtom value, float labelSize,
                             float fullSize);

    template<class T>
    void LabelAndInputTextRO(Core::StringAtom label, T value, float labelSize, float fullSize)
    {
        LabelAndInputTextRO(std::move(label), Core::StringAtom::MakeFrom(value), labelSize,
                            fullSize);
    }

    void LabelAndCheckboxRO(Core::StringAtom label, bool v, float labelSize);

    bool VectorCombo(Core::StringAtom label, int* current, std::vector<Core::StringAtom>& data);

} // namespace SW
