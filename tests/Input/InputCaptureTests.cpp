// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxSubsystems/Input/InputAction.h"

#include "gtest/gtest.h"

namespace
{
    class InputCaptureTests : public ::testing::Test
    {
    protected:
        void SetUp() override { NX::InputCapture::resetButtons(); }
        void TearDown() override { NX::InputCapture::resetButtons(); }
    };

    class PolledInputAction : public NX::InputAction<Platform::Keyboard::Key>
    {
    public:
        PolledInputAction()
            : InputAction("Polled"_atom, Platform::Keyboard::Key::Delete)
        {
        }

        mutable int pollCount = 0;

    protected:
        [[nodiscard]] bool isKeyPressed() const override
        {
            ++pollCount;
            return false;
        }
    };
} // namespace

TEST_F(InputCaptureTests, LegacyActionsWaitUntilRecordedButtonsAreReleased)
{
    PolledInputAction action;
    int presses = 0;
    const auto subscription = action.onPress->subscribeAndGetID([&](auto) { ++presses; });
    {
        const NX::InputCapture capture;
        action.update();
        EXPECT_EQ(action.pollCount, 0);
        NX::InputCapture::updateButton(Platform::Keyboard::Key::Enter,
                                       Platform::Keyboard::KeyState::Pressed);
    }
    action.update();
    EXPECT_EQ(action.pollCount, 0);
    EXPECT_EQ(presses, 0);

    NX::InputCapture::updateButton(Platform::Keyboard::Key::Enter,
                                   Platform::Keyboard::KeyState::Released);
    action.update();
    EXPECT_EQ(action.pollCount, 1);
    EXPECT_EQ(presses, 0);
}

TEST_F(InputCaptureTests, LegacyMouseMotionAndClicksAreSuppressed)
{
    NX::MouseInputAction action("Mouse"_atom, Platform::Mouse::Key::None);
    int callbacks = 0;
    Core::DelegateSubscriberPoolGuard subscriptions;
    subscriptions << action.onMove->subscribeAndGetID([&](auto, auto) { ++callbacks; });
    subscriptions << action.onDrag->subscribeAndGetID([&](auto, auto) { ++callbacks; });
    subscriptions << action.onMouseClick->subscribeAndGetID([&](auto, auto) { ++callbacks; });
    const NX::InputCapture capture;
    action.update();
    EXPECT_EQ(callbacks, 0);
}

TEST_F(InputCaptureTests, NestedCapturesKeepActionsSuspendedUntilTheLastScopeEnds)
{
    EXPECT_FALSE(NX::InputCapture::isActive());
    {
        const NX::InputCapture outer;
        {
            const NX::InputCapture inner;
            NX::InputCapture::resetButtons();
            EXPECT_TRUE(NX::InputCapture::isActive());
        }
        EXPECT_TRUE(NX::InputCapture::isActive());
    }
    EXPECT_FALSE(NX::InputCapture::isActive());
}
