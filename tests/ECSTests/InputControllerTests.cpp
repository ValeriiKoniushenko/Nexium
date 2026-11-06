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
