// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/PropertyGrid.h"
#include "Editor/GuiComponents/VecInput.h"
#include "ImGuiFrameFixture.h"
#include "NxWorld/Framework/World.h"

using namespace NX::Gui;

namespace
{
    // Labels, order and limits of the hand-written lighting panel that the generated one
    // replaces (WorldSettings.cpp before the reflection POC).
    constexpr const char* HandWrittenLightingPanel = R"(Row "Light color" #color Color3
Row "Ambient strength" #ambientStrength Number
Row "Min light strength" #minLightStrength Number
Row "Specular strength" #specularStrength Number
Row "Specular pow" #specularPow Number
Row "Sun light direction" #sunDirection Vec3
)";

    class WorldSettingsGridTests : public EditorTestUtils::ImGuiFrameTest
    {
    protected:
        void SetUp() override
        {
            ImGuiFrameTest::SetUp();
            grid = PropertyGrid::Create();
            grid->setSections(PropertySections::Flat);
            grid->setTarget([this] { return NX::Reflection::ObjectRef(lightning); });
            grid->unhandledDraw();
        }

        void TearDown() override
        {
            grid = nullptr;
            ImGuiFrameTest::TearDown();
        }

        NX::LightningProps lightning;
        PropertyGrid::Ptr grid;
    };

    TEST_F(WorldSettingsGridTests, LightingRowsMatchTheHandWrittenPanel)
    {
        EXPECT_EQ(grid->dump(), HandWrittenLightingPanel);
    }

    TEST_F(WorldSettingsGridTests, LightingLimitsMatchTheHandWrittenPanel)
    {
        const auto* pow = grid->findRow("specularPow"_atom)->findFirstChildOf<FloatInput>();
        EXPECT_FLOAT_EQ(pow->getMin(), 0.01f);
        EXPECT_FLOAT_EQ(pow->getMax(), 4096.f);
        EXPECT_FLOAT_EQ(pow->getStep(), 2.f);

        const auto* ambient = grid->findRow("ambientStrength"_atom)->findFirstChildOf<FloatInput>();
        EXPECT_FLOAT_EQ(ambient->getMin(), 0.f);
        EXPECT_FLOAT_EQ(ambient->getMax(), 10.f);
        EXPECT_FLOAT_EQ(ambient->getStep(), 0.05f);

        const auto* sun = grid->findRow("sunDirection"_atom)->findFirstChildOf<Float3Input>();
        for (const auto* axis : sun->inputs)
        {
            EXPECT_FLOAT_EQ(axis->getMin(), -1.f);
            EXPECT_FLOAT_EQ(axis->getMax(), 1.f);
            EXPECT_FLOAT_EQ(axis->getStep(), 0.01f);
        }
    }

    TEST_F(WorldSettingsGridTests, EditingALightingRowWritesTheWorldSettings)
    {
        grid->findRow("specularPow"_atom)->findFirstChildOf<FloatInput>()->onInput->trigger(64.f);

        EXPECT_FLOAT_EQ(lightning.specularPow, 64.f);
    }
} // namespace
