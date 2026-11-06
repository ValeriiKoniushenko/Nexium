// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/InputSystem.h"
#include "Platform/Input/Glfw.h"
#include "Platform/Input/Window.h"

#include "gtest/gtest.h"

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
