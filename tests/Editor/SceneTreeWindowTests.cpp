// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/Windows/Scenes/SceneTreeWindow.h"

#include "gtest/gtest.h"

namespace
{
    TEST(SceneTreeWindow, HasDefaultNameBeforeInitialization)
    {
        const NX::SceneTreeWindowEWC window;
        EXPECT_FALSE(window.isInitialized());
        EXPECT_EQ(window.getComponentName(), "Scene"_atom);
    }

    TEST(SceneTreeWindow, PreservesExplicitNameDuringConstruction)
    {
        const NX::SceneTreeWindowEWC window("Custom scene tree"_atom);
        EXPECT_EQ(window.getComponentName(), "Custom scene tree"_atom);
    }
} // namespace
