// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Parity snapshots of the generated property panels that replaced the hand-written ones in
// ObjectPropertiesWindowEWC, the ECS asset editor adapters and WorldSettingsEWC. Each snapshot
// lists the sections and rows the panel shows for an engine class with the editor's
// customizations; the comments note where it deliberately differs from the old panel.

#include "Editor/EditorPropertyCustomizations.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/ContainerEditors.h"
#include "Editor/GuiComponents/EnumInputs.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/PropertyCustomization.h"
#include "Editor/GuiComponents/PropertyGrid.h"
#include "Editor/GuiComponents/VecInput.h"
#include "ImGuiFrameFixture.h"
#include "NxWorld/Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/World.h"
#include "NxWorld/Scene/SceneObjects/Rectangle/Rectangle.h"
#include "NxWorld/Scene/SceneObjects/Rectangle/RectangleAnimated.h"
#include "PropertyGridFixtures.h"

#include "gmock/gmock.h"

#include <string>
#include <vector>

using namespace NX::Gui;
using testing::Contains;

namespace
{
    // Old "General": Name (read-only), Type (read-only), Enabled. Now Enabled comes first (it is
    // declared in AbstractComponent) and Name is editable, as in the old asset editor.
    constexpr const char* GeneralAndTransform = R"(Section "General" #General
  Row "Enabled" #_isEnabled Bool
  Row "Name" #_name Text
  Row "Type" #_type Text readonly
Section "Transform" #Transform
  Row "Position" #_position Vec3
  Row "Rotation" #_rotation Vec3
  Row "Scale" #_scale Vec3
  Row "Origin" #_origin Vec3
)";

    // Old "Component data": Parent, Children count, Is initialized, Disabled ticks, Children.
    constexpr const char* ComponentData = R"(Section "Component data" #Component data
  Row "Disabled ticks" #_noTick Bool
  Row "Parent" #parent Text readonly
  Row "Children count" #childrenCount Number readonly
  Row "Children" #_children Array readonly
  Row "Is initialized" #isInitialized Bool readonly
)";

    // Old "Texture": Atlas, Frame, "Rendering: Blending". Post draw was in the old asset editor.
    constexpr const char* RenderingAndTexture = R"(Section "Rendering" #Rendering
  Row "Post draw" #_isPostDraw Bool
  Row "Blending" #blending Bool
Section "Texture" #Texture
  Row "Atlas" #_atlasName AssetRef
  Row "Frame" #_textureName AssetRef
)";

    class EnginePanelsTests : public EditorTestUtils::ImGuiFrameTest
    {
    protected:
        void SetUp() override
        {
            ImGuiFrameTest::SetUp();
            NX::RegisterEditorPropertyCustomizations(customizations, choices, editors);
        }

        template<class T>
        PropertyGrid::Ptr gridFor(T& target)
        {
            auto grid = PropertyGrid::Create();
            grid->setSections(PropertySections::PerClass);
            grid->setCustomizations(customizations);
            grid->setChoices(choices);
            grid->setEditors(editors);
            grid->setTarget([&target] { return NX::Reflection::ObjectRef(target); });
            grid->unhandledDraw();
            return grid;
        }

        template<class W>
        static W* widgetOf(const PropertyGrid& grid, const char* row)
        {
            auto* found = grid.findRow(Core::StringAtom::Intern(row));
            EXPECT_NE(found, nullptr) << row;
            return found ? found->findFirstChildOf<W>() : nullptr;
        }

        PropertyCustomizations customizations;
        PropertyChoices choices;
        PropertyEditors editors = PropertyEditors::Builtin();
    };

    // ===================== Parity snapshots =====================

    TEST_F(EnginePanelsTests, RectanglePanel)
    {
        NX::SceneObj::Rectangle rectangle;
        auto grid = gridFor(rectangle);

        EXPECT_EQ(grid->dump(), std::string(GeneralAndTransform) + RenderingAndTexture
                                    + ComponentData);
    }

    TEST_F(EnginePanelsTests, AnimatedRectanglePanel)
    {
        // Old "Rectangle" section: Animation, Animation FPS. "Animated" was planned there but
        // never created.
        NX::SceneObj::RectangleAnimated rectangle;
        auto grid = gridFor(rectangle);

        EXPECT_EQ(grid->dump(), std::string(GeneralAndTransform) + RenderingAndTexture
                                    + R"(Section "Animation" #Animation
  Row "Animated" #animationEnabled Bool
  Row "Animation" #animation AssetRef
  Row "Animation FPS" #animationFps Number
)" + ComponentData);
    }

    TEST_F(EnginePanelsTests, PerspectiveCameraPanel)
    {
        // Old "Camera": Far plane, Near plane, FOV, Aspect, Output size.
        NX::PerspectiveCamera camera;
        auto grid = gridFor(camera);

        EXPECT_EQ(grid->dump(), std::string(GeneralAndTransform) + R"(Section "Rendering" #Rendering
  Row "Post draw" #_isPostDraw Bool
Section "Camera" #Camera
  Row "Far plane" #_far Number
  Row "Near plane" #_near Number
  Row "FOV" #_fov Number
  Row "Aspect" #_aspect Size2
  Row "Output size" #outputSize Size2 readonly
)" + ComponentData);
    }

    TEST_F(EnginePanelsTests, OrthographicCameraPanelShowsZoom)
    {
        // The old panel had an empty branch for orthographic cameras.
        NX::OrthographicCamera camera;
        auto grid = gridFor(camera);

        EXPECT_NE(grid->dump().find(R"(Section "Camera" #Camera
  Row "Far plane" #_far Number
  Row "Near plane" #_near Number
  Row "Zoom" #_zoom Number
)"),
                  std::string::npos)
            << grid->dump();
    }

    TEST_F(EnginePanelsTests, WorldSettingsPanels)
    {
        // Old "Global": Camera. World name and Start scene are World's FIELD()s.
        NX::World world;
        auto global = gridFor(world);
        auto lightning = gridFor(world.lightning);

        EXPECT_EQ(global->dump(), R"(Section "Global" #Global
  Row "Camera" #camera Custom
  Row "World name" #worldName Text
  Row "Start scene" #startScene AssetRef
)");
        EXPECT_EQ(lightning->dump(), R"(Section "Lightning" #Lightning
  Row "Light color" #color Color3
  Row "Ambient strength" #ambientStrength Number
  Row "Min light strength" #minLightStrength Number
  Row "Specular strength" #specularStrength Number
  Row "Specular pow" #specularPow Number
  Row "Sun light direction" #sunDirection Vec3
)");
    }

    // ===================== Behaviour =====================

    TEST_F(EnginePanelsTests, TransformEditsGoThroughTheSetters)
    {
        NX::SceneObj::Rectangle rectangle;
        auto grid = gridFor(rectangle);
        rectangle.setDirtyMatrices(false);

        widgetOf<Float3Input>(*grid, "_position")->onInput->trigger(glm::vec3(5.f));

        EXPECT_EQ(glm::vec3(rectangle.getPosition()), glm::vec3(5.f));
        EXPECT_TRUE(rectangle.hasDirtyMatrices()) << "a raw write would leave the matrix stale";
    }

    TEST_F(EnginePanelsTests, CameraRowsUseTheSettersAndTheirLimits)
    {
        NX::OrthographicCamera camera;
        auto grid = gridFor(camera);
        auto* zoom = widgetOf<FloatInput>(*grid, "_zoom");

        EXPECT_FLOAT_EQ(zoom->getMin(), NX::OrthographicCamera::minZoom);
        EXPECT_FLOAT_EQ(zoom->getMax(), NX::OrthographicCamera::maxZoom);
        zoom->onInput->trigger(0.f);

        EXPECT_FLOAT_EQ(camera.getZoom(), NX::OrthographicCamera::minZoom) << "setZoom clamps";
        EXPECT_FLOAT_EQ(widgetOf<FloatInput>(*grid, "_far")->getMin(), 100.f);
    }

    TEST_F(EnginePanelsTests, TheNameIsEditedThroughItsSetter)
    {
        NX::SceneObj::Rectangle rectangle;
        auto grid = gridFor(rectangle);

        widgetOf<TextInput>(*grid, "_name")->onInput->trigger("Lamp");

        EXPECT_EQ(rectangle.getComponentName(), "Lamp");
    }

    TEST_F(EnginePanelsTests, ChildrenAreListedWithTheirTypes)
    {
        NX::SceneObj::Rectangle rectangle;
        rectangle.addChildComponent<NX::SceneObj::Rectangle>("Child"_atom);
        auto grid = gridFor(rectangle);
        nextFrame();
        grid->unhandledDraw();

        EXPECT_EQ(widgetOf<NumInput<std::size_t>>(*grid, "childrenCount")->getInputtedData(), 1u);
        const auto* children = widgetOf<ArrayEditor>(*grid, "_children");
        ASSERT_EQ(children->getElementRows().size(), 1u);
        EXPECT_EQ(children->getElementRows()[0].row->findFirstChildOf<TextInput>()->getInputtedData(),
                  "Child [NX::SceneObj::Rectangle]");
        EXPECT_EQ(widgetOf<TextInput>(*grid, "parent")->getInputtedData(), "None");
    }

    TEST_F(EnginePanelsTests, BlendingIsAPropertyOfTheRectangle)
    {
        NX::SceneObj::Rectangle rectangle;
        auto grid = gridFor(rectangle);
        std::vector<std::string> edited;
        auto subscription = grid->onEdited->subscribeAndGetID(
            [&](const NX::Reflection::PropertyDesc& property)
            { edited.push_back(property.name.toStdString()); });

        widgetOf<CheckBox>(*grid, "blending")->onChange->trigger(false);

        EXPECT_FALSE(rectangle.isBlendingEnabled());
        EXPECT_THAT(edited, Contains("blending"));
        grid->onEdited->unsubscribe(subscription);
    }

    TEST_F(EnginePanelsTests, AnimationRowsNeedAnAnimator)
    {
        NX::SceneObj::RectangleAnimated rectangle;
        auto grid = gridFor(rectangle);
        nextFrame();
        grid->unhandledDraw();

        EXPECT_FALSE(grid->findRow("animationEnabled"_atom)->isDisabledWidget());
        EXPECT_TRUE(grid->findRow("animation"_atom)->isDisabledWidget());
        EXPECT_TRUE(grid->findRow("animationFps"_atom)->isDisabledWidget());
        EXPECT_TRUE(choices.get("RectangleAnimation"_atom, { .owner = &rectangle }).empty());
    }

    TEST_F(EnginePanelsTests, AnimatorsLeaveTheirAnimationsToTheAnimationEditor)
    {
        NX::Animation::FrameByFrameAnimator animator;
        auto grid = gridFor(animator);

        EXPECT_EQ(grid->findRow("_animations"_atom), nullptr);
        EXPECT_TRUE(grid->findRow("_currentAnimationName"_atom)->isReadOnly());
    }

    TEST_F(EnginePanelsTests, TheCameraRowFollowsTheWorldCamera)
    {
        NX::World world;
        auto grid = gridFor(world);
        auto* row = grid->findRow("camera"_atom);
        const auto* name = row->findFirstChildOf<TextInput>();
        const auto* button = row->findFirstChildOf<Button>();

        EXPECT_TRUE(name->getInputtedData().empty());
        EXPECT_TRUE(button->isDisabledWidget());

        NX::OrthographicCamera camera("Main camera"_atom);
        world.currentCamera = &camera;
        nextFrame();
        grid->unhandledDraw();

        EXPECT_EQ(name->getInputtedData(), "Main camera");
        EXPECT_FALSE(button->isDisabledWidget());
        world.currentCamera = nullptr;
    }

    TEST_F(EnginePanelsTests, GraphicModifiersHaveAnEditor)
    {
        using Graphics = NX::BaseGraphicsData;
        GridFixtures::GraphicsSettings settings;
        auto grid = gridFor(settings);
        nextFrame();
        grid->unhandledDraw();

        const auto* modifiers = widgetOf<ArrayEditor>(*grid, "modifiers");
        ASSERT_EQ(modifiers->getElementRows().size(), 1u);
        auto* element = modifiers->getElementRows()[0].row->findFirstChildOf<PropertyRow>();
        EXPECT_EQ(element->getEditorName(), "Modifier");

        element->findFirstChildOf<EnumCombo>()->onSelect->trigger(
            EnumBits(Graphics::Modifier::Enable));

        EXPECT_EQ(settings.modifiers[0].modifier, Graphics::Modifier::Enable);
        EXPECT_EQ(settings.modifiers[0].value, Graphics::ModifiedValue::None);
    }
} // namespace
