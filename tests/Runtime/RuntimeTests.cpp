// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxFundamental/ECS/BaseComponent.h"
#include "NxRuntime/Runtime.h"
#include "NxWorld/Framework/GameInstance.h"

#include "gtest/gtest.h"

TEST(RuntimeTests, KeepsBoundGameInstance)
{
    char executableName[] = "Nexium_Tests";
    char* arguments[] = { executableName };
    NX::GameInstance gameInstance{ 1, arguments };
    NX::Runtime runtime{ gameInstance };

    EXPECT_EQ(&runtime.getGameInstance(), &gameInstance);
}

TEST(RuntimeTests, LinksWorldComponentRegistrars)
{
    const auto& factory = NX::GetGlobalComponentFactory();

    EXPECT_TRUE(factory.containsSuchType("NX::SceneObj::Rectangle"_atom));
    EXPECT_TRUE(factory.containsSuchType("NX::SceneObj::RectangleAnimated"_atom));
}
