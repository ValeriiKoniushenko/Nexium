// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/Input/InputActionState.h"

#include "gtest/gtest.h"

namespace
{
    constexpr std::uint16_t SpaceKey = 32;
    constexpr std::uint16_t OtherKey = 65;

    NX::InputBinding Button(NX::InputTrigger trigger = NX::InputTrigger::Down)
    {
        NX::InputBinding binding;
        binding.source = NX::InputSource::Key;
        binding.code = SpaceKey;
        binding.trigger = trigger;
        return binding;
    }

    NX::InputAction ActionWith(NX::InputBinding binding)
    {
        return NX::InputAction{ .name = "Action"_atom, .bindings = { binding } };
    }
} // namespace

TEST(InputBindingTests, MatchesSourceAndCode)
{
    const auto binding = Button();
    EXPECT_TRUE(binding.matches(NX::InputSource::Key, SpaceKey));
    EXPECT_FALSE(binding.matches(NX::InputSource::Key, OtherKey));
    EXPECT_FALSE(binding.matches(NX::InputSource::MouseButton, SpaceKey));
}

TEST(InputBindingTests, PerFrameSourcesIgnoreCode)
{
    NX::InputBinding delta;
    delta.source = NX::InputSource::MouseDelta;
    delta.code = 7;
    EXPECT_TRUE(delta.matches(NX::InputSource::MouseDelta, 0));
    EXPECT_FALSE(delta.matches(NX::InputSource::MouseScroll, 0));
}

TEST(InputBindingTests, ModifierRules)
{
    auto plain = Button();
    EXPECT_TRUE(plain.acceptsModifiers(NX::InputModifier::None));
    EXPECT_TRUE(plain.acceptsModifiers(NX::InputModifier::Shift | NX::InputModifier::Control));

    auto save = Button();
    save.control = true;
    EXPECT_TRUE(save.acceptsModifiers(NX::InputModifier::Control));
    EXPECT_TRUE(save.acceptsModifiers(NX::InputModifier::Control | NX::InputModifier::CapsLock));
    EXPECT_FALSE(save.acceptsModifiers(NX::InputModifier::Control | NX::InputModifier::Shift));
    EXPECT_FALSE(save.acceptsModifiers(NX::InputModifier::None));
}

TEST(InputBindingTests, AppliesNegateAndSwizzle)
{
    auto binding = Button();
    EXPECT_EQ(binding.applyValueModifiers({ 1.f, 2.f }), glm::vec2(1.f, 2.f));

    binding.negate = true;
    EXPECT_EQ(binding.applyValueModifiers({ 1.f, 2.f }), glm::vec2(-1.f, -2.f));

    binding.swizzleYX = true;
    EXPECT_EQ(binding.applyValueModifiers({ 1.f, 0.f }), glm::vec2(0.f, -1.f));
}

TEST(BindingStateTests, DownBindingIsOnWhileHeld)
{
    const auto binding = Button();
    NX::BindingState state;

    state.onButton(binding, NX::InputState::Pressed, NX::InputModifier::None);
    EXPECT_TRUE(state.isHeld);
    EXPECT_EQ(state.contribution(binding), glm::vec2(1.f, 0.f));

    state.onButton(binding, NX::InputState::Released, NX::InputModifier::None);
    EXPECT_FALSE(state.isHeld);
    EXPECT_EQ(state.contribution(binding), glm::vec2(0.f));
}

TEST(BindingStateTests, PressWithWrongModifiersIsIgnored)
{
    auto binding = Button();
    binding.control = true;
    NX::BindingState state;

    state.onButton(binding, NX::InputState::Pressed, NX::InputModifier::Shift);
    EXPECT_FALSE(state.isHeld);

    state.onButton(binding, NX::InputState::Released, NX::InputModifier::None);
    EXPECT_FALSE(state.firedThisFrame);
}

TEST(BindingStateTests, BeginFrameClearsEdgesAndPerFrameValues)
{
    const auto button = Button(NX::InputTrigger::Pressed);
    NX::BindingState buttonState;
    buttonState.onButton(button, NX::InputState::Pressed, NX::InputModifier::None);
    ASSERT_TRUE(buttonState.firedThisFrame);
    buttonState.beginFrame(button);
    EXPECT_FALSE(buttonState.firedThisFrame);
    EXPECT_FALSE(buttonState.wasPressedThisFrame);
    EXPECT_TRUE(buttonState.isHeld);

    NX::InputBinding delta;
    delta.source = NX::InputSource::MouseDelta;
    NX::BindingState deltaState;
    deltaState.addVector({ 1.f, 2.f });
    deltaState.addVector({ 3.f, 4.f });
    EXPECT_EQ(deltaState.contribution(delta), glm::vec2(4.f, 6.f));
    deltaState.beginFrame(delta);
    EXPECT_EQ(deltaState.contribution(delta), glm::vec2(0.f));

    NX::InputBinding axis;
    axis.source = NX::InputSource::GamepadAxis;
    NX::BindingState axisState;
    axisState.setAxis(0.5f);
    axisState.beginFrame(axis);
    EXPECT_EQ(axisState.contribution(axis), glm::vec2(0.5f, 0.f));
}

TEST(ActionStateTests, QuickTapWithinOneFrameTriggers)
{
    const auto action = ActionWith(Button());
    NX::ActionState state(action);

    state.beginFrame(action);
    state.bindings[0].onButton(action.bindings[0], NX::InputState::Pressed,
                               NX::InputModifier::None);
    state.bindings[0].onButton(action.bindings[0], NX::InputState::Released,
                               NX::InputModifier::None);
    state.endFrame(action);

    EXPECT_TRUE(state.wasTriggered);
    EXPECT_FALSE(state.isActive);
}

TEST(ActionStateTests, ReleasedTriggerDoesNotFireOnPress)
{
    const auto action = ActionWith(Button(NX::InputTrigger::Released));
    NX::ActionState state(action);

    state.beginFrame(action);
    state.bindings[0].onButton(action.bindings[0], NX::InputState::Pressed,
                               NX::InputModifier::None);
    state.endFrame(action);
    EXPECT_FALSE(state.wasTriggered);

    state.beginFrame(action);
    state.bindings[0].onButton(action.bindings[0], NX::InputState::Released,
                               NX::InputModifier::None);
    state.endFrame(action);
    EXPECT_TRUE(state.wasTriggered);
}

TEST(ActionStateTests, FitsOnlyMatchingBindingCount)
{
    auto action = ActionWith(Button());
    const NX::ActionState state(action);
    EXPECT_TRUE(state.fits(action));

    action.bindings.push_back(Button());
    EXPECT_FALSE(state.fits(action));
}
