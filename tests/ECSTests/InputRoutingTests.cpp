// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/Input/InputController.h"
#include "NxWorld/Framework/InputSystem.h"
#include "Platform/Input/InputDevices/Joystick.h"
#include "Platform/Input/InputDevices/Keyboard.h"
#include "Platform/Input/InputDevices/Mouse.h"

#include "gtest/gtest.h"

namespace
{
    using Key = Platform::Keyboard::Key;
    using Axis = Platform::Joystick::GamepadAxis;

    class InputRoutingTests : public ::testing::Test
    {
    protected:
        void SetUp() override { NX::GetInputSystem().resetInput(); }

        void TearDown() override { NX::GetInputSystem().resetInput(); }

        /// A controller with one action named after `name` that counts its callbacks.
        struct Probe
        {
            NX::InputController::Ptr controller = NX::InputController::Create();
            int calls = 0;
        };

        static std::unique_ptr<Probe> makeProbe(const char* name,
                                                std::vector<NX::InputBinding> bindings,
                                                int priority = 0)
        {
            auto probe = std::make_unique<Probe>();
            probe->controller->setPriority(priority);
            EXPECT_TRUE(probe->controller->addAction(Core::StringAtom(name), std::move(bindings),
                                                     [p = probe.get()] { ++p->calls; }));
            probe->controller->initialize();
            return probe;
        }

        static void key(Key key, NX::InputState state,
                        NX::InputModifier mods = NX::InputModifier::None)
        {
            NX::KeyboardEvent event;
            event.setKey(NX::InputButton(static_cast<std::uint16_t>(key)));
            event.setState(state);
            event.setMods(mods);
            NX::GetInputSystem().queueEvent(event);
        }

        static void mouseMove(glm::vec2 delta)
        {
            NX::MouseMoveEvent event;
            event.setDelta(delta);
            NX::GetInputSystem().queueEvent(event);
        }

        static void scroll(glm::vec2 offset)
        {
            NX::MouseScrollEvent event;
            event.setOffset(offset);
            NX::GetInputSystem().queueEvent(event);
        }

        static void axis(Axis axis, float value)
        {
            NX::GamepadAxisEvent event(1);
            event.setAxis(static_cast<std::uint16_t>(axis));
            event.setValue(value);
            NX::GetInputSystem().queueEvent(event);
        }

        static void focus(bool focused)
        {
            NX::WindowFocusEvent event;
            event.setFocused(focused);
            NX::GetInputSystem().queueEvent(event);
        }

        static void iconify(bool iconified)
        {
            NX::WindowIconifyEvent event;
            event.setIconified(iconified);
            NX::GetInputSystem().queueEvent(event);
        }

        static void frame() { NX::GetInputSystem().processEvents(); }
    };
} // namespace

// ---- Priority and consumption ----

TEST_F(InputRoutingTests, DefaultsAreNeutral)
{
    const auto controller = NX::InputController::Create();
    EXPECT_EQ(controller->getPriority(), 0);
    EXPECT_TRUE(controller->doesConsumeInput());
}

TEST_F(InputRoutingTests, HigherPriorityConsumesAMatchedPress)
{
    auto high = makeProbe("High", { NX::InputBinding::Shortcut(Key::Escape) }, 10);
    auto low = makeProbe("Low", { NX::InputBinding::Shortcut(Key::Escape) }, 0);

    key(Key::Escape, NX::InputState::Pressed);
    frame();

    EXPECT_EQ(high->calls, 1);
    EXPECT_EQ(low->calls, 0);
}

TEST_F(InputRoutingTests, RegistrationOrderDoesNotDecideThePriority)
{
    auto low = makeProbe("Low", { NX::InputBinding::Shortcut(Key::Escape) }, 0);
    auto high = makeProbe("High", { NX::InputBinding::Shortcut(Key::Escape) }, 10);

    key(Key::Escape, NX::InputState::Pressed);
    frame();

    EXPECT_EQ(high->calls, 1);
    EXPECT_EQ(low->calls, 0);
}

TEST_F(InputRoutingTests, EqualPriorityControllersAllReceiveTheEvent)
{
    auto first = makeProbe("First", { NX::InputBinding::Shortcut(Key::Escape) });
    auto second = makeProbe("Second", { NX::InputBinding::Shortcut(Key::Escape) });

    key(Key::Escape, NX::InputState::Pressed);
    frame();

    EXPECT_EQ(first->calls, 1);
    EXPECT_EQ(second->calls, 1);
}

TEST_F(InputRoutingTests, AnEventTheHigherControllerDoesNotBindPassesThrough)
{
    auto high = makeProbe("High", { NX::InputBinding::Shortcut(Key::Space) }, 10);
    auto low = makeProbe("Low", { NX::InputBinding::Shortcut(Key::Escape) }, 0);

    key(Key::Escape, NX::InputState::Pressed);
    frame();

    EXPECT_EQ(high->calls, 0);
    EXPECT_EQ(low->calls, 1);
}

TEST_F(InputRoutingTests, OnlyAcceptedModifiersConsume)
{
    auto high
        = makeProbe("Save", { NX::InputBinding::Shortcut(Key::S, NX::InputModifier::Control) }, 10);
    auto low = makeProbe("Plain", { NX::InputBinding::Shortcut(Key::S) }, 0);

    // Plain S is not Ctrl+S, so the higher controller does not match and does not consume.
    key(Key::S, NX::InputState::Pressed);
    frame();
    EXPECT_EQ(high->calls, 0);
    EXPECT_EQ(low->calls, 1);

    key(Key::S, NX::InputState::Released);
    key(Key::S, NX::InputState::Pressed, NX::InputModifier::Control);
    frame();
    EXPECT_EQ(high->calls, 1);
    EXPECT_EQ(low->calls, 1);
}

TEST_F(InputRoutingTests, ConsumptionIsPerEventNotPerFrame)
{
    auto high = makeProbe("High", { NX::InputBinding::Shortcut(Key::Escape) }, 10);
    auto low = makeProbe("Low", { NX::InputBinding::Shortcut(Key::Space) }, 0);

    key(Key::Escape, NX::InputState::Pressed);
    key(Key::Space, NX::InputState::Pressed);
    frame();

    EXPECT_EQ(high->calls, 1);
    EXPECT_EQ(low->calls, 1);
}

TEST_F(InputRoutingTests, ADisabledControllerNeverConsumes)
{
    auto high = makeProbe("High", { NX::InputBinding::Shortcut(Key::Escape) }, 10);
    auto low = makeProbe("Low", { NX::InputBinding::Shortcut(Key::Escape) }, 0);
    high->controller->setEnabled(false);

    key(Key::Escape, NX::InputState::Pressed);
    frame();

    EXPECT_EQ(high->calls, 0);
    EXPECT_EQ(low->calls, 1);
}

TEST_F(InputRoutingTests, AControllerThatDoesNotConsumeOnlyObserves)
{
    auto high = makeProbe("High", { NX::InputBinding::Shortcut(Key::Escape) }, 10);
    auto low = makeProbe("Low", { NX::InputBinding::Shortcut(Key::Escape) }, 0);
    high->controller->setConsumesInput(false);

    key(Key::Escape, NX::InputState::Pressed);
    frame();

    EXPECT_EQ(high->calls, 1);
    EXPECT_EQ(low->calls, 1);
}

TEST_F(InputRoutingTests, ChangingPriorityTakesEffectOnTheNextFrame)
{
    auto a = makeProbe("A", { NX::InputBinding::Shortcut(Key::Escape) }, 10);
    auto b = makeProbe("B", { NX::InputBinding::Shortcut(Key::Escape) }, 0);

    key(Key::Escape, NX::InputState::Pressed);
    frame();
    EXPECT_EQ(a->calls, 1);
    EXPECT_EQ(b->calls, 0);

    a->controller->setPriority(-10);
    key(Key::Escape, NX::InputState::Released);
    key(Key::Escape, NX::InputState::Pressed);
    frame();
    EXPECT_EQ(a->calls, 1);
    EXPECT_EQ(b->calls, 1);
}

TEST_F(InputRoutingTests, ReleasesAlwaysReachEveryControllerSoNothingGetsStuck)
{
    auto high = makeProbe("High", { NX::InputBinding::Key(Key::W) }, 10);
    auto low = makeProbe("Low", { NX::InputBinding::Key(Key::W) }, 0);

    // The lower controller is already holding W while the higher one cannot see it.
    high->controller->setEnabled(false);
    key(Key::W, NX::InputState::Pressed);
    frame();
    ASSERT_TRUE(low->controller->isActive("Low"_atom));

    high->controller->setEnabled(true);
    key(Key::W, NX::InputState::Released);
    frame();

    EXPECT_FALSE(low->controller->isActive("Low"_atom));
}

TEST_F(InputRoutingTests, AConsumedPressNeverReachesTheLowerController)
{
    auto high = makeProbe("High", { NX::InputBinding::Key(Key::W) }, 10);
    auto low = makeProbe("Low", { NX::InputBinding::Key(Key::W) }, 0);

    key(Key::W, NX::InputState::Pressed);
    frame();

    EXPECT_TRUE(high->controller->isActive("High"_atom));
    EXPECT_FALSE(low->controller->isActive("Low"_atom));
}

TEST_F(InputRoutingTests, MouseDeltaAndScrollAreConsumedByABindingOnTheSameSource)
{
    auto high = makeProbe("Look", { NX::InputBinding::MouseDelta() }, 10);
    auto low = makeProbe("Pan", { NX::InputBinding::MouseDelta() }, 0);
    auto wheel = makeProbe("Wheel", { NX::InputBinding::MouseScroll() }, 0);

    mouseMove({ 3.f, 0.f });
    scroll({ 0.f, 1.f });
    frame();

    EXPECT_EQ(high->controller->getValue<glm::vec2>("Look"_atom), glm::vec2(3.f, 0.f));
    EXPECT_EQ(low->controller->getValue<glm::vec2>("Pan"_atom), glm::vec2(0.f));
    // The higher controller binds no scroll, so scroll still reaches the lower one.
    EXPECT_EQ(wheel->controller->getValue<glm::vec2>("Wheel"_atom), glm::vec2(0.f, 1.f));
}

TEST_F(InputRoutingTests, GamepadAxisIsConsumedByABindingOnTheSameAxis)
{
    auto high = makeProbe("High", { NX::InputBinding::GamepadAxis(Axis::LeftX) }, 10);
    auto low = makeProbe("Low", { NX::InputBinding::GamepadAxis(Axis::LeftX) }, 0);
    auto other = makeProbe("Other", { NX::InputBinding::GamepadAxis(Axis::RightX) }, 0);

    axis(Axis::LeftX, 0.5f);
    axis(Axis::RightX, 0.5f);
    frame();

    EXPECT_FLOAT_EQ(high->controller->getValue<float>("High"_atom), 0.5f);
    EXPECT_FLOAT_EQ(low->controller->getValue<float>("Low"_atom), 0.f);
    EXPECT_FLOAT_EQ(other->controller->getValue<float>("Other"_atom), 0.5f);
}

TEST_F(InputRoutingTests, PriorityAndConsumeFlagSurviveSerialization)
{
    auto original = NX::InputController::Create();
    original->setPriority(42);
    original->setConsumesInput(false);

    RResourceStream<RJsonResourceStream> stream(original->serialize());
    auto restored = NX::InputController::Create();
    restored->deserialize(stream);

    EXPECT_EQ(restored->getPriority(), 42);
    EXPECT_FALSE(restored->doesConsumeInput());
}

// ---- The window is not active: unfocused or minimized ----

TEST_F(InputRoutingTests, PressesWhileTheWindowIsUnfocusedAreIgnored)
{
    auto probe = makeProbe("Act", { NX::InputBinding::Key(Key::W) });

    focus(false);
    key(Key::W, NX::InputState::Pressed);
    frame();

    EXPECT_FALSE(probe->controller->isActive("Act"_atom));
    EXPECT_EQ(probe->calls, 0);
}

TEST_F(InputRoutingTests, InputWorksAgainWhenFocusReturns)
{
    auto probe = makeProbe("Act", { NX::InputBinding::Key(Key::W) });

    focus(false);
    key(Key::W, NX::InputState::Pressed);
    frame();
    focus(true);
    key(Key::W, NX::InputState::Pressed);
    frame();

    EXPECT_TRUE(probe->controller->isActive("Act"_atom));
}

TEST_F(InputRoutingTests, LosingFocusReleasesEverythingThatWasHeld)
{
    auto probe = makeProbe("Act", { NX::InputBinding::Key(Key::W) });
    key(Key::W, NX::InputState::Pressed);
    frame();
    ASSERT_TRUE(probe->controller->isActive("Act"_atom));

    focus(false);
    frame();

    EXPECT_FALSE(probe->controller->isActive("Act"_atom));
}

TEST_F(InputRoutingTests, MinimizingIgnoresInputAndReleasesHeldInput)
{
    auto probe = makeProbe("Act", { NX::InputBinding::Key(Key::W) });
    key(Key::W, NX::InputState::Pressed);
    frame();
    ASSERT_TRUE(probe->controller->isActive("Act"_atom));

    iconify(true);
    frame();
    EXPECT_FALSE(probe->controller->isActive("Act"_atom));

    key(Key::W, NX::InputState::Pressed);
    frame();
    EXPECT_FALSE(probe->controller->isActive("Act"_atom));

    iconify(false);
    key(Key::W, NX::InputState::Pressed);
    frame();
    EXPECT_TRUE(probe->controller->isActive("Act"_atom));
}

TEST_F(InputRoutingTests, FocusDoesNotReturnWhileStillMinimized)
{
    auto probe = makeProbe("Act", { NX::InputBinding::Key(Key::W) });

    iconify(true);
    focus(true);
    key(Key::W, NX::InputState::Pressed);
    frame();

    EXPECT_FALSE(probe->controller->isActive("Act"_atom));
}

TEST_F(InputRoutingTests, MouseMovementScrollAndAxesAreIgnoredWhileInactive)
{
    auto look = makeProbe("Look", { NX::InputBinding::MouseDelta() });
    auto wheel = makeProbe("Wheel", { NX::InputBinding::MouseScroll() });
    auto stick = makeProbe("Stick", { NX::InputBinding::GamepadAxis(Axis::LeftX) });

    focus(false);
    mouseMove({ 5.f, 5.f });
    scroll({ 0.f, 1.f });
    axis(Axis::LeftX, 0.9f);
    frame();

    EXPECT_FALSE(look->controller->isActive("Look"_atom));
    EXPECT_FALSE(wheel->controller->isActive("Wheel"_atom));
    EXPECT_FALSE(stick->controller->isActive("Stick"_atom));
}

TEST_F(InputRoutingTests, AStickHeldWhenFocusIsLostDoesNotKeepMoving)
{
    auto stick = makeProbe("Stick", { NX::InputBinding::GamepadAxis(Axis::LeftX) });
    axis(Axis::LeftX, 0.9f);
    frame();
    ASSERT_TRUE(stick->controller->isActive("Stick"_atom));

    focus(false);
    frame();

    EXPECT_FALSE(stick->controller->isActive("Stick"_atom));
}

TEST_F(InputRoutingTests, ReleasesStillReachControllersWhileInactive)
{
    auto probe = makeProbe("Act", { NX::InputBinding::Key(Key::W).onReleased() });
    key(Key::W, NX::InputState::Pressed);
    frame();

    // GLFW delivers the synthesized release after the focus-lost callback.
    focus(false);
    key(Key::W, NX::InputState::Released);
    frame();

    EXPECT_FALSE(probe->controller->isActive("Act"_atom));
}

TEST_F(InputRoutingTests, EventsQueuedBeforeFocusWasLostAreStillDelivered)
{
    auto probe = makeProbe("Act", { NX::InputBinding::Shortcut(Key::Escape) });

    key(Key::Escape, NX::InputState::Pressed);
    focus(false);
    frame();

    EXPECT_EQ(probe->calls, 1);
}

TEST_F(InputRoutingTests, ResetInputResyncsWithTheWindow)
{
    auto probe = makeProbe("Act", { NX::InputBinding::Key(Key::W) });
    focus(false);
    frame();

    // The test window is never created, so it counts as active again after a reset.
    NX::GetInputSystem().resetInput();
    key(Key::W, NX::InputState::Pressed);
    frame();

    EXPECT_TRUE(probe->controller->isActive("Act"_atom));
}
