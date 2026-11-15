// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/ContainerEditors.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "ImGuiFrameFixture.h"

#include "gmock/gmock.h"
#include <format>
#include <string>
#include <utility>
#include <vector>

using namespace NX::Gui;
using EditorTestUtils::Probe;
using testing::ElementsAre;

namespace
{
    class ContainerEditorsTests : public EditorTestUtils::ImGuiFrameTest
    {
    protected:
        template<class T>
        static std::vector<std::size_t> Tags(const std::vector<T>& rows)
        {
            std::vector<std::size_t> out;
            for (const auto& row : rows)
            {
                if (const auto* probe = row.row->template findFirstChildOf<Probe>())
                {
                    out.push_back(probe->tag);
                }
            }
            return out;
        }
    };

    // ===================== OptionalEditor =====================

    TEST_F(ContainerEditorsTests, OptionalShowsTheValueOnlyWhileEngaged)
    {
        OptionalEditor optional;
        auto* probe = optional.setValueWidget<Probe>();

        optional.setEngaged(false);
        optional.unhandledDraw();
        EXPECT_EQ(probe->draws, 0);
        EXPECT_TRUE(optional.getEmptyLabel()->isEnabled());
        EXPECT_EQ(optional.getEmptyLabel()->getText(), InspectorStyle::Default().emptyValueText);
        EXPECT_FALSE(optional.getToggle()->getValue());

        optional.setEngaged(true);
        optional.unhandledDraw();
        EXPECT_EQ(probe->draws, 1);
        EXPECT_FALSE(optional.getEmptyLabel()->isEnabled());
        EXPECT_TRUE(optional.getToggle()->getValue());
    }

    TEST_F(ContainerEditorsTests, OptionalToggleReportsTheNewStateWithoutApplyingIt)
    {
        OptionalEditor optional;
        optional.setValueWidget<Probe>();
        optional.setEngaged(false);
        std::vector<bool> changes;
        auto subscription
            = optional.onEngagedChange->subscribeAndGetID([&](bool v) { changes.push_back(v); });

        optional.getToggle()->onChange->trigger(true);

        EXPECT_THAT(changes, ElementsAre(true));
        EXPECT_FALSE(optional.isEngaged()) << "the model decides; the next pull applies it";
        optional.onEngagedChange->unsubscribe(subscription);
    }

    TEST_F(ContainerEditorsTests, OptionalReplacesItsValueWidget)
    {
        OptionalEditor optional;
        optional.setValueWidget<Probe>();
        auto* second = optional.setValueWidget<Probe>(std::size_t{ 7 });

        EXPECT_EQ(optional.getValueWidget(), second);
        std::size_t probes = 0;
        for (const auto& child : optional.getChildren())
        {
            probes += child->tryCastTo<Probe>() != nullptr ? 1 : 0;
        }
        EXPECT_EQ(probes, 1u);
    }

    // ===================== ArrayEditor =====================

    class ArrayEditorTests : public ContainerEditorsTests
    {
    protected:
        void SetUp() override
        {
            ContainerEditorsTests::SetUp();
            editor = ArrayEditor::Create();
            editor->setSizeProvider([this] { return size; });
            editor->setElementFactory(
                [this](HorizontalLayout& row, std::size_t index)
                {
                    ++factoryCalls;
                    row.addChildComponent<Probe>(index);
                });
            subscriptions << editor->onAdd->subscribeAndGetID([this] { ++adds; });
            subscriptions << editor->onRemove->subscribeAndGetID([this](std::size_t i)
                                                                 { removed.push_back(i); });
            subscriptions << editor->onMove->subscribeAndGetID(
                [this](std::size_t from, std::size_t to) { moved.emplace_back(from, to); });
        }

        void TearDown() override
        {
            subscriptions.clearAndReleaseAll();
            editor = nullptr;
            ContainerEditorsTests::TearDown();
        }

        ArrayEditor::Ptr editor;
        std::size_t size = 3;
        int factoryCalls = 0;
        int adds = 0;
        std::vector<std::size_t> removed;
        std::vector<std::pair<std::size_t, std::size_t>> moved;
        Core::DelegateSubscriberPoolGuard subscriptions;
    };

    TEST_F(ArrayEditorTests, BuildsOneRowPerElementInIndexOrder)
    {
        editor->unhandledDraw();

        ASSERT_EQ(editor->getElementRows().size(), 3u);
        EXPECT_THAT(Tags(editor->getElementRows()), ElementsAre(0u, 1u, 2u));
        EXPECT_EQ(editor->getSummaryLabel()->getText(), "3 elements");
        for (const auto& row : editor->getElementRows())
        {
            EXPECT_EQ(row.row->findFirstChildOf<Probe>()->draws, 1);
        }
    }

    TEST_F(ArrayEditorTests, SummaryUsesTheSingularForOneElement)
    {
        size = 1;
        editor->unhandledDraw();

        EXPECT_EQ(editor->getSummaryLabel()->getText(), "1 element");
    }

    TEST_F(ArrayEditorTests, RebuildsOnlyWhenTheSizeChanges)
    {
        editor->unhandledDraw();
        nextFrame();
        editor->unhandledDraw();
        EXPECT_EQ(factoryCalls, 3);

        size = 2;
        nextFrame();
        editor->unhandledDraw();

        EXPECT_EQ(factoryCalls, 5);
        EXPECT_THAT(Tags(editor->getElementRows()), ElementsAre(0u, 1u));
    }

    TEST_F(ArrayEditorTests, RowIdsSurviveARebuild)
    {
        size = 2;
        editor->unhandledDraw();
        std::vector<ImGuiID> before;
        for (const auto& row : editor->getElementRows())
        {
            before.push_back(row.row->findFirstChildOf<Probe>()->lastId);
        }

        size = 3;
        nextFrame();
        editor->unhandledDraw();

        const auto& rows = editor->getElementRows();
        EXPECT_EQ(rows[0].row->findFirstChildOf<Probe>()->lastId, before[0]);
        EXPECT_EQ(rows[1].row->findFirstChildOf<Probe>()->lastId, before[1]);
        EXPECT_NE(before[0], before[1]);
    }

    TEST_F(ArrayEditorTests, EditsAreReportedAfterTheDrawPass)
    {
        editor->unhandledDraw();
        const auto& rows = editor->getElementRows();

        rows[1].remove->onClick->trigger();
        EXPECT_TRUE(removed.empty());
        nextFrame();
        editor->unhandledDraw();
        EXPECT_THAT(removed, ElementsAre(1u));

        editor->getElementRows()[1].moveUp->onClick->trigger();
        editor->unhandledDraw();
        editor->getElementRows()[0].moveDown->onClick->trigger();
        editor->unhandledDraw();
        EXPECT_THAT(moved, ElementsAre(std::pair<std::size_t, std::size_t>(1, 0),
                                       std::pair<std::size_t, std::size_t>(0, 1)));

        editor->getAddButton()->onClick->trigger();
        editor->unhandledDraw();
        EXPECT_EQ(adds, 1);
    }

    TEST_F(ArrayEditorTests, OnlyOneEditIsReportedPerDraw)
    {
        editor->unhandledDraw();
        editor->getElementRows()[0].remove->onClick->trigger();
        editor->getElementRows()[2].remove->onClick->trigger();

        editor->unhandledDraw();

        EXPECT_THAT(removed, ElementsAre(0u)) << "later indices are stale after the first edit";
    }

    TEST_F(ArrayEditorTests, MoveButtonsAtTheEdgesAreDisabled)
    {
        editor->unhandledDraw();
        const auto& rows = editor->getElementRows();

        EXPECT_TRUE(rows.front().moveUp->isDisabledWidget());
        EXPECT_FALSE(rows.front().moveDown->isDisabledWidget());
        EXPECT_FALSE(rows.back().moveUp->isDisabledWidget());
        EXPECT_TRUE(rows.back().moveDown->isDisabledWidget());
    }

    TEST_F(ArrayEditorTests, MovesPastEitherEndAreIgnored)
    {
        editor->unhandledDraw();

        editor->getElementRows().front().moveUp->onClick->trigger();
        editor->getElementRows().back().moveDown->onClick->trigger();
        editor->unhandledDraw();

        EXPECT_TRUE(moved.empty());
    }

    TEST_F(ArrayEditorTests, FixedSizeHidesAddAndRemove)
    {
        editor->setFixedSize(true);
        editor->unhandledDraw();

        EXPECT_FALSE(editor->getAddButton()->isEnabled());
        for (const auto& row : editor->getElementRows())
        {
            EXPECT_FALSE(row.remove->isEnabled());
            EXPECT_TRUE(row.moveUp->isEnabled());
        }
    }

    TEST_F(ArrayEditorTests, LargeArraysStartCollapsedAndBuildNoRows)
    {
        size = InspectorStyle::Default().arrayAutoExpandLimit + 1;
        editor->unhandledDraw();

        EXPECT_FALSE(editor->isExpanded());
        EXPECT_TRUE(editor->getElementRows().empty());
        EXPECT_EQ(factoryCalls, 0);
        EXPECT_EQ(editor->getSummaryLabel()->getText(),
                  std::format("{} elements", InspectorStyle::Default().arrayAutoExpandLimit + 1));

        editor->getExpandButton()->onClick->trigger();
        editor->unhandledDraw();

        EXPECT_TRUE(editor->isExpanded());
        EXPECT_EQ(editor->getElementRows().size(), size);
    }

    TEST_F(ArrayEditorTests, CollapsingRemovesTheRowsAndTheChoiceIsKept)
    {
        editor->unhandledDraw();
        ASSERT_TRUE(editor->isExpanded());

        editor->getExpandButton()->onClick->trigger();
        editor->unhandledDraw();
        EXPECT_TRUE(editor->getElementRows().empty());

        size = 2;
        nextFrame();
        editor->unhandledDraw();
        EXPECT_FALSE(editor->isExpanded()) << "the user's choice wins over the size rule";
        EXPECT_EQ(editor->getSummaryLabel()->getText(), "2 elements");
    }

    TEST_F(ArrayEditorTests, EmptyArrayHasNoRows)
    {
        size = 0;
        editor->unhandledDraw();

        EXPECT_TRUE(editor->getElementRows().empty());
        EXPECT_EQ(editor->getSummaryLabel()->getText(), "0 elements");
    }

    // ===================== MapEditor =====================

    class MapEditorTests : public ContainerEditorsTests
    {
    protected:
        void SetUp() override
        {
            ContainerEditorsTests::SetUp();
            editor = MapEditor::Create();
            editor->setKeysProvider([this] { return keys; });
            editor->setElementFactory(
                [this](HorizontalLayout& row, const Core::StringAtom& key)
                {
                    built.push_back(key.toStdString());
                    row.addChildComponent<Probe>();
                });
            subscriptions << editor->onAdd->subscribeAndGetID(
                [this](const Core::StringAtom& key) { added.push_back(key.toStdString()); });
            subscriptions << editor->onRemove->subscribeAndGetID(
                [this](const Core::StringAtom& key) { removed.push_back(key.toStdString()); });
        }

        void TearDown() override
        {
            subscriptions.clearAndReleaseAll();
            editor = nullptr;
            ContainerEditorsTests::TearDown();
        }

        std::vector<std::string> rowKeys() const
        {
            std::vector<std::string> out;
            for (const auto& row : editor->getEntryRows())
            {
                out.push_back(row.key.toStdString());
            }
            return out;
        }

        void clickAdd(const std::string& key)
        {
            editor->getNewKeyInput()->setInputtedData(key);
            editor->getAddButton()->onClick->trigger();
            editor->unhandledDraw();
        }

        MapEditor::Ptr editor;
        std::vector<Core::StringAtom> keys = { "b"_atom, "a"_atom, "c"_atom };
        std::vector<std::string> built;
        std::vector<std::string> added;
        std::vector<std::string> removed;
        Core::DelegateSubscriberPoolGuard subscriptions;
    };

    TEST_F(MapEditorTests, RowsAreSortedByKey)
    {
        editor->unhandledDraw();

        EXPECT_THAT(rowKeys(), ElementsAre("a", "b", "c"));
        EXPECT_THAT(built, ElementsAre("a", "b", "c"));
        EXPECT_EQ(editor->getEntryRows()[1].row->getStableId(), std::optional("b"_atom));
    }

    TEST_F(MapEditorTests, ReorderedKeysDoNotRebuild)
    {
        editor->unhandledDraw();
        keys = { "c"_atom, "b"_atom, "a"_atom };
        nextFrame();
        editor->unhandledDraw();

        EXPECT_EQ(built.size(), 3u);
    }

    TEST_F(MapEditorTests, AChangedKeySetRebuilds)
    {
        editor->unhandledDraw();
        keys = { "a"_atom, "z"_atom };
        nextFrame();
        editor->unhandledDraw();

        EXPECT_THAT(rowKeys(), ElementsAre("a", "z"));
        EXPECT_EQ(built.size(), 5u);
    }

    TEST_F(MapEditorTests, AddUsesTheTypedKeyAndRejectsDuplicatesAndEmptyKeys)
    {
        editor->unhandledDraw();

        clickAdd("a");
        clickAdd("");
        EXPECT_TRUE(added.empty());

        clickAdd("d");
        EXPECT_THAT(added, ElementsAre("d"));
        EXPECT_TRUE(editor->getNewKeyInput()->getInputtedData().empty());
    }

    TEST_F(MapEditorTests, RemoveReportsTheKeyAfterTheDrawPass)
    {
        editor->unhandledDraw();

        editor->getEntryRows()[1].remove->onClick->trigger();
        EXPECT_TRUE(removed.empty());
        editor->unhandledDraw();

        EXPECT_THAT(removed, ElementsAre("b"));
    }
} // namespace
