// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"

#include "gtest/gtest.h"

namespace
{
    using namespace NX::Gui;

    class HorizontalLayoutTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            ImGui::CreateContext();
            auto& io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.DisplaySize = glm::vec2(800, 600);
            unsigned char* pixels = nullptr;
            int width = 0;
            int height = 0;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
            ImGui::GetStyle().ItemSpacing = glm::vec2(8, 6);
            ImGui::NewFrame();
            ImGui::SetNextWindowPos(glm::vec2(0));
            ImGui::SetNextWindowSize(glm::vec2(800, 600));
            ImGui::Begin("HorizontalLayoutTests", nullptr, ImGuiWindowFlags_NoSavedSettings);
        }

        void TearDown() override
        {
            ImGui::End();
            ImGui::EndFrame();
            ImGui::DestroyContext();
        }

        static Button* addButton(HorizontalLayout& row, float width, Flex flex = Flex::Fixed)
        {
            auto* button = row.addChildComponent<Button>("Button"_atom);
            button->initialize();
            button->setSize(glm::vec2(width, 20));
            button->setFlex(flex);
            return button;
        }

        static void draw(HorizontalLayout& row)
        {
            row.initialize();
            row.unhandledDraw();
        }
    };

    TEST_F(HorizontalLayoutTests, FlexChildrenFillThePaddedContentWidth)
    {
        HorizontalLayout row;
        row.setWidth(301);
        row.setPaddings(10, 20);
        auto* first = addButton(row, 50, Flex::FlexWidth);
        auto* last = addButton(row, 50, Flex::FlexWidth);
        const auto origin = ImGui::GetCursorScreenPos();

        draw(row);

        EXPECT_FLOAT_EQ(first->getWidth() + last->getWidth() + 8, 271);
        EXPECT_FLOAT_EQ(first->getGlobalBounds().topLeft.x, origin.x + 10);
        EXPECT_FLOAT_EQ(last->getGlobalBounds().bottomRight.x, origin.x + 281);
    }

    TEST_F(HorizontalLayoutTests, SingleFlexChildHasNoInterItemGap)
    {
        HorizontalLayout row;
        row.setWidth(300);
        auto* button = addButton(row, 50, Flex::FlexWidth);

        draw(row);

        EXPECT_FLOAT_EQ(button->getWidth(), 300);
    }

    TEST_F(HorizontalLayoutTests, ReservesEveryNonWidthFlexChild)
    {
        for (auto flex : { Flex::Fixed, Flex::None, Flex::FlexHeight })
        {
            HorizontalLayout row;
            row.setWidth(300);
            addButton(row, 100, flex);
            auto* last = addButton(row, 50, Flex::FlexWidth);

            draw(row);

            EXPECT_FLOAT_EQ(last->getWidth(), 192);
        }
    }

    TEST_F(HorizontalLayoutTests, AlignmentRespectsAsymmetricPadding)
    {
        for (auto align : { Align::Left, Align::Right, Align::Center, Align::SpaceBetween })
        {
            HorizontalLayout row;
            row.setWidth(300);
            row.setPaddings(10, 20);
            row.setHorizontalAlign(align);
            auto* first = addButton(row, 100);
            auto* last = addButton(row, 100);
            const auto origin = ImGui::GetCursorScreenPos();

            draw(row);

            const float offset = align == Align::Right ? 62.f : align == Align::Center ? 31.f : 0.f;
            EXPECT_FLOAT_EQ(first->getGlobalBounds().topLeft.x, origin.x + 10 + offset);
            EXPECT_FLOAT_EQ(last->getGlobalBounds().bottomRight.x,
                            origin.x + (align == Align::SpaceBetween ? 280.f : 218.f + offset));
        }
    }

    TEST_F(HorizontalLayoutTests, DisabledChildrenDoNotReserveSpace)
    {
        HorizontalLayout row;
        row.setWidth(300);
        auto* first = addButton(row, 100, Flex::FlexWidth);
        addButton(row, 100)->setEnabled(false);
        auto* last = addButton(row, 100, Flex::FlexWidth);
        addButton(row, 100)->setEnabled(false);

        draw(row);

        EXPECT_FLOAT_EQ(first->getWidth() + last->getWidth() + 8, 300);
        EXPECT_FLOAT_EQ(ImGui::GetItemRectSize().x, 300);
    }

    TEST_F(HorizontalLayoutTests, FitContentCountsOnlyVisibleGapsAndIncludesPadding)
    {
        HorizontalLayout row;
        row.setFitContent(true);
        row.setPaddings(10, 20);
        auto* first = addButton(row, 100);
        auto* last = addButton(row, 100);

        EXPECT_FLOAT_EQ(row.getWidth(), 238);
        first->setEnabled(false);
        EXPECT_FLOAT_EQ(row.getWidth(), 130);
        last->setEnabled(false);
        EXPECT_FLOAT_EQ(row.getWidth(), 30);
        row.setPaddings(0, 0);
        EXPECT_FLOAT_EQ(row.getWidth(), 0);
    }

    TEST_F(HorizontalLayoutTests, NestedFitContentFlexRowKeepsItsWidthAcrossDraws)
    {
        HorizontalLayout outer;
        outer.setFitContent(true);
        auto* inner = outer.addChildComponent<HorizontalLayout>();
        inner->setFitContent(true);
        addButton(*inner, 100, Flex::FlexWidth);
        addButton(*inner, 100, Flex::FlexWidth);

        draw(outer);
        draw(outer);

        EXPECT_FLOAT_EQ(inner->getWidth(), 208);
        EXPECT_FLOAT_EQ(outer.getWidth(), 208);
    }

    TEST_F(HorizontalLayoutTests, RegistersRowBoundsAndRestoresTheStartingColumn)
    {
        HorizontalLayout row;
        row.setWidth(300);
        row.setPaddings(10, 20, 4, 6);
        auto* button = addButton(row, 100);
        ImGui::Indent(25);
        const auto origin = ImGui::GetCursorPos();
        const auto screenOrigin = ImGui::GetCursorScreenPos();

        draw(row);

        EXPECT_FLOAT_EQ(ImGui::GetCursorPosX(), origin.x);
        EXPECT_FLOAT_EQ(ImGui::GetCursorPosY(), origin.y + 30 + 6);
        EXPECT_FLOAT_EQ(ImGui::GetItemRectMin().x, screenOrigin.x);
        EXPECT_FLOAT_EQ(ImGui::GetItemRectSize().x, 300);
        EXPECT_FLOAT_EQ(ImGui::GetItemRectSize().y, 30);
        EXPECT_FLOAT_EQ(button->getGlobalBounds().topLeft.y, screenOrigin.y + 4);
        ImGui::Unindent(25);
    }
} // namespace
