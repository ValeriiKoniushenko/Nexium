// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/ContainerEditors.h"
#include "Editor/GuiComponents/EnumInputs.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/InspectorStyle.h"
#include "Editor/GuiComponents/PropertyCustomization.h"
#include "Editor/GuiComponents/PropertyGrid.h"
#include "Editor/GuiComponents/VecInput.h"
#include "ImGuiFrameFixture.h"
#include "PropertyGridFixtures.h"

#include "gmock/gmock.h"

#include <string>
#include <tuple>
#include <vector>

using namespace NX::Gui;
using namespace GridFixtures;
using testing::ElementsAre;

namespace
{
    constexpr const char* DefaultDump = R"(Section "Custom base" #Custom base
  Row "Base value" #baseValue Number
Section "Customized" #Customized
  Row "Alpha" #alpha Number
  Row "Beta" #beta Number
  Row "Enabled" #enabled Bool
  Row "Gated" #gated Number
  Row "Atlas" #atlas AssetRef
  Row "Frame" #frame AssetRef
Section "Extra" #Extra
  Row "Extra" #extra Number
)";

    class PropertyCustomizationTests : public EditorTestUtils::ImGuiFrameTest
    {
    protected:
        void SetUp() override
        {
            ImGuiFrameTest::SetUp();
            grid = PropertyGrid::Create();
            grid->setSections(PropertySections::PerClass);
            grid->setCustomizations(customizations);
            grid->setChoices(choices);
            grid->setTarget([this] { return NX::Reflection::ObjectRef(object); });
            subscriptions << grid->onEdited->subscribeAndGetID(
                [this](const NX::Reflection::PropertyDesc& property)
                { reported.push_back(property.name.toStdString()); });
        }

        void TearDown() override
        {
            subscriptions.clearAndReleaseAll();
            grid = nullptr;
            ImGuiFrameTest::TearDown();
        }

        void draw() { grid->unhandledDraw(); }

        void redraw()
        {
            nextFrame();
            draw();
        }

        template<class W>
        W* widgetOf(const char* row) const
        {
            auto* found = grid->findRow(Core::StringAtom::Intern(row));
            EXPECT_NE(found, nullptr) << row;
            return found ? found->findFirstChildOf<W>() : nullptr;
        }

        [[nodiscard]] PropertyRow* rowOf(const char* row) const
        {
            return grid->findRow(Core::StringAtom::Intern(row));
        }

        static std::vector<std::string> Names(const std::vector<Core::StringAtom>& atoms)
        {
            std::vector<std::string> out;
            for (const auto& atom : atoms)
            {
                out.push_back(atom.toStdString());
            }
            return out;
        }

        Customized object;
        PropertyCustomizations customizations;
        PropertyChoices choices;
        PropertyGrid::Ptr grid;
        std::vector<std::string> reported;
        Core::DelegateSubscriberPoolGuard subscriptions;
    };

    // ===================== Sections =====================

    TEST_F(PropertyCustomizationTests, ByDefaultEveryClassHasASectionAndCategoriesComeLast)
    {
        draw();

        EXPECT_EQ(grid->dump(), DefaultDump);
    }

    TEST_F(PropertyCustomizationTests, SectionsWithTheSameTitleAreMerged)
    {
        customizations.For<CustomBase>().section("Main");
        customizations.For<Customized>().section("Main");
        draw();

        EXPECT_TRUE(grid->dump().starts_with(R"(Section "Main" #Main
  Row "Base value" #baseValue Number
  Row "Alpha" #alpha Number
)")) << grid->dump();
        EXPECT_EQ(grid->getChildrenCount(), 2u) << "Main and Extra";
    }

    TEST_F(PropertyCustomizationTests, ACategoryOverrideMovesTheRowAfterTheClassSections)
    {
        customizations.For<Customized>().category("beta", "Extra");
        draw();

        EXPECT_TRUE(grid->dump().ends_with(R"(Section "Extra" #Extra
  Row "Extra" #extra Number
  Row "Beta" #beta Number
)")) << grid->dump();
    }

    TEST_F(PropertyCustomizationTests, ACategoryNamedLikeAClassSectionJoinsIt)
    {
        customizations.For<CustomBase>().category("extra", "Customized");
        draw();

        EXPECT_TRUE(grid->dump().ends_with(R"(  Row "Frame" #frame AssetRef
  Row "Extra" #extra Number
)")) << grid->dump();
    }

    // ===================== Field rules =====================

    TEST_F(PropertyCustomizationTests, FieldsCanBeHiddenMadeReadOnlyAndRelabelled)
    {
        customizations.For<Customized>().hide("alpha").readOnly("beta").label("beta", "Second");
        draw();

        EXPECT_EQ(rowOf("alpha"), nullptr);
        EXPECT_EQ(rowOf("beta")->getLabel()->getText(), "Second");
        EXPECT_TRUE(rowOf("beta")->isReadOnly());
    }

    TEST_F(PropertyCustomizationTests, OrderPutsTheListedRowsFirst)
    {
        customizations.For<Customized>().order({ "frame", "atlas" });
        draw();

        EXPECT_TRUE(grid->dump().find(R"(Section "Customized" #Customized
  Row "Frame" #frame AssetRef
  Row "Atlas" #atlas AssetRef
  Row "Alpha" #alpha Number
)") != std::string::npos)
            << grid->dump();
    }

    TEST_F(PropertyCustomizationTests, EnabledIfMetadataFollowsABoolField)
    {
        draw();
        EXPECT_TRUE(rowOf("gated")->isDisabledWidget());
        EXPECT_FALSE(rowOf("gated")->isReadOnly());

        object.enabled = true;
        redraw();

        EXPECT_FALSE(rowOf("gated")->isDisabledWidget());
    }

    TEST_F(PropertyCustomizationTests, EnabledIfCustomizationTakesAPredicate)
    {
        customizations.For<Customized>().enabledIf("beta",
                                                   [](const Customized& c) { return c.alpha > 1; });
        draw();
        EXPECT_TRUE(rowOf("beta")->isDisabledWidget());

        object.alpha = 2;
        redraw();
        EXPECT_FALSE(rowOf("beta")->isDisabledWidget());
    }

    TEST_F(PropertyCustomizationTests, AfterEditRunsOnceTheFieldIsWritten)
    {
        customizations.For<Customized>().afterEdit("alpha",
                                                   [](Customized& c) { c.beta = c.alpha + 100; });
        draw();

        widgetOf<NumInput<int>>("alpha")->onInput->trigger(3);

        EXPECT_EQ(object.alpha, 3);
        EXPECT_EQ(object.beta, 103);
        EXPECT_THAT(reported, ElementsAre("alpha"));
    }

    // ===================== Virtual rows =====================

    TEST_F(PropertyCustomizationTests, ComputedRowsShowAValueThatIsNotAField)
    {
        customizations.For<Customized>().computed<int>("doubled", "Doubled",
                                                       [](Customized& c) { return c.alpha * 2; });
        draw();

        EXPECT_TRUE(grid->dump().find("  Row \"Doubled\" #doubled Number readonly\nSection") !=
                    std::string::npos)
            << grid->dump();
        EXPECT_EQ(widgetOf<NumInput<int>>("doubled")->getInputtedData(), 2);

        object.alpha = 5;
        redraw();
        EXPECT_EQ(widgetOf<NumInput<int>>("doubled")->getInputtedData(), 10);
    }

    TEST_F(PropertyCustomizationTests, PropertyRowsWriteThroughTheirSetter)
    {
        customizations.For<Customized>().property<int>(
            "counter", "Counter", [](Customized& c) { return c.counter; },
            [](Customized& c, const int& value) { c.counter = value * 2; },
            { .min = 0.0, .max = 50.0, .step = 5.0 });
        draw();
        auto* input = widgetOf<NumInput<int>>("counter");

        input->onInput->trigger(7);

        EXPECT_EQ(object.counter, 14);
        EXPECT_THAT(reported, ElementsAre("counter"));
        EXPECT_EQ(input->getMax(), 50);
        EXPECT_FALSE(rowOf("counter")->isReadOnly());
    }

    TEST_F(PropertyCustomizationTests, PropertyRowsCanGoToACategory)
    {
        customizations.For<Customized>().property<int>(
            "counter", "Counter", [](Customized& c) { return c.counter; },
            [](Customized& c, const int& value) { c.counter = value; }, { .category = "Extra" });
        draw();

        EXPECT_TRUE(grid->dump().ends_with("  Row \"Counter\" #counter Number\n")) << grid->dump();
    }

    TEST_F(PropertyCustomizationTests, CustomRowsBuildTheirOwnWidgets)
    {
        customizations.For<Customized>().customRow(
            "tools", "Tools",
            [](CustomRowContext& context)
            {
                auto* button = context.row.addChildComponent<Button>("Bump"_atom);
                std::ignore = button->onClick->subscribeAndGetID(
                    [owner = context.owner, edited = context.edited]
                    {
                        static_cast<Customized*>(owner())->alpha = 9;
                        edited();
                    });
            });
        draw();
        EXPECT_EQ(rowOf("tools")->getEditorName(), "Custom");

        widgetOf<Button>("tools")->onClick->trigger();

        EXPECT_EQ(object.alpha, 9);
        EXPECT_THAT(reported, ElementsAre("tools"));
    }

    // ===================== Asset references =====================

    TEST_F(PropertyCustomizationTests, AssetReferencesOfferTheRegisteredChoices)
    {
        choices.add("TestAtlas"_atom, [](const ChoiceRequest&)
                    { return std::vector<Core::StringAtom>{ "a"_atom, "b"_atom }; });
        draw();
        auto* combo = widgetOf<ChoiceCombo>("atlas");

        EXPECT_EQ(combo->getValue(), "a");
        EXPECT_THAT(Names(combo->refreshOptions()), ElementsAre("a", "b"));

        combo->onSelect->trigger("b"_atom);
        EXPECT_EQ(object.atlas, "b");
        EXPECT_THAT(Names(object.edited), ElementsAre("atlas"));
    }

    TEST_F(PropertyCustomizationTests, DependentChoicesSeeTheDependencyAndTheOwner)
    {
        const void* seenOwner = nullptr;
        choices.add("TestFrame"_atom,
                    [&seenOwner](const ChoiceRequest& request)
                    {
                        seenOwner = request.owner;
                        const std::string base = request.dependency.toStdString();
                        return std::vector<Core::StringAtom>{ Core::StringAtom(base + "1"),
                                                              Core::StringAtom(base + "2") };
                    });
        object.atlas = "b"_atom;
        draw();

        EXPECT_THAT(Names(widgetOf<ChoiceCombo>("frame")->refreshOptions()),
                    ElementsAre("b1", "b2"));
        EXPECT_EQ(seenOwner, &object);
    }

    TEST_F(PropertyCustomizationTests, AnUnknownChoiceKindOffersNothing)
    {
        draw();

        EXPECT_TRUE(widgetOf<ChoiceCombo>("atlas")->refreshOptions().empty());
    }

    TEST_F(PropertyCustomizationTests, VirtualPropertiesCanUseChoices)
    {
        choices.add("Counters"_atom, [](const ChoiceRequest&)
                    { return std::vector<Core::StringAtom>{ "one"_atom, "two"_atom }; });
        Core::StringAtom picked = "one"_atom;
        customizations.For<Customized>().property<Core::StringAtom>(
            "pick", "Pick", [&picked](Customized&) { return picked; },
            [&picked](Customized&, const Core::StringAtom& value) { picked = value; },
            { .meta = { { "Editor", "AssetRef" }, { "AssetType", "Counters" } } });
        draw();
        auto* combo = widgetOf<ChoiceCombo>("pick");

        EXPECT_THAT(Names(combo->refreshOptions()), ElementsAre("one", "two"));
        combo->onSelect->trigger("two"_atom);
        EXPECT_EQ(picked, "two");
    }

    // ===================== Wrapper types =====================

    class WrapperEditorsTests : public EditorTestUtils::ImGuiFrameTest
    {
    protected:
        void SetUp() override
        {
            ImGuiFrameTest::SetUp();
            grid = PropertyGrid::Create();
            grid->setSections(PropertySections::Flat);
            grid->setTarget([this] { return NX::Reflection::ObjectRef(wrappers); });
            grid->unhandledDraw();
        }

        void TearDown() override
        {
            grid = nullptr;
            ImGuiFrameTest::TearDown();
        }

        Wrappers wrappers;
        PropertyGrid::Ptr grid;
    };

    TEST_F(WrapperEditorsTests, PositionsSizesAndComponentLinksHaveEditors)
    {
        EXPECT_EQ(grid->dump(), R"(Row "Position" #position Vec3
Row "Size" #size Size2
Row "Link" #link Component readonly
Row "Links" #links Array readonly
)");
    }

    TEST_F(WrapperEditorsTests, ContainersOfValuesThatCannotBeEditedAreReadOnly)
    {
        // Adding an element would insert a null link: nothing can edit it afterwards.
        const auto* links = grid->findRow("links"_atom)->findFirstChildOf<ArrayEditor>();

        EXPECT_TRUE(links->isFixedSize());
        EXPECT_FALSE(links->getAddButton()->isEnabled());
    }

    TEST_F(WrapperEditorsTests, PositionEditsWriteTheWrapperType)
    {
        auto* input = grid->findRow("position"_atom)->findFirstChildOf<Float3Input>();
        EXPECT_EQ(input->getInputtedData(), glm::vec3(1.f, 2.f, 3.f));

        input->onInput->trigger(glm::vec3(7.f));

        EXPECT_EQ(glm::vec3(wrappers.position), glm::vec3(7.f));
    }

    TEST_F(WrapperEditorsTests, SizesUseWidthAndHeightLabels)
    {
        auto* input = grid->findRow("size"_atom)->findFirstChildOf<Float2Input>();
        EXPECT_EQ(input->getInputtedData(), glm::vec2(4.f, 5.f));
        EXPECT_EQ(input->labels[0]->getText(), "W:");
        EXPECT_EQ(input->labels[1]->getText(), "H:");

        input->onInput->trigger(glm::vec2(8.f, 9.f));
        EXPECT_EQ(wrappers.size, Core::FSize2(8.f, 9.f));
    }

    TEST_F(WrapperEditorsTests, ComponentLinksShowNameAndType)
    {
        const auto* text = grid->findRow("link"_atom)->findFirstChildOf<TextInput>();
        EXPECT_EQ(text->getInputtedData(), InspectorStyle::Default().emptyValueText);

        wrappers.link = GridComponent::Create();
        wrappers.link->setComponentName("Lamp"_atom);
        nextFrame();
        grid->unhandledDraw();

        EXPECT_EQ(text->getInputtedData(), "Lamp [GridFixtures::GridComponent]");
    }
} // namespace
