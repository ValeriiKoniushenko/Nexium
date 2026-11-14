// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/Input/InputActionTypes.h"

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
        return NX::InputAction{ "Action"_atom, { binding } };
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

TEST(InputBindingTests, AtLeastAllowsExtraModifiers)
{
    using Mod = NX::InputModifier;
    auto plain = Button();
    EXPECT_EQ(plain.modifierMatch, NX::ModifierMatch::AtLeast);
    EXPECT_TRUE(plain.acceptsModifiers(Mod::None));
    EXPECT_TRUE(plain.acceptsModifiers(Mod::Shift | Mod::Control));

    auto sprintJump = Button();
    sprintJump.shift = true;
    EXPECT_TRUE(sprintJump.acceptsModifiers(Mod::Shift));
    EXPECT_TRUE(sprintJump.acceptsModifiers(Mod::Shift | Mod::Control));
    EXPECT_FALSE(sprintJump.acceptsModifiers(Mod::Control));
    EXPECT_FALSE(sprintJump.acceptsModifiers(Mod::None));
}

TEST(InputBindingTests, ExactRejectsExtraModifiersIgnoringLocks)
{
    using Mod = NX::InputModifier;
    auto del = Button();
    del.modifierMatch = NX::ModifierMatch::Exact;
    EXPECT_TRUE(del.acceptsModifiers(Mod::None));
    EXPECT_TRUE(del.acceptsModifiers(Mod::CapsLock | Mod::NumLock));
    EXPECT_FALSE(del.acceptsModifiers(Mod::Shift));

    auto save = Button();
    save.control = true;
    save.modifierMatch = NX::ModifierMatch::Exact;
    EXPECT_TRUE(save.acceptsModifiers(Mod::Control));
    EXPECT_TRUE(save.acceptsModifiers(Mod::Control | Mod::CapsLock));
    EXPECT_FALSE(save.acceptsModifiers(Mod::Control | Mod::Shift));
    EXPECT_FALSE(save.acceptsModifiers(Mod::None));
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

TEST(InputBindingRuntimeTests, DownBindingIsOnWhileHeld)
{
    auto binding = Button();

    binding.handleButton(NX::InputState::Pressed, NX::InputModifier::None);
    EXPECT_TRUE(binding.runtime.isHeld);
    EXPECT_EQ(binding.getContribution(), glm::vec2(1.f, 0.f));

    binding.handleButton(NX::InputState::Released, NX::InputModifier::None);
    EXPECT_FALSE(binding.runtime.isHeld);
    EXPECT_EQ(binding.getContribution(), glm::vec2(0.f));
}

TEST(InputBindingRuntimeTests, PressWithWrongModifiersIsIgnored)
{
    auto binding = Button();
    binding.control = true;

    binding.handleButton(NX::InputState::Pressed, NX::InputModifier::Shift);
    EXPECT_FALSE(binding.runtime.isHeld);

    binding.handleButton(NX::InputState::Released, NX::InputModifier::None);
    EXPECT_FALSE(binding.runtime.firedThisFrame);
}

TEST(InputBindingRuntimeTests, BeginFrameClearsEdgesAndPerFrameValues)
{
    auto button = Button(NX::InputTrigger::Pressed);
    button.handleButton(NX::InputState::Pressed, NX::InputModifier::None);
    ASSERT_TRUE(button.runtime.firedThisFrame);
    button.beginFrame();
    EXPECT_FALSE(button.runtime.firedThisFrame);
    EXPECT_FALSE(button.runtime.wasPressedThisFrame);
    EXPECT_TRUE(button.runtime.isHeld);

    auto delta = NX::InputBinding::MouseDelta();
    delta.handleVector({ 1.f, 2.f });
    delta.handleVector({ 3.f, 4.f });
    EXPECT_EQ(delta.getContribution(), glm::vec2(4.f, 6.f));
    delta.beginFrame();
    EXPECT_EQ(delta.getContribution(), glm::vec2(0.f));

    auto axis = NX::InputBinding::GamepadAxis(Platform::Joystick::GamepadAxis::LeftX);
    axis.handleAxis(0.5f);
    axis.beginFrame();
    EXPECT_EQ(axis.getContribution(), glm::vec2(0.5f, 0.f));
}

TEST(InputActionRuntimeTests, QuickTapWithinOneFrameTriggers)
{
    auto action = ActionWith(Button());

    action.beginFrame();
    action.bindings[0].handleButton(NX::InputState::Pressed, NX::InputModifier::None);
    action.bindings[0].handleButton(NX::InputState::Released, NX::InputModifier::None);
    action.endFrame();

    EXPECT_TRUE(action.runtime.wasTriggered);
    EXPECT_FALSE(action.runtime.isActive);
}

TEST(InputActionRuntimeTests, ReleasedTriggerDoesNotFireOnPress)
{
    auto action = ActionWith(Button(NX::InputTrigger::Released));

    action.beginFrame();
    action.bindings[0].handleButton(NX::InputState::Pressed, NX::InputModifier::None);
    action.endFrame();
    EXPECT_FALSE(action.runtime.wasTriggered);

    action.beginFrame();
    action.bindings[0].handleButton(NX::InputState::Released, NX::InputModifier::None);
    action.endFrame();
    EXPECT_TRUE(action.runtime.wasTriggered);
}

TEST(InputActionRuntimeTests, ResetRuntimeForgetsHeldButtons)
{
    auto action = ActionWith(Button());
    action.bindings[0].handleButton(NX::InputState::Pressed, NX::InputModifier::None);
    action.endFrame();
    ASSERT_TRUE(action.runtime.isActive);

    action.resetRuntime();
    EXPECT_FALSE(action.runtime.isActive);
    EXPECT_FALSE(action.bindings[0].runtime.isHeld);
}

TEST(InputActionRuntimeTests, RuntimeIsNotSerialized)
{
    auto action = ActionWith(Button());
    action.bindings[0].handleButton(NX::InputState::Pressed, NX::InputModifier::None);
    action.endFrame();

    const nlohmann::json json = action;
    const auto restored = json.get<NX::InputAction>();

    EXPECT_EQ(json.dump().find("runtime"), std::string::npos);
    EXPECT_FALSE(restored.runtime.isActive);
    EXPECT_FALSE(restored.bindings[0].runtime.isHeld);
}

TEST(InputBindingTests, FactoriesSetSourceAndCode)
{
    const auto key = NX::InputBinding::Key(Platform::Keyboard::Key::S);
    EXPECT_EQ(key.source, NX::InputSource::Key);
    EXPECT_EQ(key.code, static_cast<std::uint16_t>(Platform::Keyboard::Key::S));
    EXPECT_EQ(key.trigger, NX::InputTrigger::Down);

    const auto button = NX::InputBinding::MouseButton(Platform::Mouse::Key::Middle);
    EXPECT_EQ(button.source, NX::InputSource::MouseButton);
    EXPECT_EQ(button.code, static_cast<std::uint16_t>(Platform::Mouse::Key::Middle));

    EXPECT_EQ(NX::InputBinding::MouseDelta().source, NX::InputSource::MouseDelta);
    EXPECT_EQ(NX::InputBinding::MouseScroll().source, NX::InputSource::MouseScroll);

    const auto pad = NX::InputBinding::GamepadButton(Platform::Joystick::GamepadButton::A);
    EXPECT_EQ(pad.source, NX::InputSource::GamepadButton);
    EXPECT_EQ(pad.code, static_cast<std::uint16_t>(Platform::Joystick::GamepadButton::A));

    const auto axis = NX::InputBinding::GamepadAxis(Platform::Joystick::GamepadAxis::LeftY);
    EXPECT_EQ(axis.source, NX::InputSource::GamepadAxis);
    EXPECT_EQ(axis.code, static_cast<std::uint16_t>(Platform::Joystick::GamepadAxis::LeftY));
}

TEST(InputBindingTests, ChainedModifiersReturnConfiguredCopy)
{
    const auto base = NX::InputBinding::Key(Platform::Keyboard::Key::S);
    const auto save = base.withControl().withShift().onPressed();

    EXPECT_EQ(save.getModifiers(), NX::InputModifier::Control | NX::InputModifier::Shift);
    EXPECT_EQ(save.trigger, NX::InputTrigger::Pressed);
    EXPECT_EQ(base.getModifiers(), NX::InputModifier::None);
    EXPECT_EQ(base.trigger, NX::InputTrigger::Down);

    const auto other = base.withAlt().withSuper().onReleased().negated().swizzled();
    EXPECT_EQ(other.getModifiers(), NX::InputModifier::Alt | NX::InputModifier::Super);
    EXPECT_EQ(other.trigger, NX::InputTrigger::Released);
    EXPECT_TRUE(other.negate);
    EXPECT_TRUE(other.swizzleYX);
}

TEST(InputBindingTests, OnDownOverridesShortcutTriggerAndKeepsExactModifiers)
{
    auto binding
        = NX::InputBinding::Shortcut(Platform::Keyboard::Key::S, NX::InputModifier::Control);
    ASSERT_EQ(binding.trigger, NX::InputTrigger::Pressed);

    binding = binding.onDown();
    EXPECT_EQ(binding.trigger, NX::InputTrigger::Down);
    EXPECT_EQ(binding.modifierMatch, NX::ModifierMatch::Exact);

    binding.handleButton(NX::InputState::Pressed, NX::InputModifier::Control);
    binding.beginFrame();
    EXPECT_EQ(binding.getContribution(), glm::vec2(1.f, 0.f));

    binding.handleButton(NX::InputState::Released, NX::InputModifier::Control);
    EXPECT_EQ(binding.getContribution(), glm::vec2(0.f));
}

TEST(InputBindingTests, ShortcutIsExactAndFiresOnPress)
{
    using Mod = NX::InputModifier;
    const auto save
        = NX::InputBinding::Shortcut(Platform::Keyboard::Key::S, Mod::Control | Mod::Shift);
    EXPECT_EQ(save.source, NX::InputSource::Key);
    EXPECT_EQ(save.code, static_cast<std::uint16_t>(Platform::Keyboard::Key::S));
    EXPECT_EQ(save.trigger, NX::InputTrigger::Pressed);
    EXPECT_EQ(save.modifierMatch, NX::ModifierMatch::Exact);
    EXPECT_EQ(save.getModifiers(), Mod::Control | Mod::Shift);

    const auto del = NX::InputBinding::Shortcut(Platform::Keyboard::Key::Delete);
    EXPECT_EQ(del.getModifiers(), Mod::None);
    EXPECT_TRUE(del.acceptsModifiers(Mod::None));
    EXPECT_FALSE(del.acceptsModifiers(Mod::Shift));
}

TEST(InputBindingTests, WithExactModifiersSwitchesMatch)
{
    const auto key = NX::InputBinding::Key(Platform::Keyboard::Key::Delete);
    EXPECT_EQ(key.modifierMatch, NX::ModifierMatch::AtLeast);
    EXPECT_EQ(key.withExactModifiers().modifierMatch, NX::ModifierMatch::Exact);
}

TEST(InputBindingTests, ModifierMatchSurvivesSerialization)
{
    const auto del = NX::InputBinding::Shortcut(Platform::Keyboard::Key::Delete);
    const nlohmann::json json = del;
    EXPECT_EQ(json.get<NX::InputBinding>().modifierMatch, NX::ModifierMatch::Exact);
}

TEST(InputBindingTests, ToStringDescribesBinding)
{
    using Key = Platform::Keyboard::Key;
    using NX::InputBinding;

    EXPECT_EQ(InputBinding::Key(Key::F1).toString(), "F1");
    EXPECT_EQ(InputBinding::Key(Key::S).withShift().withControl().toString(), "Ctrl+Shift+S");
    EXPECT_EQ(InputBinding::Key(Key::Q).withAlt().withSuper().toString(), "Alt+Super+Q");
    EXPECT_EQ(InputBinding::MouseButton(Platform::Mouse::Key::Left).toString(), "Mouse Left");
    EXPECT_EQ(InputBinding::MouseButton(Platform::Mouse::Key::Middle).toString(), "Mouse Middle");
    EXPECT_EQ(InputBinding::MouseButton(Platform::Mouse::Key::_5).toString(), "Mouse 5");
    EXPECT_EQ(InputBinding::MouseDelta().toString(), "Mouse Move");
    EXPECT_EQ(InputBinding::MouseScroll().toString(), "Mouse Wheel");
    EXPECT_EQ(InputBinding::GamepadButton(Platform::Joystick::GamepadButton::LeftBumper).toString(),
              "Gamepad LeftBumper");
    EXPECT_EQ(InputBinding::GamepadAxis(Platform::Joystick::GamepadAxis::RightTrigger).toString(),
              "Gamepad RightTrigger");
    EXPECT_EQ(InputBinding::Key(Key::Space).onReleased().toString(), "Space (release)");
}

TEST(InputBindingTests, AxisDeadzoneZeroesSmallValuesOnly)
{
    auto axis
        = NX::InputBinding::GamepadAxis(Platform::Joystick::GamepadAxis::LeftX).withDeadzone(0.15f);
    EXPECT_FLOAT_EQ(axis.deadzone, 0.15f);

    axis.handleAxis(0.1f);
    EXPECT_EQ(axis.getContribution(), glm::vec2(0.f));
    axis.handleAxis(-0.1f);
    EXPECT_EQ(axis.getContribution(), glm::vec2(0.f));

    // The deadzone is a threshold, not a rescale: values outside it pass through unchanged.
    axis.handleAxis(0.5f);
    EXPECT_EQ(axis.getContribution(), glm::vec2(0.5f, 0.f));
    axis.handleAxis(-0.5f);
    EXPECT_EQ(axis.getContribution(), glm::vec2(-0.5f, 0.f));
    axis.handleAxis(0.15f);
    EXPECT_EQ(axis.getContribution(), glm::vec2(0.15f, 0.f));
}

TEST(InputBindingTests, DefaultAxisDeadzoneIsZeroSoOldBehaviourIsKept)
{
    auto axis = NX::InputBinding::GamepadAxis(Platform::Joystick::GamepadAxis::LeftX);
    EXPECT_FLOAT_EQ(axis.deadzone, 0.f);
    axis.handleAxis(0.01f);
    EXPECT_EQ(axis.getContribution(), glm::vec2(0.01f, 0.f));
}

TEST(InputBindingTests, DeadzoneSurvivesSerializationAndIsOptionalInOldData)
{
    const auto axis
        = NX::InputBinding::GamepadAxis(Platform::Joystick::GamepadAxis::RightX).withDeadzone(0.2f);
    nlohmann::json json = axis;
    EXPECT_FLOAT_EQ(json.get<NX::InputBinding>().deadzone, 0.2f);

    // Data saved before the field existed must still load, with no deadzone.
    json.erase("deadzone");
    EXPECT_FLOAT_EQ(json.get<NX::InputBinding>().deadzone, 0.f);
}

TEST(InputBindingTests, DeadzoneKeepsOtherFieldsWhenChained)
{
    const auto copy = NX::InputBinding::GamepadAxis(Platform::Joystick::GamepadAxis::LeftY)
                          .negated()
                          .withDeadzone(0.3f);
    EXPECT_TRUE(copy.negate);
    EXPECT_FLOAT_EQ(copy.deadzone, 0.3f);
    EXPECT_FLOAT_EQ(NX::InputBinding::Key(Platform::Keyboard::Key::W).deadzone, 0.f);
}
