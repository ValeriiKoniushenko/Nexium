// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/GuiComponents/EnumInputs.h"
#include "Editor/GuiComponents/Input.h"
#include "ImGuiFrameFixture.h"

#include "gmock/gmock.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

using namespace NX::Gui;
using testing::ElementsAre;

namespace
{
    class ValueEditorsTests : public EditorTestUtils::ImGuiFrameTest
    {
    };

    class TestEnumCombo : public EnumCombo
    {
    public:
        using EnumCombo::select;
    };

    class TestFlagsEditor : public FlagsEditor
    {
    public:
        using FlagsEditor::toggle;
    };

    std::vector<std::string> Names(const std::vector<EnumOption>& options)
    {
        std::vector<std::string> out;
        for (const auto& option : options)
        {
            out.push_back(option.name.toStdString());
        }
        return out;
    }

    constexpr std::uint64_t Bits(Flex flex)
    {
        return static_cast<std::uint64_t>(flex);
    }

    // ===================== Color4Input =====================

    TEST_F(ValueEditorsTests, Color4InputKeepsAlpha)
    {
        Color4Input input;
        input.initialize();
        const Core::Color4 color(10, 20, 30, 40);

        input.setInputtedData(color);

        EXPECT_EQ(input.getInputtedData(), color);
    }

    TEST_F(ValueEditorsTests, Color4InputDrawsWithoutEditing)
    {
        Color4Input input;
        input.initialize();
        input.setInputtedData(Core::Color4(1, 2, 3, 4));
        int fired = 0;
        auto subscription = input.onInput->subscribeAndGetID([&](const Core::Color4&) { ++fired; });

        resetLastItem();
        input.unhandledDraw();

        EXPECT_NE(lastItemId(), 0u);
        EXPECT_EQ(fired, 0);
        EXPECT_EQ(input.getInputtedData(), Core::Color4(1, 2, 3, 4));
        input.onInput->unsubscribe(subscription);
    }

    // ===================== Enum options =====================

    TEST(EnumOptions, FollowDeclarationOrder)
    {
        const auto options = EnumOptionsOf<Align>();

        EXPECT_THAT(Names(options), ElementsAre("None", "Left", "Right", "SpaceBetween", "Center",
                                                "Top", "Bottom"));
        EXPECT_EQ(options[4].value, static_cast<std::uint64_t>(Align::Center));
    }

    // ===================== EnumCombo =====================

    TEST_F(ValueEditorsTests, EnumComboPreviewsTheCurrentValue)
    {
        EnumCombo combo;
        combo.setOptions(EnumOptionsOf<Align>());

        combo.setValue(static_cast<std::uint64_t>(Align::Center));

        EXPECT_EQ(combo.getPreviewText(), "Center");
    }

    TEST_F(ValueEditorsTests, EnumComboShowsAnUnknownValueAsANumber)
    {
        EnumCombo combo;
        combo.setOptions(EnumOptionsOf<Align>());

        combo.setValue(42);

        EXPECT_EQ(combo.getPreviewText(), "<42>");
    }

    TEST_F(ValueEditorsTests, EnumComboSelectionChangesTheValueAndNotifies)
    {
        TestEnumCombo combo;
        combo.setOptions(EnumOptionsOf<Align>());
        std::vector<std::uint64_t> selected;
        auto subscription
            = combo.onSelect->subscribeAndGetID([&](std::uint64_t v) { selected.push_back(v); });

        combo.select(2);

        EXPECT_EQ(combo.getValue(), static_cast<std::uint64_t>(Align::Right));
        EXPECT_THAT(selected, ElementsAre(static_cast<std::uint64_t>(Align::Right)));
        combo.onSelect->unsubscribe(subscription);
    }

    TEST_F(ValueEditorsTests, EnumComboDrawsWithoutSelecting)
    {
        EnumCombo combo;
        combo.initialize();
        combo.setOptions(EnumOptionsOf<Align>());
        combo.setValue(static_cast<std::uint64_t>(Align::Top));
        int fired = 0;
        auto subscription = combo.onSelect->subscribeAndGetID([&](std::uint64_t) { ++fired; });

        resetLastItem();
        combo.unhandledDraw();

        EXPECT_NE(lastItemId(), 0u);
        EXPECT_EQ(fired, 0);
        combo.onSelect->unsubscribe(subscription);
    }

    // ===================== FlagsEditor =====================

    TEST_F(ValueEditorsTests, FlagsEditorListsOnlySingleBitEnumerators)
    {
        FlagsEditor flags;
        flags.setOptions(EnumOptionsOf<Flex>());

        EXPECT_THAT(Names(flags.getFlagOptions()), ElementsAre("Fixed", "FlexWidth", "FlexHeight"));
    }

    TEST_F(ValueEditorsTests, FlagsEditorPreviewJoinsTheSetFlags)
    {
        FlagsEditor flags;
        flags.setOptions(EnumOptionsOf<Flex>());

        flags.setValue(0);
        EXPECT_EQ(flags.getPreviewText(), "None");

        flags.setValue(Bits(Flex::FlexWidth) | Bits(Flex::FlexHeight));
        EXPECT_EQ(flags.getPreviewText(), "FlexWidth | FlexHeight");
    }

    TEST_F(ValueEditorsTests, FlagsEditorTogglesOneBitAndKeepsUnknownBits)
    {
        TestFlagsEditor flags;
        flags.setOptions(EnumOptionsOf<Flex>());
        constexpr std::uint64_t unknown = 1u << 10;
        flags.setValue(Bits(Flex::FlexWidth) | unknown);
        std::vector<std::uint64_t> changes;
        auto subscription
            = flags.onChange->subscribeAndGetID([&](std::uint64_t v) { changes.push_back(v); });

        EXPECT_EQ(flags.getPreviewText(), "FlexWidth | 0x400");

        flags.toggle(0); // Fixed on
        flags.toggle(1); // FlexWidth off

        EXPECT_EQ(flags.getValue(), Bits(Flex::Fixed) | unknown);
        EXPECT_THAT(changes, ElementsAre(Bits(Flex::Fixed) | Bits(Flex::FlexWidth) | unknown,
                                         Bits(Flex::Fixed) | unknown));
        flags.onChange->unsubscribe(subscription);
    }

    TEST_F(ValueEditorsTests, FlagsEditorWithoutAZeroEnumeratorStillNamesTheEmptySet)
    {
        FlagsEditor flags;
        flags.setOptions({ { "A"_atom, 1 }, { "B"_atom, 2 } });

        flags.setValue(0);

        EXPECT_EQ(flags.getPreviewText(), "None");
    }

    // ===================== PathInput =====================

    TEST_F(ValueEditorsTests, PathInputRoundTripsAGenericPath)
    {
        PathInput input;
        input.initialize();
        const std::filesystem::path path = "assets/textures/brick wall.png";

        input.setPath(path);

        EXPECT_EQ(input.getInputtedData(), "assets/textures/brick wall.png");
        EXPECT_EQ(input.getPath(), path);
    }

    TEST_F(ValueEditorsTests, PathInputDrawsWithoutEditing)
    {
        PathInput input;
        input.initialize();
        input.setPath("data/scene.json");
        int fired = 0;
        auto subscription = input.onPathInput->subscribeAndGetID(
            [&](const std::filesystem::path&) { ++fired; });

        resetLastItem();
        input.unhandledDraw();

        EXPECT_NE(lastItemId(), 0u);
        EXPECT_EQ(fired, 0);
        input.onPathInput->unsubscribe(subscription);
    }
} // namespace
