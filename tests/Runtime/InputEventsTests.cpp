// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/InputSystem.h"
#include "Platform/Input/CursorDeltaTracker.h"
#include "Platform/Input/Glfw.h"
#include "Platform/Input/InputDevices/Joystick.h"
#include "Platform/Input/Window.h"

#include "gtest/gtest.h"
#include <type_traits>
#include <variant>

namespace
{
    class InputSystemQueueTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            NX::GetInputSystem().initialize(Platform::GetWindow());
            NX::GetInputSystem().resetInput();
        }

        void TearDown() override { NX::GetInputSystem().resetInput(); }
    };
} // namespace

TEST(InputEventsTests, InputStateNoneIsDistinctFromReleased)
{
    static_assert(NX::InputState::None != NX::InputState::Released);
    static_assert(NX::InputState::None != NX::InputState::Pressed);
    static_assert(NX::InputState::None != NX::InputState::Repeated);
    EXPECT_NE(NX::InputState{}, NX::InputState::Released);
}

TEST(InputEventsTests, ConvertsGlfwActionsToInputState)
{
    static_assert(Platform::ToInputState(GLFW_PRESS) == NX::InputState::Pressed);
    EXPECT_EQ(Platform::ToInputState(GLFW_PRESS), NX::InputState::Pressed);
    EXPECT_EQ(Platform::ToInputState(GLFW_RELEASE), NX::InputState::Released);
    EXPECT_EQ(Platform::ToInputState(GLFW_REPEAT), NX::InputState::Repeated);
    EXPECT_EQ(Platform::ToInputState(-1), NX::InputState::None);
}

TEST_F(InputSystemQueueTests, FocusLossKeepsQueuedEvents)
{
    auto& input = NX::GetInputSystem();

    NX::KeyboardEvent press;
    press.setKey(NX::InputButton(static_cast<std::uint16_t>(Platform::Keyboard::Key::W)));
    press.setState(NX::InputState::Pressed);
    Platform::GetWindow().onKeyPressed->trigger(press);

    NX::WindowFocusEvent focusLost;
    focusLost.setFocused(false);
    Platform::GetWindow().onFocusChanged->trigger(focusLost);

    // GLFW synthesizes the release after the focus callback; it must land after the press.
    NX::KeyboardEvent release = press;
    release.setState(NX::InputState::Released);
    Platform::GetWindow().onKeyPressed->trigger(release);

    EXPECT_EQ(input.getPendingEventCount(), 3U);

    input.processEvents();
    EXPECT_EQ(input.getPendingEventCount(), 0U);
}

TEST(InputEventsTests, EventsAreNonPolymorphicValueTypes)
{
    static_assert(!std::is_polymorphic_v<NX::InputEvent>);
    static_assert(!std::is_destructible_v<NX::InputEvent>, "base must not be destroyed directly");
    static_assert(std::is_trivially_copyable_v<NX::KeyboardEvent>);
    static_assert(std::is_trivially_copyable_v<NX::MouseMoveEvent>);
    static_assert(NX::InputEventType<NX::GamepadAxisEvent>);
    static_assert(NX::InputEventType<const NX::KeyboardEvent&>);
    SUCCEED();
}

TEST_F(InputSystemQueueTests, QueuedEventsKeepArrivalOrder)
{
    auto& window = Platform::GetWindow();

    NX::KeyboardEvent key;
    key.setState(NX::InputState::Pressed);
    window.onKeyPressed->trigger(key);

    NX::MouseMoveEvent move;
    move.setDelta({ 1.f, 2.f });
    window.onMouseMove->trigger(move);

    NX::GamepadButtonEvent gamepadButton(1);
    gamepadButton.setState(NX::InputState::Released);
    window.onJoystickButtonPressed->trigger(gamepadButton);

    const auto& events = NX::GetInputSystem().getPendingEvents();
    ASSERT_EQ(events.size(), 3U);
    EXPECT_TRUE(std::holds_alternative<NX::KeyboardEvent>(events[0]));
    EXPECT_TRUE(std::holds_alternative<NX::MouseMoveEvent>(events[1]));
    ASSERT_TRUE(std::holds_alternative<NX::GamepadButtonEvent>(events[2]));
    EXPECT_EQ(std::get<NX::GamepadButtonEvent>(events[2]).getDeviceId(), 1U);
    EXPECT_EQ(std::get<NX::MouseMoveEvent>(events[1]).getDelta(), glm::vec2(1.f, 2.f));
}

TEST_F(InputSystemQueueTests, QueueEventStoresACopy)
{
    auto& input = NX::GetInputSystem();

    NX::MouseScrollEvent scroll;
    scroll.setOffset({ 0.f, 1.f });
    input.queueEvent(scroll);
    scroll.setOffset({ 0.f, 5.f });

    const auto& events = input.getPendingEvents();
    ASSERT_EQ(events.size(), 1U);
    ASSERT_TRUE(std::holds_alternative<NX::MouseScrollEvent>(events[0]));
    EXPECT_EQ(std::get<NX::MouseScrollEvent>(events[0]).getOffset(), glm::vec2(0.f, 1.f));

    input.processEvents();
    EXPECT_TRUE(input.getPendingEvents().empty());
}

TEST_F(InputSystemQueueTests, MouseButtonEventKeepsClickPosition)
{
    NX::MouseButtonEvent click;
    click.setState(NX::InputState::Pressed);
    click.setPosition({ 10.f, 20.f });
    Platform::GetWindow().onMouseKeyPressed->trigger(click);

    NX::MouseMoveEvent move;
    move.setPosition({ 25.f, 20.f });
    Platform::GetWindow().onMouseMove->trigger(move);

    const auto& events = NX::GetInputSystem().getPendingEvents();
    ASSERT_EQ(events.size(), 2U);
    ASSERT_TRUE(std::holds_alternative<NX::MouseButtonEvent>(events[0]));
    EXPECT_EQ(std::get<NX::MouseButtonEvent>(events[0]).getPosition(), glm::vec2(10.f, 20.f));
}

TEST(InputEventsTests, ConvertsGlfwModifiersToInputModifiers)
{
    static_assert(Platform::ToInputModifiers(0) == NX::InputModifier::None);
    EXPECT_EQ(Platform::ToInputModifiers(GLFW_MOD_SHIFT), NX::InputModifier::Shift);
    EXPECT_EQ(Platform::ToInputModifiers(GLFW_MOD_CONTROL | GLFW_MOD_ALT),
              NX::InputModifier::Control | NX::InputModifier::Alt);
    EXPECT_EQ(Platform::ToInputModifiers(GLFW_MOD_SUPER | GLFW_MOD_CAPS_LOCK | GLFW_MOD_NUM_LOCK),
              NX::InputModifier::Super | NX::InputModifier::CapsLock | NX::InputModifier::NumLock);
    EXPECT_EQ(Platform::ToInputModifiers(GLFW_MOD_SHIFT | GLFW_MOD_CONTROL | GLFW_MOD_ALT
                                         | GLFW_MOD_SUPER | GLFW_MOD_CAPS_LOCK | GLFW_MOD_NUM_LOCK),
              NX::InputModifier::All);
}

TEST(CursorDeltaTrackerTests, FirstPositionHasNoDelta)
{
    Platform::CursorDeltaTracker tracker;
    EXPECT_EQ(tracker.update({ 100.f, 50.f }), glm::vec2(0.f));
    EXPECT_EQ(tracker.update({ 103.f, 45.f }), glm::vec2(3.f, -5.f));
}

TEST(CursorDeltaTrackerTests, ResetDropsTheJumpAfterCursorModeChange)
{
    Platform::CursorDeltaTracker tracker;
    (void)tracker.update({ 10.f, 10.f });
    tracker.reset();
    EXPECT_EQ(tracker.update({ 900.f, 600.f }), glm::vec2(0.f));
    EXPECT_EQ(tracker.update({ 901.f, 600.f }), glm::vec2(1.f, 0.f));
}

TEST(CursorDeltaTrackerTests, ResetToIgnoresProgrammaticWarp)
{
    Platform::CursorDeltaTracker tracker;
    (void)tracker.update({ 10.f, 10.f });
    tracker.resetTo({ 400.f, 300.f });
    EXPECT_EQ(tracker.update({ 400.f, 300.f }), glm::vec2(0.f));
    EXPECT_EQ(tracker.update({ 402.f, 299.f }), glm::vec2(2.f, -1.f));
}

TEST(JoystickTests, TriggersRestAtMinusOneAndSticksAtZero)
{
    EXPECT_EQ(Platform::Joystick::getAxisRestValue(GLFW_GAMEPAD_AXIS_LEFT_X), 0.f);
    EXPECT_EQ(Platform::Joystick::getAxisRestValue(GLFW_GAMEPAD_AXIS_RIGHT_Y), 0.f);
    EXPECT_EQ(Platform::Joystick::getAxisRestValue(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER), -1.f);
    EXPECT_EQ(Platform::Joystick::getAxisRestValue(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER), -1.f);
}
