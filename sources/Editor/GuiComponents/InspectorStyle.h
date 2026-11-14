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

#include "Core/Color.h"
#include "Foundation/Configs.h"
#include "ImGui/imgui.h"

#include <array>
#include <string_view>

namespace NX::Gui
{
    /// One look for every property panel. `ObjectPropertiesWindowEWC` is the reference, so
    /// generated rows and the hand-written ones it still has line up.
    struct InspectorStyle
    {
        /// Width of the label column of a property row.
        float labelWidth = 100.f;
        /// Label column for panels with long names.
        float wideLabelWidth = 150.f;
        /// Width of the index column of an array row.
        float indexWidth = 24.f;

        /// Header flags of a section (one section per reflected class).
        ImGuiTreeNodeFlags sectionFlags = ImGuiTreeNodeFlags_DefaultOpen;

        /// Axis labels and colours of vector rows (x, y, z, w).
        std::array<char, 4> axisNames = { 'X', 'Y', 'Z', 'W' };
        std::array<Core::Color4, 4> axisColors = {
            Foundation::Config::ColorRed,
            Foundation::Config::ColorGreen,
            Foundation::Config::ColorBlue,
            Foundation::Config::ColorYellow,
        };

        /// Shown instead of a value that is not there (empty optional, null pointer).
        std::string_view emptyValueText = "<none>";

        [[nodiscard]] static const InspectorStyle& Default() noexcept
        {
            static constexpr InspectorStyle style{};
            return style;
        }
    };
} // namespace NX::Gui
