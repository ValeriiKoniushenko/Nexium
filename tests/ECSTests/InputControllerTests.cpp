// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/Input/InputController.h"
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
        move.type = NX::ActionValueType::Axis2D;
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
    EXPECT_EQ(move->type, NX::ActionValueType::Axis2D);
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
