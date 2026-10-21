// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/Input/InputController.h"
#include "NxWorld/Framework/Input/InputSystem.h"
#include "NxWorld/Scene/Spectator.h"
#include "Platform/Window.h"

#include "gtest/gtest.h"

using namespace NX;
using Key = Platform::Keyboard::Key;
using KeyState = Platform::Keyboard::KeyState;

namespace
{
    class InputRoutingTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            auto& system = GetInputSystem();
            system.initialize(Platform::GetWindow());
            system.resetInput();
            system.setGlobalContext(std::nullopt);
            system.setActiveContext(InputContext::Gameplay);
        }
        void TearDown() override
        {
            GetInputSystem().resetInput();
            GetInputSystem().setGlobalContext(std::nullopt);
        }
        static void key(Key key, KeyState state)
        {
            Platform::GetWindow().onKeyPressed->trigger(key, 0, state, 0);
        }
        static void mouse(Platform::Mouse::State state)
        {
            Platform::GetWindow().onMouseKeyPressed->trigger(Platform::Mouse::Key::Right, state,
                                                             Platform::Mouse::Mod{});
        }
        static void frame() { GetInputSystem().processEvents(); }

        static void expectConsumingOverlayInterruptsIdleDrag(bool moveBeforeIdle)
        {
            SCOPED_TRACE(moveBeforeIdle);
            auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
            auto menu = InputController::Create("Menu"_atom, InputContext::Modal);
            game->bind({ .action = "Look"_atom,
                         .chord = { .triggerKey = Platform::Mouse::Key::Right,
                                    .requiredKeys = { Key::W, Key::Left_Control } },
                         .source = InputEventType::PointerMove });
            game->bind("Other"_atom, KeyChord::Exact(Key::D), InputActionTrigger::WhileHeld);
            menu->bind({ .action = "Drag"_atom,
                         .chord = KeyChord::Exact(Platform::Mouse::Key::Right),
                         .source = InputEventType::PointerMove });
            key(Key::Right_Control, KeyState::Pressed);
            key(Key::W, KeyState::Pressed);
            key(Key::D, KeyState::Pressed);
            Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 10.f, 10.f });
            mouse(Platform::Mouse::State::Press);
            if (moveBeforeIdle)
            {
                Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 12.f, 15.f });
            }
            frame();
            ASSERT_TRUE(GetInputSystem().hasPointerCapture());
            if (moveBeforeIdle)
            {
                ASSERT_TRUE(game->isActive("Look"_atom));
                frame();
            }
            ASSERT_FALSE(game->isActive("Look"_atom));

            auto overlay = GetInputSystem().pushContext({ .context = InputContext::Modal });
            EXPECT_FALSE(GetInputSystem().hasPointerCapture());
            EXPECT_TRUE(game->isActive("Other"_atom));
            Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 14.f, 20.f });
            frame();
            EXPECT_FALSE(game->isActive("Look"_atom));
            EXPECT_FALSE(menu->isActive("Drag"_atom));
            EXPECT_FALSE(GetInputSystem().hasPointerCapture());

            overlay.reset();
            mouse(Platform::Mouse::State::Release);
            mouse(Platform::Mouse::State::Press);
            Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 15.f, 21.f });
            frame();
            EXPECT_FALSE(game->isActive("Look"_atom));
            EXPECT_FALSE(GetInputSystem().hasPointerCapture());
            EXPECT_TRUE(game->isActive("Other"_atom));

            key(Key::W, KeyState::Released);
            mouse(Platform::Mouse::State::Release);
            key(Key::W, KeyState::Pressed);
            mouse(Platform::Mouse::State::Press);
            Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 16.f, 22.f });
            frame();
            EXPECT_TRUE(game->isActive("Look"_atom));
            EXPECT_TRUE(GetInputSystem().hasPointerCapture());
            EXPECT_TRUE(game->isActive("Other"_atom));
            EXPECT_EQ(game->getActionModifiers("Look"_atom), InputModifier_Control);
        }
    };
} // namespace

TEST_F(InputRoutingTests, ZeroPointerMovementIsIgnoredWhileButtonPressesAreDelivered)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    int keyPresses = 0;
    int mousePresses = 0;
    int movements = 0;
    game->bind("Key"_atom, KeyChord::Exact(Key::W), [&](const InputActionEvent&) { ++keyPresses; });
    game->bind("Click"_atom, KeyChord::Exact(Platform::Mouse::Key::Right),
               [&](const InputActionEvent&) { ++mousePresses; });
    game->bind({ .action = "Move"_atom, .source = InputEventType::PointerMove });
    game->setActionCallback("Move"_atom, [&](const InputActionEvent&) { ++movements; });

    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 10.f, 10.f });
    key(Key::W, KeyState::Pressed);
    mouse(Platform::Mouse::State::Press);
    frame();
    EXPECT_EQ(keyPresses, 1);
    EXPECT_EQ(mousePresses, 1);
    EXPECT_EQ(movements, 0);
    EXPECT_FALSE(game->isActive("Move"_atom));

    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 10.f, 10.f });
    frame();
    EXPECT_EQ(movements, 0);
    EXPECT_FALSE(game->isActive("Move"_atom));

    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 12.f, 15.f });
    frame();
    EXPECT_EQ(movements, 1);
    EXPECT_EQ(game->getActionValue("Move"_atom), glm::vec2(2.f, 5.f));
}

// tag::input-modal[]
TEST_F(InputRoutingTests, ModalBlocksUnmatchedInputAndCancelsWithoutReleaseAction)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    int releases = 0;
    game->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    game->bind(
        "Release"_atom, KeyChord::Exact(Key::S), [&](const InputActionEvent&) { ++releases; },
        InputActionTrigger::OnRelease);
    key(Key::W, KeyState::Pressed);
    key(Key::S, KeyState::Pressed);
    frame();
    ASSERT_TRUE(game->isActive("Move"_atom));
    auto modal = GetInputSystem().pushContext(
        { .context = InputContext::Modal, .policy = InputContextPolicy::BlockLower });
    EXPECT_FALSE(game->isActive("Move"_atom));
    key(Key::S, KeyState::Released);
    frame();
    EXPECT_EQ(releases, 0);
    modal.reset();
    frame();
    EXPECT_FALSE(game->isActive("Move"_atom));
    key(Key::W, KeyState::Released);
    key(Key::W, KeyState::Pressed);
    frame();
    EXPECT_TRUE(game->isActive("Move"_atom));
}
// end::input-modal[]

// tag::input-text-input[]
TEST_F(InputRoutingTests, TextInputBlocksKeyboardWhilePointerRemainsAvailable)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    game->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    game->bind({ .action = "Scroll"_atom, .source = InputEventType::Wheel });
    key(Key::W, KeyState::Pressed);
    frame();
    auto text = GetInputSystem().pushContext({ .context = InputContext::TextInput,
                                               .policy = InputContextPolicy::BlockLower,
                                               .channels = InputChannel_Keyboard });
    EXPECT_FALSE(game->isActive("Move"_atom));
    Platform::GetWindow().onMouseWheel->trigger(glm::vec2{ 0.f, 2.f });
    frame();
    EXPECT_TRUE(game->isActive("Scroll"_atom));
    EXPECT_EQ(game->getActionValue("Scroll"_atom), glm::vec2(0.f, 2.f));
    frame();
    EXPECT_FALSE(game->isActive("Scroll"_atom));
}
// end::input-text-input[]

TEST_F(InputRoutingTests, KeyboardOnlyOverlayPreservesAcceptedPointerDrag)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    game->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    game->bind({ .action = "Look"_atom,
                 .chord = KeyChord::Exact(Platform::Mouse::Key::Right),
                 .source = InputEventType::PointerMove });
    key(Key::W, KeyState::Pressed);
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 10.f, 10.f });
    mouse(Platform::Mouse::State::Press);
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 12.f, 15.f });
    frame();
    ASSERT_TRUE(game->isActive("Move"_atom));
    ASSERT_TRUE(GetInputSystem().hasPointerCapture());

    auto text = GetInputSystem().pushContext({ .context = InputContext::TextInput,
                                               .policy = InputContextPolicy::BlockLower,
                                               .channels = InputChannel_Keyboard });
    EXPECT_FALSE(game->isActive("Move"_atom));
    EXPECT_TRUE(game->isActive("Look"_atom));
    EXPECT_TRUE(GetInputSystem().hasPointerCapture());
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 15.f, 19.f });
    frame();
    EXPECT_TRUE(game->isActive("Look"_atom));
    EXPECT_EQ(game->getActionValue("Look"_atom), glm::vec2(3.f, 4.f));
    EXPECT_TRUE(GetInputSystem().hasPointerCapture());
}

TEST_F(InputRoutingTests, RestoredKeyboardChannelQuarantinesButtonsPressedWhileUnavailable)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    game->bind("Chord"_atom, { .triggerKey = Key::S, .requiredKeys = { Key::W } });
    auto text = GetInputSystem().pushContext({ .context = InputContext::TextInput,
                                               .policy = InputContextPolicy::BlockLower,
                                               .channels = InputChannel_Keyboard });
    key(Key::W, KeyState::Pressed);
    frame();
    text.reset();
    key(Key::S, KeyState::Pressed);
    frame();
    EXPECT_FALSE(game->isActive("Chord"_atom));

    key(Key::S, KeyState::Released);
    key(Key::W, KeyState::Released);
    key(Key::W, KeyState::Pressed);
    key(Key::S, KeyState::Pressed);
    frame();
    EXPECT_TRUE(game->isActive("Chord"_atom));
}

TEST_F(InputRoutingTests, RestoredChannelsQuarantineKeyboardAndMouseButPreserveHeldModifier)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    game->bind("Keyboard"_atom,
               { .triggerKey = Key::S, .requiredKeys = { Key::W, Key::Left_Control } },
               InputActionTrigger::WhileHeld);
    game->bind("Pointer"_atom,
               { .triggerKey = Platform::Mouse::Key::Left,
                 .requiredKeys = { Platform::Mouse::Key::Right, Key::Left_Control } },
               InputActionTrigger::WhileHeld);
    const auto leftMouse = [](Platform::Mouse::State state)
    {
        Platform::GetWindow().onMouseKeyPressed->trigger(Platform::Mouse::Key::Left, state,
                                                         Platform::Mouse::Mod{});
    };
    key(Key::Right_Control, KeyState::Pressed);
    key(Key::W, KeyState::Pressed);
    mouse(Platform::Mouse::State::Press);
    key(Key::S, KeyState::Pressed);
    leftMouse(Platform::Mouse::State::Press);
    frame();
    ASSERT_TRUE(game->isActive("Keyboard"_atom));
    ASSERT_TRUE(game->isActive("Pointer"_atom));

    auto modal = GetInputSystem().pushContext(
        { .context = InputContext::Modal, .policy = InputContextPolicy::BlockLower });
    EXPECT_FALSE(game->isActive("Keyboard"_atom));
    EXPECT_FALSE(game->isActive("Pointer"_atom));
    modal.reset();
    key(Key::S, KeyState::Released);
    leftMouse(Platform::Mouse::State::Release);
    key(Key::S, KeyState::Pressed);
    leftMouse(Platform::Mouse::State::Press);
    frame();
    EXPECT_FALSE(game->isActive("Keyboard"_atom));
    EXPECT_FALSE(game->isActive("Pointer"_atom));

    key(Key::S, KeyState::Released);
    leftMouse(Platform::Mouse::State::Release);
    key(Key::W, KeyState::Released);
    mouse(Platform::Mouse::State::Release);
    key(Key::W, KeyState::Pressed);
    mouse(Platform::Mouse::State::Press);
    key(Key::S, KeyState::Pressed);
    leftMouse(Platform::Mouse::State::Press);
    frame();
    EXPECT_TRUE(game->isActive("Keyboard"_atom));
    EXPECT_TRUE(game->isActive("Pointer"_atom));
    EXPECT_EQ(game->getActionModifiers("Keyboard"_atom), InputModifier_Control);
    EXPECT_EQ(game->getActionModifiers("Pointer"_atom), InputModifier_Control);
}

// tag::input-consume-matched[]
TEST_F(InputRoutingTests, ConsumingLayerDeliversToAllItsControllersAndPassesUnmatchedInput)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    auto first = InputController::Create("First menu"_atom, InputContext::Modal);
    auto second = InputController::Create("Second menu"_atom, InputContext::Modal);
    game->bind("Move"_atom, KeyChord::Exact(Key::W));
    game->bind("Other"_atom, KeyChord::Exact(Key::D));
    first->bind("Select"_atom, KeyChord::Exact(Key::W));
    second->bind("Select"_atom, KeyChord::Exact(Key::W));
    auto menu = GetInputSystem().pushContext({ .context = InputContext::Modal });
    key(Key::W, KeyState::Pressed);
    key(Key::D, KeyState::Pressed);
    frame();
    EXPECT_TRUE(first->isActive("Select"_atom));
    EXPECT_TRUE(second->isActive("Select"_atom));
    EXPECT_FALSE(game->isActive("Move"_atom));
    EXPECT_TRUE(game->isActive("Other"_atom));
}
// end::input-consume-matched[]

TEST_F(InputRoutingTests, ConsumingDragBindingOwnsPressWithoutMatchingButtonBinding)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    auto menu = InputController::Create("Menu"_atom, InputContext::Modal);
    int clicks = 0;
    game->bind("Click"_atom, KeyChord::Exact(Platform::Mouse::Key::Right),
               [&](const InputActionEvent&) { ++clicks; });
    game->bind({ .action = "Look"_atom,
                 .chord = KeyChord::Exact(Platform::Mouse::Key::Right),
                 .source = InputEventType::PointerMove });
    menu->bind({ .action = "Drag"_atom,
                 .chord = KeyChord::Exact(Platform::Mouse::Key::Right),
                 .source = InputEventType::PointerMove });
    auto overlay = GetInputSystem().pushContext({ .context = InputContext::Modal });

    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 10.f, 10.f });
    mouse(Platform::Mouse::State::Press);
    frame();
    EXPECT_EQ(clicks, 0);
    EXPECT_FALSE(menu->isActive("Drag"_atom));
    EXPECT_TRUE(GetInputSystem().hasPointerCapture());

    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 12.f, 15.f });
    frame();
    EXPECT_TRUE(menu->isActive("Drag"_atom));
    EXPECT_EQ(menu->getActionValue("Drag"_atom), glm::vec2(2.f, 5.f));
    EXPECT_EQ(clicks, 0);
    EXPECT_FALSE(game->isActive("Look"_atom));

    mouse(Platform::Mouse::State::Release);
    frame();
    EXPECT_FALSE(GetInputSystem().hasPointerCapture());
}

TEST_F(InputRoutingTests, ConsumingOverlayCancelsDragCaptureBeforeFirstMovement)
{
    expectConsumingOverlayInterruptsIdleDrag(false);
}

TEST_F(InputRoutingTests, ConsumingOverlayCancelsDragCaptureAfterIdleFrame)
{
    expectConsumingOverlayInterruptsIdleDrag(true);
}

TEST_F(InputRoutingTests, UnmatchedConsumingOverlayPreservesIdleDragCapture)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    auto menu = InputController::Create("Menu"_atom, InputContext::Modal);
    game->bind({ .action = "Look"_atom,
                 .chord = KeyChord::Exact(Platform::Mouse::Key::Right),
                 .source = InputEventType::PointerMove });
    menu->bind({ .action = "Drag"_atom,
                 .chord = KeyChord::Exact(Platform::Mouse::Key::Left),
                 .source = InputEventType::PointerMove });
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 10.f, 10.f });
    mouse(Platform::Mouse::State::Press);
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 12.f, 15.f });
    frame();
    frame();
    ASSERT_FALSE(game->isActive("Look"_atom));
    ASSERT_TRUE(GetInputSystem().hasPointerCapture());

    auto overlay = GetInputSystem().pushContext({ .context = InputContext::Modal });
    EXPECT_TRUE(GetInputSystem().hasPointerCapture());
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 14.f, 20.f });
    frame();
    EXPECT_TRUE(game->isActive("Look"_atom));
    EXPECT_EQ(game->getActionValue("Look"_atom), glm::vec2(2.f, 5.f));
    EXPECT_FALSE(menu->isActive("Drag"_atom));
    EXPECT_TRUE(GetInputSystem().hasPointerCapture());
}

// tag::input-selective-cancel[]
TEST_F(InputRoutingTests, ConsumingOverlayCancelsOnlyShadowedHeldBindings)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    auto menu = InputController::Create("Menu"_atom, InputContext::Modal);
    game->bind("Move"_atom, KeyChord::Exact(Key::W), InputActionTrigger::WhileHeld);
    game->bind("Other"_atom, KeyChord::Exact(Key::D), InputActionTrigger::WhileHeld);
    menu->bind("Select"_atom, KeyChord::Exact(Key::W));
    key(Key::W, KeyState::Pressed);
    key(Key::D, KeyState::Pressed);
    frame();
    auto overlay = GetInputSystem().pushContext({ .context = InputContext::Modal });
    EXPECT_FALSE(game->isActive("Move"_atom));
    EXPECT_TRUE(game->isActive("Other"_atom));
    EXPECT_FALSE(menu->isActive("Select"_atom));
    overlay.reset();
    frame();
    EXPECT_FALSE(game->isActive("Move"_atom));
}
// end::input-selective-cancel[]

TEST_F(InputRoutingTests, NewContextCanUseHeldModifierWithFreshTrigger)
{
    auto editor = InputController::Create("Editor"_atom, InputContext::Editor);
    editor->bind("Save"_atom, { .triggerKey = Key::S, .requiredKeys = { Key::Left_Control } });
    key(Key::Right_Control, KeyState::Pressed);
    frame();
    GetInputSystem().setActiveContext(InputContext::Editor);
    key(Key::S, KeyState::Pressed);
    frame();
    EXPECT_TRUE(editor->isActive("Save"_atom));
}

TEST_F(InputRoutingTests, ContextSwitchInsideCallbackSkipsOtherRecipients)
{
    auto first = InputController::Create("First"_atom, InputContext::Gameplay);
    auto second = InputController::Create("Second"_atom, InputContext::Gameplay);
    int calls = 0;
    first->bind("Switch"_atom, KeyChord::Exact(Key::W), [](const InputActionEvent&)
                { GetInputSystem().setActiveContext(InputContext::Editor); });
    second->bind("Move"_atom, KeyChord::Exact(Key::W), [&](const InputActionEvent&) { ++calls; });
    key(Key::W, KeyState::Pressed);
    frame();
    EXPECT_EQ(calls, 0);
    EXPECT_FALSE(first->isActive("Switch"_atom));
}

TEST_F(InputRoutingTests, ContextSwitchInsideCallbackRoutesNextQueuedEventToNewContext)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    auto editor = InputController::Create("Editor"_atom, InputContext::Editor);
    int gameCalls = 0;
    int editorCalls = 0;
    game->bind("Switch"_atom, KeyChord::Exact(Key::W), [](const InputActionEvent&)
               { GetInputSystem().setActiveContext(InputContext::Editor); });
    game->bind("Move"_atom, KeyChord::Exact(Key::D), [&](const InputActionEvent&) { ++gameCalls; });
    editor->bind("Move"_atom, KeyChord::Exact(Key::D),
                 [&](const InputActionEvent&) { ++editorCalls; });
    key(Key::W, KeyState::Pressed);
    key(Key::D, KeyState::Pressed);
    frame();

    EXPECT_EQ(gameCalls, 0);
    EXPECT_EQ(editorCalls, 1);
    EXPECT_FALSE(game->isActive("Switch"_atom));
    EXPECT_TRUE(editor->isActive("Move"_atom));
}

TEST_F(InputRoutingTests, ReregisteringRecipientInsideCallbackInvalidatesSelectedHandle)
{
    auto first = InputController::Create("First"_atom, InputContext::Gameplay);
    auto second = InputController::Create("Second"_atom, InputContext::Gameplay);
    bool reregistered = false;
    int calls = 0;
    first->bind("Restart"_atom, KeyChord::Exact(Key::W),
                [&](const InputActionEvent&)
                {
                    if (!reregistered)
                    {
                        second->deinitialize();
                        second->initialize();
                        reregistered = true;
                    }
                });
    second->bind(
        "Move"_atom, KeyChord::Exact(Key::W), [&](const InputActionEvent&) { ++calls; },
        InputActionTrigger::WhileHeld);
    key(Key::W, KeyState::Pressed);
    frame();
    EXPECT_TRUE(reregistered);
    EXPECT_EQ(calls, 0);
    EXPECT_FALSE(second->isActive("Move"_atom));

    key(Key::W, KeyState::Released);
    key(Key::W, KeyState::Pressed);
    frame();
    EXPECT_EQ(calls, 1);
    EXPECT_TRUE(second->isActive("Move"_atom));
}

TEST_F(InputRoutingTests, RebindingAnotherRecipientInvalidatesPreviouslySelectedBinding)
{
    auto first = InputController::Create("First"_atom, InputContext::Gameplay);
    auto second = InputController::Create("Second"_atom, InputContext::Gameplay);
    int calls = 0;
    first->bind("Edit"_atom, KeyChord::Exact(Key::W),
                [&](const InputActionEvent&) { second->clearBindings(); });
    second->bind("Removed"_atom, KeyChord::Exact(Key::W),
                 [&](const InputActionEvent&) { ++calls; });
    key(Key::W, KeyState::Pressed);
    frame();
    EXPECT_EQ(calls, 0);
    EXPECT_FALSE(second->isActive("Removed"_atom));
}

// tag::input-drag-ownership[]
TEST_F(InputRoutingTests, DragRequiresAcceptedPressAndModalCancelsItsOwnership)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    game->bind({ .action = "Look"_atom,
                 .chord = KeyChord::Exact(Platform::Mouse::Key::Right),
                 .source = InputEventType::PointerMove });
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 10.f, 10.f });
    mouse(Platform::Mouse::State::Press);
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 12.f, 15.f });
    frame();
    EXPECT_TRUE(game->isActive("Look"_atom));
    EXPECT_EQ(game->getActionValue("Look"_atom), glm::vec2(2.f, 5.f));
    EXPECT_TRUE(GetInputSystem().hasPointerCapture());
    auto modal = GetInputSystem().pushContext(
        { .context = InputContext::Modal, .policy = InputContextPolicy::BlockLower });
    EXPECT_FALSE(GetInputSystem().hasPointerCapture());
    EXPECT_FALSE(game->isActive("Look"_atom));
    modal.reset();
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 14.f, 20.f });
    frame();
    EXPECT_FALSE(game->isActive("Look"_atom));
    mouse(Platform::Mouse::State::Release);
    mouse(Platform::Mouse::State::Press);
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 15.f, 21.f });
    frame();
    EXPECT_TRUE(game->isActive("Look"_atom));
}
// end::input-drag-ownership[]

TEST_F(InputRoutingTests, ConsumingOverlayCancelsDragAndQuarantinesRequiredNonModifier)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    auto menu = InputController::Create("Menu"_atom, InputContext::Modal);
    game->bind({ .action = "Look"_atom,
                 .chord = { .triggerKey = Platform::Mouse::Key::Right,
                            .requiredKeys = { Key::W, Key::Left_Control } },
                 .source = InputEventType::PointerMove });
    menu->bind({ .action = "Drag"_atom,
                 .chord = KeyChord::Exact(Platform::Mouse::Key::Right),
                 .source = InputEventType::PointerMove });
    key(Key::Right_Control, KeyState::Pressed);
    key(Key::W, KeyState::Pressed);
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 10.f, 10.f });
    mouse(Platform::Mouse::State::Press);
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 12.f, 15.f });
    frame();
    ASSERT_TRUE(game->isActive("Look"_atom));
    ASSERT_TRUE(GetInputSystem().hasPointerCapture());

    auto overlay = GetInputSystem().pushContext({ .context = InputContext::Modal });
    EXPECT_FALSE(game->isActive("Look"_atom));
    EXPECT_FALSE(GetInputSystem().hasPointerCapture());
    overlay.reset();
    mouse(Platform::Mouse::State::Release);
    mouse(Platform::Mouse::State::Press);
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 14.f, 20.f });
    frame();
    EXPECT_FALSE(game->isActive("Look"_atom));
    EXPECT_FALSE(GetInputSystem().hasPointerCapture());

    key(Key::W, KeyState::Released);
    mouse(Platform::Mouse::State::Release);
    key(Key::W, KeyState::Pressed);
    mouse(Platform::Mouse::State::Press);
    Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 15.f, 21.f });
    frame();
    EXPECT_TRUE(game->isActive("Look"_atom));
    EXPECT_TRUE(GetInputSystem().hasPointerCapture());
    EXPECT_EQ(game->getActionModifiers("Look"_atom), InputModifier_Control);
}

// tag::input-guard-lifetime[]
TEST_F(InputRoutingTests, OverlayGuardsCanMoveAndCloseOutOfOrder)
{
    auto game = InputController::Create("Game"_atom, InputContext::Gameplay);
    game->bind("Move"_atom, KeyChord::Exact(Key::W));
    auto lower = GetInputSystem().pushContext(
        { .context = InputContext::Modal, .policy = InputContextPolicy::BlockLower });
    auto upper = GetInputSystem().pushContext(
        { .context = InputContext::TextInput, .policy = InputContextPolicy::BlockLower });
    auto moved = std::move(upper);
    lower.reset();
    key(Key::W, KeyState::Pressed);
    frame();
    EXPECT_FALSE(game->isActive("Move"_atom));
    moved.reset();
    key(Key::W, KeyState::Released);
    key(Key::W, KeyState::Pressed);
    frame();
    EXPECT_TRUE(game->isActive("Move"_atom));
}
// end::input-guard-lifetime[]

TEST_F(InputRoutingTests, SpectatorInitializationPreservesItsCustomInputMap)
{
    auto spectator = Spectator3D::Create();
    spectator->initialize();
    spectator->getInputController()->setBindings({ { .action = "Move forward"_atom,
                                                     .chord = KeyChord::Exact(Key::Up),
                                                     .trigger = InputActionTrigger::WhileHeld } });
    spectator->deinitialize();
    spectator->initialize();
    const auto* input = spectator->getInputController();
    ASSERT_NE(input, nullptr);
    ASSERT_EQ(input->getBindings().size(), 1);
    EXPECT_EQ(input->getBindings()[0].chord.triggerKey, Key::Up);
    key(Key::W, KeyState::Pressed);
    frame();
    EXPECT_FALSE(input->isActive("Move forward"_atom));
    key(Key::Up, KeyState::Pressed);
    frame();
    EXPECT_TRUE(input->isActive("Move forward"_atom));
}

TEST_F(InputRoutingTests, SpectatorPanTracksPointerDistanceAtDifferentZoomLevels)
{
    auto spectator = Spectator2D::Create();
    auto* camera = spectator->addChildComponent<OrthographicCamera>();
    spectator->initialize();
    spectator->getInputController()->speed = 6000.f;

    for (const float zoom : { 0.5f, 1.f, 2.f })
    {
        SCOPED_TRACE(zoom);
        camera->setZoom(zoom);
        spectator->setPosition(GPos3{ 0.f, 0.f, 0.f });
        Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 100.f, 100.f });
        Platform::GetWindow().onMouseKeyPressed->trigger(
            Platform::Mouse::Key::Middle, Platform::Mouse::State::Press, Platform::Mouse::Mod{});
        Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 104.f, 103.f });
        Platform::GetWindow().onMouseMove->trigger(glm::vec2{ 110.f, 106.f });
        frame();

        const auto position = spectator->getPosition();
        EXPECT_FLOAT_EQ(position.x * zoom, -10.f);
        EXPECT_FLOAT_EQ(position.y * zoom, 6.f);
        EXPECT_NEAR(position.z, 0.f, 0.00001f);
        frame();
        EXPECT_EQ(spectator->getPosition(), position);

        Platform::GetWindow().onMouseKeyPressed->trigger(
            Platform::Mouse::Key::Middle, Platform::Mouse::State::Release, Platform::Mouse::Mod{});
        frame();
    }
}
