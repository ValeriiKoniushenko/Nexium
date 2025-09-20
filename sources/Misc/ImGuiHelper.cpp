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

namespace ImGui
{
    Core::StringAtom ToString_ImGuiCol_(ImGuiCol_ val)
    {
        // clang-format off
        if (val == ImGuiCol_Text) return "Text"_atom;
        if (val == ImGuiCol_TextDisabled) return "TextDisabled"_atom;
        if (val == ImGuiCol_WindowBg) return "WindowBg"_atom;
        if (val == ImGuiCol_ChildBg) return "ChildBg"_atom;
        if (val == ImGuiCol_PopupBg) return "PopupBg"_atom;
        if (val == ImGuiCol_Border) return "Border"_atom;
        if (val == ImGuiCol_BorderShadow) return "BorderShadow"_atom;
        if (val == ImGuiCol_FrameBg) return "FrameBg"_atom;
        if (val == ImGuiCol_FrameBgHovered) return "FrameBgHovered"_atom;
        if (val == ImGuiCol_FrameBgActive) return "FrameBgActive"_atom;
        if (val == ImGuiCol_TitleBg) return "TitleBg"_atom;
        if (val == ImGuiCol_TitleBgActive) return "TitleBgActive"_atom;
        if (val == ImGuiCol_TitleBgCollapsed) return "TitleBgCollapsed"_atom;
        if (val == ImGuiCol_MenuBarBg) return "MenuBarBg"_atom;
        if (val == ImGuiCol_ScrollbarBg) return "ScrollbarBg"_atom;
        if (val == ImGuiCol_ScrollbarGrab) return "ScrollbarGrab"_atom;
        if (val == ImGuiCol_ScrollbarGrabHovered) return "ScrollbarGrabHovered"_atom;
        if (val == ImGuiCol_ScrollbarGrabActive) return "ScrollbarGrabActive"_atom;
        if (val == ImGuiCol_CheckMark) return "CheckMark"_atom;
        if (val == ImGuiCol_SliderGrab) return "SliderGrab"_atom;
        if (val == ImGuiCol_SliderGrabActive) return "SliderGrabActive"_atom;
        if (val == ImGuiCol_Button) return "Button"_atom;
        if (val == ImGuiCol_ButtonHovered) return "ButtonHovered"_atom;
        if (val == ImGuiCol_ButtonActive) return "ButtonActive"_atom;
        if (val == ImGuiCol_Header) return "Header"_atom;
        if (val == ImGuiCol_HeaderHovered) return "HeaderHovered"_atom;
        if (val == ImGuiCol_HeaderActive) return "HeaderActive"_atom;
        if (val == ImGuiCol_Separator) return "Separator"_atom;
        if (val == ImGuiCol_SeparatorHovered) return "SeparatorHovered"_atom;
        if (val == ImGuiCol_SeparatorActive) return "SeparatorActive"_atom;
        if (val == ImGuiCol_ResizeGrip) return "ResizeGrip"_atom;
        if (val == ImGuiCol_ResizeGripHovered) return "ResizeGripHovered"_atom;
        if (val == ImGuiCol_ResizeGripActive) return "ResizeGripActive"_atom;
        if (val == ImGuiCol_InputTextCursor) return "InputTextCursor"_atom;
        if (val == ImGuiCol_TabHovered) return "TabHovered"_atom;
        if (val == ImGuiCol_Tab) return "Tab"_atom;
        if (val == ImGuiCol_TabSelected) return "TabSelected"_atom;
        if (val == ImGuiCol_TabSelectedOverline) return "TabSelectedOverline"_atom;
        if (val == ImGuiCol_TabDimmed) return "TabDimmed"_atom;
        if (val == ImGuiCol_TabDimmedSelected) return "TabDimmedSelected"_atom;
        if (val == ImGuiCol_TabDimmedSelectedOverline) return "TabDimmedSelectedOverline"_atom;
        if (val == ImGuiCol_DockingPreview) return "DockingPreview"_atom;
        if (val == ImGuiCol_DockingEmptyBg) return "DockingEmptyBg"_atom;
        if (val == ImGuiCol_PlotLines) return "PlotLines"_atom;
        if (val == ImGuiCol_PlotLinesHovered) return "PlotLinesHovered"_atom;
        if (val == ImGuiCol_PlotHistogram) return "PlotHistogram"_atom;
        if (val == ImGuiCol_PlotHistogramHovered) return "PlotHistogramHovered"_atom;
        if (val == ImGuiCol_TableHeaderBg) return "TableHeaderBg"_atom;
        if (val == ImGuiCol_TableBorderStrong) return "TableBorderStrong"_atom;
        if (val == ImGuiCol_TableBorderLight) return "TableBorderLight"_atom;
        if (val == ImGuiCol_TableRowBg) return "TableRowBg"_atom;
        if (val == ImGuiCol_TableRowBgAlt) return "TableRowBgAlt"_atom;
        if (val == ImGuiCol_TextLink) return "TextLink"_atom;
        if (val == ImGuiCol_TextSelectedBg) return "TextSelectedBg"_atom;
        if (val == ImGuiCol_TreeLines) return "TreeLines"_atom;
        if (val == ImGuiCol_DragDropTarget) return "DragDropTarget"_atom;
        if (val == ImGuiCol_NavCursor) return "NavCursor"_atom;
        if (val == ImGuiCol_NavWindowingHighlight) return "NavWindowingHighlight"_atom;
        if (val == ImGuiCol_NavWindowingDimBg) return "NavWindowingDimBg"_atom;
        if (val == ImGuiCol_ModalWindowDimBg) return "ModalWindowDimBg"_atom;
        // clang-format on

        Assert(false);

        return ""_atom;
    }

    Core::StringAtom ToString_ImGuiStyleVar_(ImGuiStyleVar_ val)
    {
        // clang-format off
        if (val == ImGuiStyleVar_Alpha) return "Alpha"_atom;
        if (val == ImGuiStyleVar_DisabledAlpha) return "DisabledAlpha"_atom;
        if (val == ImGuiStyleVar_WindowPadding) return "WindowPadding"_atom;
        if (val == ImGuiStyleVar_WindowRounding) return "WindowRounding"_atom;
        if (val == ImGuiStyleVar_WindowBorderSize) return "WindowBorderSize"_atom;
        if (val == ImGuiStyleVar_WindowMinSize) return "WindowMinSize"_atom;
        if (val == ImGuiStyleVar_WindowTitleAlign) return "WindowTitleAlign"_atom;
        if (val == ImGuiStyleVar_ChildRounding) return "ChildRounding"_atom;
        if (val == ImGuiStyleVar_ChildBorderSize) return "ChildBorderSize"_atom;
        if (val == ImGuiStyleVar_PopupRounding) return "PopupRounding"_atom;
        if (val == ImGuiStyleVar_PopupBorderSize) return "PopupBorderSize"_atom;
        if (val == ImGuiStyleVar_FramePadding) return "FramePadding"_atom;
        if (val == ImGuiStyleVar_FrameRounding) return "FrameRounding"_atom;
        if (val == ImGuiStyleVar_FrameBorderSize) return "FrameBorderSize"_atom;
        if (val == ImGuiStyleVar_ItemSpacing) return "ItemSpacing"_atom;
        if (val == ImGuiStyleVar_ItemInnerSpacing) return "ItemInnerSpacing"_atom;
        if (val == ImGuiStyleVar_IndentSpacing) return "IndentSpacing"_atom;
        if (val == ImGuiStyleVar_CellPadding) return "CellPadding"_atom;
        if (val == ImGuiStyleVar_ScrollbarSize) return "ScrollbarSize"_atom;
        if (val == ImGuiStyleVar_ScrollbarRounding) return "ScrollbarRounding"_atom;
        if (val == ImGuiStyleVar_GrabMinSize) return "GrabMinSize"_atom;
        if (val == ImGuiStyleVar_GrabRounding) return "GrabRounding"_atom;
        if (val == ImGuiStyleVar_ImageBorderSize) return "ImageBorderSize"_atom;
        if (val == ImGuiStyleVar_TabRounding) return "TabRounding"_atom;
        if (val == ImGuiStyleVar_TabBorderSize) return "TabBorderSize"_atom;
        if (val == ImGuiStyleVar_TabMinWidthBase) return "TabMinWidthBase"_atom;
        if (val == ImGuiStyleVar_TabMinWidthShrink) return "TabMinWidthShrink"_atom;
        if (val == ImGuiStyleVar_TabBarBorderSize) return "TabBarBorderSize"_atom;
        if (val == ImGuiStyleVar_TabBarOverlineSize) return "TabBarOverlineSize"_atom;
        if (val == ImGuiStyleVar_TableAngledHeadersAngle) return "TableAngledHeadersAngle"_atom;
        if (val == ImGuiStyleVar_TableAngledHeadersTextAlign) return "TableAngledHeadersTextAlign"_atom;
        if (val == ImGuiStyleVar_TreeLinesSize) return "TreeLinesSize"_atom;
        if (val == ImGuiStyleVar_TreeLinesRounding) return "TreeLinesRounding"_atom;
        if (val == ImGuiStyleVar_ButtonTextAlign) return "ButtonTextAlign"_atom;
        if (val == ImGuiStyleVar_SelectableTextAlign) return "SelectableTextAlign"_atom;
        if (val == ImGuiStyleVar_SeparatorTextBorderSize) return "SeparatorTextBorderSize"_atom;
        if (val == ImGuiStyleVar_SeparatorTextAlign) return "SeparatorTextAlign"_atom;
        if (val == ImGuiStyleVar_SeparatorTextPadding) return "SeparatorTextPadding"_atom;
        if (val == ImGuiStyleVar_DockingSeparatorSize) return "DockingSeparatorSize"_atom;
        // clang-format on

        Assert(false);

        return ""_atom;
    }

} // namespace ImGui

namespace Core
{

    bool InputText(const StringAtom& label, std::string& value, float size, int flags)
    {
        ImGui::PushItemWidth(size);
        const auto out = ImGui::InputText(("##" + label).c_str(), &value, flags);
        ImGui::PopItemWidth();
        return out;
    }

#if 0
    void FixedLabel(const char* label, float size)
    {
    #if defined(DEBUG)
        Assert(ImGui::CalcTextSize(label).x < size);
    #endif
        ImGui::TextUnformatted(label);
        ImGui::SameLine(0, 0);
        ImGui::Dummy(glm::vec2(size - ImGui::CalcTextSize(label).x, 0));
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

    bool ToggleButton(const char* label, bool cond, const glm::vec4& onColor,
                      const glm::vec4& offColor)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, cond ? onColor : offColor);
        const bool isPressed = ImGui::Button(label);
        ImGui::PopStyleColor();

        return isPressed;
    }

    bool ToggleButton(const char* label, bool cond)
    {
        return ToggleButton(label, cond, ImGui::GetStyle().Colors[ImGuiCol_Button],
                            glm::vec4(0, 0, 0, 0));
    }

    bool ButtonAndInputTextRO(StringAtom label, StringAtom value, float labelSize, float fullSize)
    {
        const auto& style = ImGui::GetStyle();

        labelSize -= style.FramePadding.x * 2.f;

        ImGui::PushID(static_cast<int>(label.makeHash()));
        const bool isPressed = ImGui::Button(label.data(), glm::vec2(labelSize, 0.f));

        ImGui::SameLine(0, 8.f);

        InputTextRO(std::move(value), fullSize - labelSize - 8.f);
        ImGui::PopID();

        return isPressed;
    }

#endif

} // namespace Core
