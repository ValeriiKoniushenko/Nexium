// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/CollapsingSection.h"
#include "Editor/GuiComponents/ContainerEditors.h"
#include "Editor/GuiComponents/EnumInputs.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/PropertyGrid.h"
#include "ImGuiFrameFixture.h"
#include "PropertyGridFixtures.h"

#include "gmock/gmock.h"

#include <string>
#include <vector>

using namespace NX::Gui;
using namespace GridFixtures;
using testing::ElementsAre;

ECS_IMPL(GridFixtures::GridComponent);

namespace
{
    constexpr const char* SettingsDump = R"(Row "Strength" #strength Number
Row "Light color" #color Color3
Row "Enabled" #enabled Bool
Row "Quality" #quality Enum
Row "Limit" #limit Optional
  Row Number
Row "Values" #values Array
  Element [0]
    Row Number
  Element [1]
    Row Number
Section "Inner" #inner
  Row "A" #a Number
  Row "B" #b Number
Row "Locked" #locked Number readonly
Row "Level" #level Number
Row "Count" #count Number
Row "Direction" #direction Vec3
Row "Weights" #weights Map
)";

    class PropertyGridTests : public EditorTestUtils::ImGuiFrameTest
    {
    protected:
        void SetUp() override
        {
            ImGuiFrameTest::SetUp();
            grid = makeGrid(settings);
        }

        void TearDown() override
        {
            grid = nullptr;
            ImGuiFrameTest::TearDown();
        }

        template<class T>
        static PropertyGrid::Ptr makeGrid(T& target, PropertySections sections
                                                     = PropertySections::Flat)
        {
            auto out = PropertyGrid::Create();
            out->setSections(sections);
            out->setTarget([&target] { return NX::Reflection::ObjectRef(target); });
            return out;
        }

        template<class W>
        W* widgetOf(const char* property) const
        {
            auto* row = grid->findRow(Core::StringAtom::Intern(property));
            EXPECT_NE(row, nullptr) << property;
            return row ? row->findFirstChildOf<W>() : nullptr;
        }

        void draw() { grid->unhandledDraw(); }

        static std::vector<std::string> Names(const std::vector<Core::StringAtom>& atoms)
        {
            std::vector<std::string> out;
            for (const auto& atom : atoms)
            {
                out.push_back(atom.toStdString());
            }
            return out;
        }

        Settings settings;
        PropertyGrid::Ptr grid;
    };

    // ===================== Determinism =====================

    TEST_F(PropertyGridTests, FlatDumpOfAStruct)
    {
        draw();

        EXPECT_EQ(grid->dump(), SettingsDump);
    }

    TEST_F(PropertyGridTests, PerClassModeWrapsTheRowsInOneSectionPerClass)
    {
        grid->setSections(PropertySections::PerClass);
        draw();

        const std::string dump = grid->dump();
        EXPECT_TRUE(dump.starts_with("Section \"Settings\" #Settings\n  Row \"Strength\""))
            << dump;
    }

    TEST_F(PropertyGridTests, BuildingTwiceGivesTheSameTree)
    {
        Settings other;
        auto second = makeGrid(other);

        draw();
        second->unhandledDraw();

        EXPECT_EQ(second->dump(), grid->dump());
    }

    TEST_F(PropertyGridTests, RebuildingGivesTheSameTree)
    {
        draw();
        const std::string before = grid->dump();
        const std::size_t rows = grid->getChildrenCount();

        grid->rebuild();
        nextFrame();
        draw();

        EXPECT_EQ(grid->dump(), before);
        EXPECT_EQ(grid->getChildrenCount(), rows);
    }

    TEST_F(PropertyGridTests, LabelsComeFromDisplayNameOrTheFieldName)
    {
        EXPECT_EQ(LabelFromFieldName("_isEnabled"), "Is enabled");
        EXPECT_EQ(LabelFromFieldName("minLightStrength"), "Min light strength");
        EXPECT_EQ(LabelFromFieldName("HTTPServer"), "HTTP server");

        draw();
        EXPECT_EQ(grid->findRow("color"_atom)->getLabel()->getText(), "Light color");
    }

    // ===================== Pull =====================

    TEST_F(PropertyGridTests, RowsShowTheCurrentValue)
    {
        settings.strength = 7.f;
        draw();
        EXPECT_FLOAT_EQ(widgetOf<NumInput<float>>("strength")->getInputtedData(), 7.f);

        settings.strength = 3.f;
        nextFrame();
        draw();
        EXPECT_FLOAT_EQ(widgetOf<NumInput<float>>("strength")->getInputtedData(), 3.f);
        EXPECT_EQ(widgetOf<EnumCombo>("quality")->getPreviewText(), "Medium");
    }

    TEST_F(PropertyGridTests, DrawingNeverEdits)
    {
        int edits = 0;
        auto subscription
            = grid->onEdited->subscribeAndGetID([&](const NX::Reflection::PropertyDesc&) { ++edits; });

        for (int i = 0; i < 3; ++i)
        {
            draw();
            nextFrame();
        }

        EXPECT_EQ(edits, 0);
        EXPECT_TRUE(settings.edited.empty());
        grid->onEdited->unsubscribe(subscription);
    }

    // ===================== Edits =====================

    TEST_F(PropertyGridTests, EditsGoThroughTheSetterAndNotify)
    {
        std::vector<Core::StringAtom> reported;
        auto subscription = grid->onEdited->subscribeAndGetID(
            [&](const NX::Reflection::PropertyDesc& property) { reported.push_back(property.name); });
        draw();

        widgetOf<NumInput<int>>("level")->onInput->trigger(4);

        EXPECT_EQ(settings.level, 40);
        EXPECT_THAT(Names(settings.edited), ElementsAre("level"));
        EXPECT_THAT(Names(reported), ElementsAre("level"));
        grid->onEdited->unsubscribe(subscription);
    }

    TEST_F(PropertyGridTests, FieldsWithoutASetterAreWrittenDirectly)
    {
        draw();

        widgetOf<NumInput<float>>("strength")->onInput->trigger(3.5f);
        widgetOf<CheckBox>("enabled")->onChange->trigger(false);

        EXPECT_FLOAT_EQ(settings.strength, 3.5f);
        EXPECT_FALSE(settings.enabled);
        EXPECT_THAT(Names(settings.edited), ElementsAre("strength", "enabled"));
    }

    TEST_F(PropertyGridTests, LimitsComeFromTheFieldAttributes)
    {
        draw();

        const auto* strength = widgetOf<NumInput<float>>("strength");
        EXPECT_FLOAT_EQ(strength->getMin(), 0.f);
        EXPECT_FLOAT_EQ(strength->getMax(), 10.f);
        EXPECT_FLOAT_EQ(strength->getStep(), 0.5f);
        EXPECT_EQ(widgetOf<NumInput<std::size_t>>("count")->getMin(), 2u);
    }

    TEST_F(PropertyGridTests, ReadOnlyRowsAreDisabledAndHiddenFieldsAreSkipped)
    {
        draw();

        EXPECT_TRUE(grid->findRow("locked"_atom)->isDisabledWidget());
        EXPECT_FALSE(grid->findRow("level"_atom)->isDisabledWidget());
        EXPECT_EQ(grid->findRow("secret"_atom), nullptr);
    }

    TEST_F(PropertyGridTests, AReadOnlyGridDisablesEveryRow)
    {
        grid->setReadOnly(true);
        draw();

        EXPECT_TRUE(grid->findRow("strength"_atom)->isDisabledWidget());
        EXPECT_TRUE(grid->findRow("a"_atom)->isDisabledWidget());
    }

    TEST_F(PropertyGridTests, EnumSelectionWritesTheEnum)
    {
        draw();

        widgetOf<EnumCombo>("quality")->onSelect->trigger(2);

        EXPECT_EQ(settings.quality, Quality::High);
        EXPECT_THAT(Names(settings.edited), ElementsAre("quality"));
    }

    TEST_F(PropertyGridTests, OptionalToggleSetsAndResetsTheValue)
    {
        draw();
        auto* optional = widgetOf<OptionalEditor>("limit");
        EXPECT_FALSE(optional->isEngaged());

        optional->getToggle()->onChange->trigger(true);
        ASSERT_TRUE(settings.limit.has_value());
        nextFrame();
        draw();
        EXPECT_TRUE(optional->isEngaged());

        optional->getValueWidget()->findFirstChildOf<NumInput<float>>()->onInput->trigger(1.5f);
        EXPECT_EQ(settings.limit, std::optional(1.5f));

        optional->getToggle()->onChange->trigger(false);
        EXPECT_FALSE(settings.limit.has_value());
        EXPECT_THAT(Names(settings.edited), ElementsAre("limit", "limit", "limit"));
    }

    TEST_F(PropertyGridTests, ArrayButtonsEditTheVector)
    {
        draw();
        auto* array = widgetOf<ArrayEditor>("values");

        array->getAddButton()->onClick->trigger();
        draw();
        EXPECT_THAT(settings.values, ElementsAre(1, 2, 0));

        nextFrame();
        draw();
        array->getElementRows()[0].remove->onClick->trigger();
        draw();
        EXPECT_THAT(settings.values, ElementsAre(2, 0));
        EXPECT_THAT(Names(settings.edited), ElementsAre("values", "values"));
    }

    TEST_F(PropertyGridTests, ElementEditsWriteTheWholeContainer)
    {
        draw();
        auto* array = widgetOf<ArrayEditor>("values");

        array->getElementRows()[1].row->findFirstChildOf<NumInput<int>>()->onInput->trigger(9);

        EXPECT_THAT(settings.values, ElementsAre(1, 9));
        EXPECT_THAT(Names(settings.edited), ElementsAre("values"));
    }

    TEST_F(PropertyGridTests, NestedStructEditsWriteTheOwningField)
    {
        draw();

        widgetOf<NumInput<int>>("a")->onInput->trigger(5);

        EXPECT_EQ(settings.inner.a, 5);
        EXPECT_THAT(Names(settings.edited), ElementsAre("inner"));
    }

    TEST_F(PropertyGridTests, MapEntriesCanBeAddedAndEdited)
    {
        draw();
        auto* map = widgetOf<MapEditor>("weights");

        map->getNewKeyInput()->setInputtedData("w");
        map->getAddButton()->onClick->trigger();
        draw();
        ASSERT_TRUE(settings.weights.contains("w"_atom));

        nextFrame();
        draw();
        ASSERT_EQ(map->getEntryRows().size(), 1u);
        map->getEntryRows()[0].row->findFirstChildOf<NumInput<float>>()->onInput->trigger(0.5f);

        EXPECT_FLOAT_EQ(settings.weights.at("w"_atom), 0.5f);
    }

    // ===================== Targets =====================

    TEST_F(PropertyGridTests, ATargetOfTheSameTypeIsRebound)
    {
        Settings first;
        Settings second;
        second.strength = 8.f;
        Settings* current = &first;
        grid->setTarget([&current] { return NX::Reflection::ObjectRef(*current); });
        draw();
        const auto* input = widgetOf<NumInput<float>>("strength");

        current = &second;
        nextFrame();
        draw();

        EXPECT_EQ(widgetOf<NumInput<float>>("strength"), input) << "rebound, not rebuilt";
        EXPECT_FLOAT_EQ(input->getInputtedData(), 8.f);
    }

    TEST_F(PropertyGridTests, ATargetOfAnotherTypeRebuilds)
    {
        OtherSettings other;
        draw();

        grid->setTarget([&other] { return NX::Reflection::ObjectRef(other); });
        nextFrame();
        draw();

        EXPECT_EQ(grid->dump(), "Row \"Value\" #value Number\n");
    }

    TEST_F(PropertyGridTests, ADestroyedComponentEmptiesTheGrid)
    {
        auto component = GridComponent::Create();
        grid->setTarget(*component);
        draw();
        ASSERT_NE(grid->findRow("speed"_atom), nullptr);

        component = nullptr;
        nextFrame();
        draw();

        EXPECT_EQ(grid->getChildrenCount(), 0u);
        EXPECT_EQ(grid->dump(), "");
    }

    TEST_F(PropertyGridTests, ComponentsGetOneSectionPerClassOfTheirChain)
    {
        auto component = GridComponent::Create();
        grid->setSections(PropertySections::PerClass);
        grid->setTarget(*component);
        draw();

        std::vector<std::string> titles;
        for (const auto& child : grid->getChildren())
        {
            titles.push_back(child->castTo<CollapsingSection>()->getTitle().toStdString());
        }
        // `_noTick` and `_children` are annotated with Category = "Component data".
        EXPECT_THAT(titles, ElementsAre("Abstract component", "Base component", "Grid component",
                                        "Component data"));

        widgetOf<NumInput<float>>("speed")->onInput->trigger(4.f);
        EXPECT_FLOAT_EQ(component->speed, 4.f);
        EXPECT_THAT(Names(component->edited), ElementsAre("speed"));
    }

    // ===================== Registry =====================

    TEST_F(PropertyGridTests, TypesWithoutAnEditorGetAReadOnlyFallbackRow)
    {
        const PropertyEditors empty;
        grid->setEditors(empty);
        draw();

        const auto* row = grid->findRow("strength"_atom);
        ASSERT_NE(row, nullptr);
        EXPECT_EQ(row->getEditorName(), "Fallback");
        EXPECT_TRUE(row->isDisabledWidget());
        const auto* text = row->findFirstChildOf<TextInput>();
        ASSERT_NE(text, nullptr);
        EXPECT_EQ(text->getInputtedData(), "float (no editor)");
    }

    TEST_F(PropertyGridTests, ARegisteredEditorReplacesTheBuiltinOne)
    {
        PropertyEditors editors = PropertyEditors::Builtin();
        editors.add<float>("Custom"_atom, [](PropertyEditorContext& context)
                           { context.row.addChildComponent<Label>("custom"_atom); });
        grid->setEditors(editors);
        draw();

        EXPECT_EQ(grid->findRow("strength"_atom)->getEditorName(), "Custom");
        EXPECT_EQ(grid->findRow("count"_atom)->getEditorName(), "Number");
    }
} // namespace
