// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/InputController.h"
#include "NxWorld/Framework/InputSystem.h"
#include "Platform/Window.h"

#include "gtest/gtest.h"

using namespace NX;
using Key = Platform::Keyboard::Key;
using State = Platform::Keyboard::KeyState;

namespace
{
    class InputControllerTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            GetInputSystem().initialize(Platform::GetWindow());
            GetInputSystem().resetInput();
            GetInputSystem().setActiveContext(InputContext::Gameplay);
        }

        void TearDown() override { GetInputSystem().resetInput(); }

        static void key(Key key, State state, int modifiers = 0)
        {
            Platform::GetWindow().onKeyPressed->trigger(key, 0, state, modifiers);
        }

        static void frame() { GetInputSystem().processEvents(); }
    };
} // namespace

TEST_F(InputControllerTests, RoutesContextsAndReleasesHeldActions)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    auto editor = InputController::Create("Editor"_atom, InputContext::Editor);
    game->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    editor->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(game->isActionPressed("Move"_atom));
    EXPECT_FALSE(editor->isActionPressed("Move"_atom));
    frame();
    EXPECT_TRUE(game->isActionPressed("Move"_atom));
    GetInputSystem().setActiveContext(InputContext::Editor);
    EXPECT_FALSE(game->isActionPressed("Move"_atom));
    key(Key::W, State::Released);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(editor->isActionPressed("Move"_atom));
}

TEST_F(InputControllerTests, MostSpecificChordWinsAndRightModifiersWork)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    int saved = 0;
    controller->bind("Plain"_atom, KeyChord::Exact(Key::S));
    controller->bind(
        "Save"_atom,
        { .triggerKey = Key::S, .requiredKeys = { Key::Left_Control, Key::Left_Shift } },
        [&](const InputActionEvent&) { ++saved; });
    key(Key::Right_Control, State::Pressed);
    key(Key::Right_Shift, State::Pressed);
    key(Key::S, State::Pressed, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT);
    frame();
    EXPECT_EQ(saved, 1);
    EXPECT_TRUE(controller->isActionPressed("Save"_atom));
    EXPECT_FALSE(controller->isActionPressed("Plain"_atom));
    key(Key::S, State::Repeated);
    frame();
    EXPECT_EQ(saved, 1);
    EXPECT_FALSE(controller->isActionPressed("Save"_atom));
}

TEST_F(InputControllerTests, ReleaseTriggerLastsOneFrame)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind("Release"_atom, KeyChord::Exact(Key::W), InputActionTrigger::OnRelease);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Release"_atom));
    key(Key::W, State::Released);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Release"_atom));
    frame();
    EXPECT_FALSE(controller->isActionPressed("Release"_atom));
}

TEST_F(InputControllerTests, DisableAndReenableDoesNotLeaveStuckKeys)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    key(Key::W, State::Pressed);
    frame();
    controller->disable();
    key(Key::W, State::Released);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Move"_atom));
    controller->enable();
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Move"_atom));
    GetInputSystem().resetInput();
    EXPECT_FALSE(controller->isActionPressed("Move"_atom));
}

TEST_F(InputControllerTests, BindingSerializationPreservesChordAndTrigger)
{
    const InputController::Binding binding{ .action = "Save"_atom,
                                            .chord = { .triggerKey = Key::S,
                                                       .requiredKeys = { Key::Left_Control } },
                                            .trigger = InputActionTrigger::OnRelease };
    const nlohmann::json json = binding;
    const auto restored = json.get<InputController::Binding>();
    EXPECT_EQ(restored.action, binding.action);
    EXPECT_EQ(restored.chord.triggerKey, Key::S);
    EXPECT_EQ(restored.chord.requiredKeys, binding.chord.requiredKeys);
    EXPECT_EQ(restored.trigger, binding.trigger);
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->setBindings({ restored });
    EXPECT_TRUE(controller->getTags() & Tag_InputController);
    EXPECT_EQ(TagHelper::ToTag("InputController"), Tag_InputController);
    EXPECT_EQ(controller->serialize()["_bindings"][0], json);
}

TEST_F(InputControllerTests, CallbackCanDestroyAnotherRegisteredController)
{
    auto first = InputController::Create("First"_atom, InputContext::Gameplay);
    auto second = InputController::Create("Second"_atom, InputContext::Gameplay);
    int calls = 0;
    first->bind("Delete"_atom, KeyChord::Exact(Key::W),
                [&](const InputActionEvent&) { second = nullptr; });
    second->bind("Move"_atom, KeyChord::Exact(Key::W), [&](const InputActionEvent&) { ++calls; });
    key(Key::W, State::Pressed);
    frame();
    EXPECT_EQ(calls, 0);
}

TEST_F(InputControllerTests, EditingBindingsPreservesCallbacksAndResetsHeldState)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    int calls = 0;
    controller->bind(
        "Move"_atom, KeyChord::Exact(Key::W), [&](const InputActionEvent&) { ++calls; },
        InputActionTrigger::WhileHeld);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_EQ(calls, 1);
    controller->setBindings(controller->getBindings());
    EXPECT_FALSE(controller->isActionPressed("Move"_atom));
    ASSERT_EQ(controller->getBindings().size(), 1);
    key(Key::W, State::Released);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_EQ(calls, 2);
}

TEST_F(InputControllerTests, CloneHasIndependentRuntimeStateAndRegistersOnInitialize)
{
    auto original = InputController::Create("Original"_atom, InputContext::Gameplay);
    original->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    key(Key::W, State::Pressed);
    frame();
    auto clonedBase = original->clone();
    auto* clone = dynamic_cast<InputController*>(clonedBase.get());
    ASSERT_NE(clone, nullptr);
    EXPECT_FALSE(clone->isActionPressed("Move"_atom));
    EXPECT_NE(clone->onAction.get(), original->onAction.get());
    clone->initialize();
    key(Key::W, State::Released);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(clone->isActionPressed("Move"_atom));
}

TEST_F(InputControllerTests, DeserializedComponentReceivesInput)
{
    InputController original;
    original.bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    RResourceStream<RJsonResourceStream> stream(original.serialize());
    auto restored = InputController::Create();
    restored->deserialize(stream);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(restored->isActionPressed("Move"_atom));
    restored->deserialize(stream);
    EXPECT_FALSE(restored->isActionPressed("Move"_atom));
}

TEST_F(InputControllerTests, FocusLossClearsHeldAndQueuedInputEvenIfFocusReturnsBeforeNextFrame)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    controller->bind("Queued"_atom, KeyChord::Exact(Key::S));
    key(Key::W, State::Pressed);
    frame();
    ASSERT_TRUE(controller->isActionPressed("Move"_atom));
    key(Key::S, State::Pressed);
    Platform::GetWindow().onFocusChanged->trigger(false);
    EXPECT_FALSE(controller->isActionPressed("Move"_atom));
    Platform::GetWindow().onFocusChanged->trigger(true);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Queued"_atom));
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Move"_atom));
}

TEST_F(InputControllerTests, ReleasingRequiredKeyEndsHeldChord)
{
    auto controller = InputController::Create("Game"_atom, InputContext::Gameplay);
    controller->bind("Move"_atom, { .triggerKey = Key::W, .requiredKeys = { Key::Left_Control } },
                     InputActionTrigger::WhileHeld);
    key(Key::Left_Control, State::Pressed);
    key(Key::W, State::Pressed);
    frame();
    EXPECT_TRUE(controller->isActionPressed("Move"_atom));
    key(Key::Left_Control, State::Released);
    frame();
    EXPECT_FALSE(controller->isActionPressed("Move"_atom));
}
