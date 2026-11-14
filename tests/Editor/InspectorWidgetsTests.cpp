// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/GuiComponents/CollapsingSection.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/InspectorStyle.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/VecInput.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Editor/Windows/ObjectPropertiesWindow.h"
#include "ImGuiFrameFixture.h"

#include <cstdint>
#include <limits>

using namespace NX::Gui;
using EditorTestUtils::Probe;

namespace
{
    class InspectorWidgetsTests : public EditorTestUtils::ImGuiFrameTest
    {
    protected:
        struct Tree
        {
            VerticalLayout::Ptr root;
            Probe* first = nullptr;
            Probe* second = nullptr;
        };

        /// root("Grid") -> row("speed") -> [first, second]; ids are stable when `stable`.
        static Tree makeTree(bool stable)
        {
            Tree tree;
            tree.root = VerticalLayout::Create();
            auto* row = tree.root->addChildComponent<HorizontalLayout>();
            tree.first = row->addChildComponent<Probe>();
            tree.second = row->addChildComponent<Probe>();
            if (stable)
            {
                tree.root->setStableId("Grid"_atom);
                row->setStableId("speed"_atom);
            }
            return tree;
        }

        static void burnWidgetIds()
        {
            for (int i = 0; i < 5; ++i)
            {
                Label label;
                label.initialize();
            }
        }
    };

    // ===================== Stable ids =====================

    TEST_F(InspectorWidgetsTests, StableIdsDoNotDependOnCreationOrder)
    {
        Tree a = makeTree(true);
        a.root->unhandledDraw();
        burnWidgetIds();
        Tree b = makeTree(true);
        b.root->unhandledDraw();

        EXPECT_NE(a.first->lastId, 0u);
        EXPECT_EQ(a.first->lastId, b.first->lastId);
        EXPECT_EQ(a.second->lastId, b.second->lastId);
        EXPECT_NE(a.first->lastId, a.second->lastId);
    }

    TEST_F(InspectorWidgetsTests, CounterIdsDependOnCreationOrder)
    {
        Tree a = makeTree(false);
        a.root->unhandledDraw();
        burnWidgetIds();
        Tree b = makeTree(false);
        b.root->unhandledDraw();

        EXPECT_NE(a.first->lastId, b.first->lastId);
    }

    TEST_F(InspectorWidgetsTests, DifferentStableIdsGiveDifferentIds)
    {
        Tree a = makeTree(true);
        Tree b = makeTree(true);
        b.root->setStableId("OtherGrid"_atom);

        a.root->unhandledDraw();
        b.root->unhandledDraw();

        EXPECT_NE(a.first->lastId, b.first->lastId);
    }

    TEST_F(InspectorWidgetsTests, StableChildIdsSurviveRebuildingTheChildren)
    {
        auto scope = VerticalLayout::Create();
        scope->setStableChildIds(true);
        auto* before = scope->addChildComponent<HorizontalLayout>()->addChildComponent<Probe>();
        scope->unhandledDraw();
        const ImGuiID original = before->lastId;

        scope->removeAllChildren();
        burnWidgetIds();
        auto* after = scope->addChildComponent<HorizontalLayout>()->addChildComponent<Probe>();
        scope->unhandledDraw();

        EXPECT_TRUE(scope->hasStableIdScope());
        EXPECT_FALSE(scope->getStableId().has_value());
        EXPECT_TRUE(after->hasStableIdScope());
        EXPECT_EQ(after->lastId, original);
    }

    // ===================== NumInput =====================

    static_assert(ImGuiDataTypeOf<float>() == ImGuiDataType_Float);
    static_assert(ImGuiDataTypeOf<double>() == ImGuiDataType_Double);
    static_assert(ImGuiDataTypeOf<std::int8_t>() == ImGuiDataType_S8);
    static_assert(ImGuiDataTypeOf<std::uint8_t>() == ImGuiDataType_U8);
    static_assert(ImGuiDataTypeOf<std::int16_t>() == ImGuiDataType_S16);
    static_assert(ImGuiDataTypeOf<std::uint16_t>() == ImGuiDataType_U16);
    static_assert(ImGuiDataTypeOf<int>() == ImGuiDataType_S32);
    static_assert(ImGuiDataTypeOf<std::uint32_t>() == ImGuiDataType_U32);
    static_assert(ImGuiDataTypeOf<std::int64_t>() == ImGuiDataType_S64);
    static_assert(ImGuiDataTypeOf<std::uint64_t>() == ImGuiDataType_U64);
    static_assert(ImGuiDataTypeOf<long>() == (sizeof(long) == 8 ? ImGuiDataType_S64
                                                                : ImGuiDataType_S32));
    static_assert(ImGuiDataTypeOf<std::size_t>() == (sizeof(std::size_t) == 8
                                                          ? ImGuiDataType_U64
                                                          : ImGuiDataType_U32));

    template<class T>
    class NumInputTypedTests : public EditorTestUtils::ImGuiFrameTest
    {
    };

    using NumberTypes = ::testing::Types<float, double, int, std::int64_t, std::uint16_t,
                                         std::uint8_t, std::size_t>;
    TYPED_TEST_SUITE(NumInputTypedTests, NumberTypes);

    TYPED_TEST(NumInputTypedTests, DrawsAnItemAndNeverEditsTheValue)
    {
        using T = TypeParam;
        const T value = std::numeric_limits<T>::max() / T(3);
        NumInput<T> input;
        input.initialize();
        input.setInputtedData(value);
        int fired = 0;
        auto subscription = input.onInput->subscribeAndGetID([&](T) { ++fired; });

        this->resetLastItem();
        input.unhandledDraw();

        EXPECT_NE(this->lastItemId(), 0u) << "nothing was submitted to ImGui";
        EXPECT_EQ(input.getInputtedData(), value);
        EXPECT_EQ(fired, 0);
        input.onInput->unsubscribe(subscription);
    }

    // ===================== CollapsingSection =====================

    TEST_F(InspectorWidgetsTests, SectionDrawsItsRowsWhileOpen)
    {
        CollapsingSection section("Lighting"_atom);
        auto* probe = section.addRow<Probe>();

        section.unhandledDraw();

        EXPECT_TRUE(section.isOpen());
        EXPECT_EQ(probe->draws, 1);
        EXPECT_EQ(section.getTitle(), "Lighting");
    }

    TEST_F(InspectorWidgetsTests, ClosedSectionSkipsItsRows)
    {
        CollapsingSection section("Lighting"_atom);
        auto* probe = section.addRow<Probe>();
        section.setOpen(false);

        section.unhandledDraw();

        EXPECT_FALSE(section.isOpen());
        EXPECT_EQ(probe->draws, 0);
    }

    TEST_F(InspectorWidgetsTests, SectionWithoutDefaultOpenStartsClosed)
    {
        CollapsingSection section("Lighting"_atom);
        section.setHeaderFlags(ImGuiTreeNodeFlags_None);
        auto* probe = section.addRow<Probe>();

        section.unhandledDraw();

        EXPECT_FALSE(section.isOpen());
        EXPECT_EQ(probe->draws, 0);
    }

    TEST_F(InspectorWidgetsTests, OpenStateSurvivesARebuildWithTheSameStableId)
    {
        {
            CollapsingSection section("Lighting"_atom);
            section.setStableId("Lighting"_atom);
            section.setOpen(false);
            section.unhandledDraw();
        }
        nextFrame();

        CollapsingSection rebuilt("Lighting"_atom);
        rebuilt.setStableId("Lighting"_atom);
        auto* probe = rebuilt.addRow<Probe>();
        rebuilt.unhandledDraw();

        EXPECT_FALSE(rebuilt.isOpen());
        EXPECT_EQ(probe->draws, 0);
    }

    TEST_F(InspectorWidgetsTests, RenamingASectionKeepsItsOpenState)
    {
        CollapsingSection section("Lighting"_atom);
        section.setStableId("Lighting"_atom);
        section.setOpen(false);
        section.unhandledDraw();
        nextFrame();

        section.setTitle("Light"_atom);
        section.unhandledDraw();

        EXPECT_FALSE(section.isOpen());
    }

    TEST_F(InspectorWidgetsTests, SectionHeightIsTheSpaceItTakes)
    {
        for (const bool open : { true, false })
        {
            CollapsingSection section("Lighting"_atom);
            section.addRow<Probe>();
            section.addRow<Probe>();
            section.setOpen(open);

            const float before = ImGui::GetCursorPosY();
            section.unhandledDraw();
            const float advance = ImGui::GetCursorPosY() - before;

            EXPECT_FLOAT_EQ(advance, section.getHeight() + ImGui::GetStyle().ItemSpacing.y)
                << "open=" << open;
            nextFrame();
        }
    }

    TEST_F(InspectorWidgetsTests, OpenSectionIsTallerByItsRows)
    {
        CollapsingSection open("A"_atom);
        open.addRow<Probe>();
        open.unhandledDraw();
        CollapsingSection closed("B"_atom);
        closed.addRow<Probe>();
        closed.setOpen(false);
        closed.unhandledDraw();

        EXPECT_FLOAT_EQ(open.getHeight() - closed.getHeight(),
                        10.f + ImGui::GetStyle().ItemSpacing.y);
    }

    // ===================== InspectorStyle =====================

    TEST(InspectorStyle, MatchesTheObjectPropertiesReference)
    {
        const InspectorStyle& style = InspectorStyle::Default();

        EXPECT_FLOAT_EQ(style.labelWidth, NX::ObjectPropertiesWindowEWC::defaultLabelWidth);
        EXPECT_FLOAT_EQ(style.wideLabelWidth, NX::ObjectPropertiesWindowEWC::defaultLabelWidthBig);
        EXPECT_EQ(style.sectionFlags, ImGuiTreeNodeFlags_DefaultOpen);
        EXPECT_EQ(style.axisNames, (std::array<char, 4>{ 'X', 'Y', 'Z', 'W' }));
        EXPECT_EQ(style.axisColors[0], Foundation::Config::ColorRed);
        EXPECT_EQ(style.axisColors[3], Foundation::Config::ColorYellow);
    }

    TEST_F(InspectorWidgetsTests, VecInputTakesItsAxisLabelsFromTheStyle)
    {
        Float3Input input;
        input.initialize();
        const InspectorStyle& style = InspectorStyle::Default();

        for (std::size_t i = 0; i < 3; ++i)
        {
            EXPECT_EQ(input.labels[i]->getTextColor(), style.axisColors[i]);
            EXPECT_EQ(input.labels[i]->getText()[0], style.axisNames[i]);
        }
    }
} // namespace
