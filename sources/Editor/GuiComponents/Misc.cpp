// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Misc.h"

#include "ImGui/imgui_internal.h"

namespace Core::Gui
{
    bool CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags, float paddings)
    {
        ImGui::Dummy({ 0, ImGui::GetStyle().ItemSpacing.y });
        using namespace ImGui;

        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
        {
            return false;
        }

        ImGuiID id = window->GetID(label);
        return TreeNodeBehavior(id, flags | ImGuiTreeNodeFlags_CollapsingHeader, label);
    }
} // namespace Core::Gui