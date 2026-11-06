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
#include "NxWorld/Scene/SceneObjects/SceneObject.h"
#include "Platform/Input/InputDevices/Keyboard.h"

#include "gtest/gtest.h"

TEST(InputControllerTests, IsRegisteredInComponentFactory)
{
    EXPECT_TRUE(NX::GetGlobalComponentFactory().containsSuchType("NX::InputController"_atom));
}

TEST(InputControllerTests, CanBeAttachedToSceneObject)
{
    auto object = NX::SceneObject::Create();
    auto* controller = object->addChildComponent<NX::InputController>();

    ASSERT_NE(controller, nullptr);
    EXPECT_EQ(object->findFirstChildOf<NX::InputController>(), controller);
}

namespace
{
    NX::InputBinding KeyBinding(Platform::Keyboard::Key key)
    {
        NX::InputBinding binding;
        binding.source = NX::InputSource::Key;
        binding.code = static_cast<std::uint16_t>(key);
        return binding;
    }

    NX::InputAction MoveAction()
    {
        auto up = KeyBinding(Platform::Keyboard::Key::W);
        up.swizzleYX = true;

        auto down = KeyBinding(Platform::Keyboard::Key::S);
        down.swizzleYX = true;
        down.negate = true;

        NX::InputAction move;
        move.name = "Move"_atom;
        move.bindings = { up, down };
        return move;
    }
} // namespace

TEST(InputControllerTests, AddActionRejectsEmptyAndDuplicateNames)
{
    auto controller = NX::InputController::Create();

    EXPECT_TRUE(controller->addAction(MoveAction()));
    EXPECT_FALSE(controller->addAction(MoveAction()));
    EXPECT_FALSE(controller->addAction(NX::InputAction{}));
    EXPECT_EQ(controller->getActions().size(), 1U);
}

TEST(InputControllerTests, FindsAndRemovesActionsByName)
{
    auto controller = NX::InputController::Create();
    ASSERT_TRUE(controller->addAction(MoveAction()));

    const auto* move = controller->findAction("Move"_atom);
    ASSERT_NE(move, nullptr);
    EXPECT_EQ(move->bindings.size(), 2U);
    EXPECT_EQ(controller->findAction("Jump"_atom), nullptr);

    EXPECT_FALSE(controller->removeAction("Jump"_atom));
    EXPECT_TRUE(controller->removeAction("Move"_atom));
    EXPECT_TRUE(controller->getActions().empty());
}

TEST(InputControllerTests, BindingBuildsModifierMask)
{
    NX::InputBinding binding;
    EXPECT_EQ(binding.getModifiers(), NX::InputModifier::None);

    binding.control = true;
    binding.shift = true;
    EXPECT_EQ(binding.getModifiers(), NX::InputModifier::Control | NX::InputModifier::Shift);

    binding.alt = true;
    binding.super = true;
    EXPECT_EQ(binding.getModifiers(), NX::InputModifier::Control | NX::InputModifier::Shift
                                          | NX::InputModifier::Alt | NX::InputModifier::Super);
}

TEST(InputControllerTests, ActionsSurviveSerializationRoundTrip)
{
    auto original = NX::InputController::Create();
    ASSERT_TRUE(original->addAction(MoveAction()));

    auto save = KeyBinding(Platform::Keyboard::Key::S);
    save.control = true;
    save.shift = true;
    save.trigger = NX::InputTrigger::Pressed;
    ASSERT_TRUE(original->addAction(NX::InputAction{ .name = "Save"_atom, .bindings = { save } }));

    RResourceStream<RJsonResourceStream> stream(original->serialize());
    auto restored = NX::InputController::Create();
    restored->deserialize(stream);

    ASSERT_EQ(restored->getActions().size(), 2U);

    const auto* move = restored->findAction("Move"_atom);
    ASSERT_NE(move, nullptr);
    ASSERT_EQ(move->bindings.size(), 2U);
    EXPECT_EQ(move->bindings[1].code, static_cast<std::uint16_t>(Platform::Keyboard::Key::S));
    EXPECT_TRUE(move->bindings[1].negate);
    EXPECT_TRUE(move->bindings[1].swizzleYX);

    const auto* restoredSave = restored->findAction("Save"_atom);
    ASSERT_NE(restoredSave, nullptr);
    ASSERT_EQ(restoredSave->bindings.size(), 1U);
    EXPECT_EQ(restoredSave->bindings[0].trigger, NX::InputTrigger::Pressed);
    EXPECT_EQ(restoredSave->bindings[0].getModifiers(),
              NX::InputModifier::Control | NX::InputModifier::Shift);
}

namespace
{
    using Key = Platform::Keyboard::Key;

    class InputControllerRuntimeTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            NX::GetInputSystem().resetInput();
            controller = NX::InputController::Create();
        }

        void TearDown() override { NX::GetInputSystem().resetInput(); }

        void addAction(const char* name, std::vector<NX::InputBinding> bindings)
        {
            ASSERT_TRUE(controller->addAction(NX::InputAction{ .name = Core::StringAtom(name),
                                                               .bindings = std::move(bindings) }));
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

        static void frame() { NX::GetInputSystem().processEvents(); }

        NX::InputController::Ptr controller;
    };

    NX::InputBinding KeyBinding(Key key, NX::InputTrigger trigger)
    {
        auto binding = KeyBinding(key);
        binding.trigger = trigger;
        return binding;
    }
} // namespace

TEST_F(InputControllerRuntimeTests, RegistersOnInitializeAndUnregistersOnDestruction)
{
    EXPECT_FALSE(NX::GetInputSystem().isRegistered(controller.get()));
    controller->initialize();
    EXPECT_TRUE(NX::GetInputSystem().isRegistered(controller.get()));

    const auto* raw = controller.get();
    controller = nullptr;
    EXPECT_FALSE(NX::GetInputSystem().isRegistered(raw));
}

TEST_F(InputControllerRuntimeTests, DownBindingIsActiveWhileHeld)
{
    addAction("Fire", { KeyBinding(Key::Space, NX::InputTrigger::Down) });
    controller->initialize();

    key(Key::Space, NX::InputState::Pressed);
    frame();
    EXPECT_TRUE(controller->isActive("Fire"_atom));
    EXPECT_TRUE(controller->wasTriggered("Fire"_atom));

    frame();
    EXPECT_TRUE(controller->isActive("Fire"_atom));
    EXPECT_FALSE(controller->wasTriggered("Fire"_atom));

    key(Key::Space, NX::InputState::Released);
    frame();
    EXPECT_FALSE(controller->isActive("Fire"_atom));
}

TEST_F(InputControllerRuntimeTests, QuickTapWithinOneFrameIsTriggered)
{
    addAction("Fire", { KeyBinding(Key::Space, NX::InputTrigger::Down) });
    controller->initialize();

    key(Key::Space, NX::InputState::Pressed);
    key(Key::Space, NX::InputState::Released);
    frame();
    EXPECT_TRUE(controller->wasTriggered("Fire"_atom));
    EXPECT_FALSE(controller->isActive("Fire"_atom));
}

TEST_F(InputControllerRuntimeTests, PressedAndReleasedTriggersFireOnce)
{
    addAction("Jump", { KeyBinding(Key::Space, NX::InputTrigger::Pressed) });
    addAction("Land", { KeyBinding(Key::Space, NX::InputTrigger::Released) });
    controller->initialize();

    key(Key::Space, NX::InputState::Pressed);
    frame();
    EXPECT_TRUE(controller->wasTriggered("Jump"_atom));
    EXPECT_FALSE(controller->wasTriggered("Land"_atom));

    frame();
    EXPECT_FALSE(controller->wasTriggered("Jump"_atom));
    EXPECT_FALSE(controller->isActive("Jump"_atom));

    key(Key::Space, NX::InputState::Released);
    frame();
    EXPECT_TRUE(controller->wasTriggered("Land"_atom));
    EXPECT_FALSE(controller->wasTriggered("Jump"_atom));
}

TEST_F(InputControllerRuntimeTests, KeysComposeMoveVector)
{
    auto right = KeyBinding(Key::D);
    auto left = KeyBinding(Key::A);
    left.negate = true;
    auto move = MoveAction();
    move.bindings.push_back(right);
    move.bindings.push_back(left);
    ASSERT_TRUE(controller->addAction(move));
    controller->initialize();

    key(Key::W, NX::InputState::Pressed);
    key(Key::D, NX::InputState::Pressed);
    frame();
    EXPECT_EQ(controller->getValue<glm::vec2>("Move"_atom), glm::vec2(1.f, 1.f));

    key(Key::S, NX::InputState::Pressed);
    key(Key::D, NX::InputState::Released);
    frame();
    EXPECT_EQ(controller->getValue<glm::vec2>("Move"_atom), glm::vec2(0.f, 0.f));
    EXPECT_FALSE(controller->isActive("Move"_atom));

    key(Key::W, NX::InputState::Released);
    frame();
    EXPECT_EQ(controller->getValue<glm::vec2>("Move"_atom), glm::vec2(0.f, -1.f));
}

TEST_F(InputControllerRuntimeTests, ModifiersMustMatchExactlyIgnoringLocks)
{
    auto save = KeyBinding(Key::S, NX::InputTrigger::Pressed);
    save.control = true;
    addAction("Save", { save });
    controller->initialize();

    key(Key::S, NX::InputState::Pressed, NX::InputModifier::Control | NX::InputModifier::Shift);
    frame();
    EXPECT_FALSE(controller->wasTriggered("Save"_atom));

    key(Key::S, NX::InputState::Released);
    key(Key::S, NX::InputState::Pressed);
    frame();
    EXPECT_FALSE(controller->wasTriggered("Save"_atom));

    key(Key::S, NX::InputState::Released);
    key(Key::S, NX::InputState::Pressed, NX::InputModifier::Control | NX::InputModifier::CapsLock);
    frame();
    EXPECT_TRUE(controller->wasTriggered("Save"_atom));
}

TEST_F(InputControllerRuntimeTests, MouseDeltaAccumulatesWithinFrame)
{
    NX::InputBinding look;
    look.source = NX::InputSource::MouseDelta;
    addAction("Look", { look });
    controller->initialize();

    NX::MouseMoveEvent first;
    first.setDelta({ 1.f, 2.f });
    NX::MouseMoveEvent second;
    second.setDelta({ 3.f, 4.f });
    NX::GetInputSystem().queueEvent(first);
    NX::GetInputSystem().queueEvent(second);
    frame();
    EXPECT_EQ(controller->getValue<glm::vec2>("Look"_atom), glm::vec2(4.f, 6.f));

    frame();
    EXPECT_EQ(controller->getValue<glm::vec2>("Look"_atom), glm::vec2(0.f, 0.f));
}

TEST_F(InputControllerRuntimeTests, GamepadTriggerAxisIsNormalizedFromRest)
{
    NX::InputBinding throttle;
    throttle.source = NX::InputSource::GamepadAxis;
    throttle.code = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
    NX::InputBinding steer;
    steer.source = NX::InputSource::GamepadAxis;
    steer.code = GLFW_GAMEPAD_AXIS_LEFT_X;
    addAction("Throttle", { throttle });
    addAction("Steer", { steer });
    controller->initialize();

    NX::GamepadAxisEvent trigger(1);
    trigger.setAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER);
    trigger.setValue(0.f);
    NX::GamepadAxisEvent stick(1);
    stick.setAxis(GLFW_GAMEPAD_AXIS_LEFT_X);
    stick.setValue(-0.5f);
    NX::GetInputSystem().queueEvent(trigger);
    NX::GetInputSystem().queueEvent(stick);
    frame();
    EXPECT_FLOAT_EQ(controller->getValue<float>("Throttle"_atom), 0.5f);
    EXPECT_FLOAT_EQ(controller->getValue<float>("Steer"_atom), -0.5f);

    trigger.setValue(-1.f);
    NX::GetInputSystem().queueEvent(trigger);
    frame();
    EXPECT_FALSE(controller->isActive("Throttle"_atom));
    EXPECT_FLOAT_EQ(controller->getValue<float>("Steer"_atom), -0.5f);
}

TEST_F(InputControllerRuntimeTests, UnknownActionReturnsDefaults)
{
    controller->initialize();
    frame();
    EXPECT_FALSE(controller->isActive("Missing"_atom));
    EXPECT_FALSE(controller->wasTriggered("Missing"_atom));
    EXPECT_EQ(controller->getValue<glm::vec2>("Missing"_atom), glm::vec2(0.f));
}

TEST_F(InputControllerRuntimeTests, DisabledControllerKeepsTrackingReleases)
{
    addAction("Fire", { KeyBinding(Key::Space, NX::InputTrigger::Down) });
    controller->initialize();

    key(Key::Space, NX::InputState::Pressed);
    frame();
    ASSERT_TRUE(controller->isActive("Fire"_atom));

    controller->disable();
    EXPECT_FALSE(controller->isActive("Fire"_atom));
    key(Key::Space, NX::InputState::Released);
    frame();

    controller->enable();
    EXPECT_FALSE(controller->isActive("Fire"_atom));
}
