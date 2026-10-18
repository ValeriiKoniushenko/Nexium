// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/Input/AIController.h"
#include "NxWorld/Framework/Input/EditorController.h"
#include "NxWorld/Framework/Input/HumanController.h"
#include "NxWorld/Framework/Input/InputSystem.h"
#include "Platform/Window.h"
#include "TestHumanController.h"

#include "gtest/gtest.h"
#include <concepts>

using namespace NX;

namespace NX
{
    // tag::input-custom-registration[]
    ECS_IMPL(TestHumanController);
    // end::input-custom-registration[]
} // namespace NX

namespace
{
    class PossessionSpy : public GameController
    {
    public:
        int possessions = 0;
        int releases = 0;

    protected:
        void onPossess(Actor&) override { ++possessions; }
        void onUnpossess(Actor&) override { ++releases; }
    };

    class ControllerHierarchyTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            GetInputSystem().initialize(Platform::GetWindow());
            GetInputSystem().resetInput();
            GetInputSystem().setActiveContext(InputContext::Gameplay);
        }

        void TearDown() override { GetInputSystem().resetInput(); }

        static void pressW()
        {
            Platform::GetWindow().onKeyPressed->trigger(Platform::Keyboard::Key::W, 0,
                                                        Platform::Keyboard::KeyState::Pressed, 0);
            GetInputSystem().processEvents();
        }
    };
} // namespace

TEST_F(ControllerHierarchyTests, ControllersFollowRequestedHierarchyAndRegisterWithFactory)
{
    static_assert(std::derived_from<HumanController, GameController>);
    static_assert(std::derived_from<AIController, GameController>);
    static_assert(std::derived_from<GameController, BaseController>);
    static_assert(std::derived_from<EditorController, BaseController>);

    EXPECT_TRUE(GetGlobalComponentFactory().containsSuchType(BaseController::componentType));
    EXPECT_TRUE(GetGlobalComponentFactory().containsSuchType(GameController::componentType));
    EXPECT_TRUE(GetGlobalComponentFactory().containsSuchType(HumanController::componentType));
    EXPECT_TRUE(GetGlobalComponentFactory().containsSuchType(AIController::componentType));
    EXPECT_TRUE(GetGlobalComponentFactory().containsSuchType(EditorController::componentType));
}

TEST_F(ControllerHierarchyTests, HumanAndEditorOwnInputWithIndependentContexts)
{
    auto human = HumanController::Create();
    auto editor = EditorController::Create();
    auto& humanInput = human->getInput();
    auto& editorInput = editor->getInput();
    EXPECT_EQ(humanInput.getParent(), human.get());
    EXPECT_EQ(editorInput.getParent(), editor.get());
    EXPECT_EQ(humanInput.getInputContext(), InputContext::Gameplay);
    EXPECT_EQ(editorInput.getInputContext(), InputContext::Editor);

    humanInput.bind("Move"_atom, KeyChord::Exact(Platform::Keyboard::Key::W),
                    InputActionTrigger::WhileHeld);
    editorInput.bind("Move"_atom, KeyChord::Exact(Platform::Keyboard::Key::W),
                     InputActionTrigger::WhileHeld);
    pressW();
    EXPECT_TRUE(humanInput.isActive("Move"_atom));
    EXPECT_FALSE(editorInput.isActive("Move"_atom));
}

TEST_F(ControllerHierarchyTests, DisabledOwnerCannotReceiveHardwareInput)
{
    auto human = HumanController::Create();
    auto& input = human->getInput();
    input.bind("Move"_atom, KeyChord::Exact(Platform::Keyboard::Key::W),
               InputActionTrigger::WhileHeld);
    human->disable();
    pressW();
    EXPECT_FALSE(input.isActive("Move"_atom));
}

// tag::input-ai-possession[]
TEST_F(ControllerHierarchyTests, AIControllerNeedsNoHardwareInputComponent)
{
    auto ai = AIController::Create();
    ai->initialize();
    EXPECT_FALSE(ai->hasChildrenAs<InputController>());
    auto actor = Actor::Create();
    ai->possess(actor.get());
    EXPECT_EQ(ai->getControlledActor(), actor.get());
}
// end::input-ai-possession[]

TEST_F(ControllerHierarchyTests, CustomControllerCanExtendPossessionWithoutHandlingRawInput)
{
    PossessionSpy controller;
    auto first = Actor::Create();
    auto second = Actor::Create();
    controller.possess(first.get());
    controller.possess(first.get());
    EXPECT_EQ(controller.possessions, 1);
    EXPECT_EQ(controller.releases, 0);
    controller.possess(second.get());
    EXPECT_EQ(controller.possessions, 2);
    EXPECT_EQ(controller.releases, 1);
    EXPECT_EQ(controller.getControlledActor(), second.get());
    controller.deinitialize();
    EXPECT_EQ(controller.releases, 2);
    EXPECT_EQ(controller.getControlledActor(), nullptr);
}

// tag::input-possession-lifetime[]
TEST_F(ControllerHierarchyTests, PossessionDoesNotKeepActorAliveOrTransferToClone)
{
    auto controller = GameController::Create();
    auto actor = Actor::Create();
    controller->possess(actor.get());
    auto clone = controller->clone();
    EXPECT_EQ(clone->castTo<GameController>()->getControlledActor(), nullptr);
    actor.reset();
    EXPECT_EQ(controller->getControlledActor(), nullptr);
    controller->unpossess();
}
// end::input-possession-lifetime[]

// tag::input-human-clone[]
TEST_F(ControllerHierarchyTests, ClonedHumanOwnsIndependentInputBindings)
{
    auto human = HumanController::Create();
    human->getInput().bind("Move"_atom, KeyChord::Exact(Platform::Keyboard::Key::W));
    auto clone = human->clone();
    auto& cloneInput = clone->castTo<HumanController>()->getInput();
    EXPECT_NE(&cloneInput, &human->getInput());
    EXPECT_EQ(cloneInput.getParent(), clone.get());
    EXPECT_EQ(clone->getChildrenCount(), 1);
    cloneInput.clearBindings();
    EXPECT_EQ(human->getInput().getBindings().size(), 1);
}
// end::input-human-clone[]

TEST_F(ControllerHierarchyTests, DeserializedHumanReusesItsInputChild)
{
    auto human = HumanController::Create();
    human->getInput().bind("Move"_atom, KeyChord::Exact(Platform::Keyboard::Key::W),
                           InputActionTrigger::WhileHeld);
    RResourceStream<RJsonResourceStream> stream(human->serialize());
    auto restored = HumanController::Create();
    restored->deserialize(stream);
    EXPECT_EQ(restored->getChildrenCount(), 1);
    EXPECT_EQ(restored->getInput().getParent(), restored.get());
    EXPECT_EQ(restored->getInput().getInputContext(), InputContext::Gameplay);
    pressW();
    EXPECT_TRUE(restored->getInput().isActive("Move"_atom));
}

// tag::input-custom-human-usage[]
TEST_F(ControllerHierarchyTests, CustomHumanControllerRegistersAndDrivesItsPossessedActor)
{
    auto controller = TestHumanController::Create();
    auto actor = Actor::Create();
    actor->setPosition({ 0.f, 0.f, 0.f });
    controller->possess(actor.get());
    controller->getInput().bind("Run"_atom, KeyChord::Exact(Platform::Keyboard::Key::W),
                                InputActionTrigger::WhileHeld);
    EXPECT_TRUE(GetGlobalComponentFactory().containsSuchType(TestHumanController::componentType));
    pressW();
    controller->tick(2.f);
    EXPECT_EQ(actor->getPosition().x, 2.f);
    controller->unpossess();
    controller->tick(2.f);
    EXPECT_EQ(actor->getPosition().x, 2.f);
}
// end::input-custom-human-usage[]
