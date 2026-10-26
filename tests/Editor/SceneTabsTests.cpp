// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/IconsFontAwesome.h"
#include "Editor/Windows/Scenes/SceneTabs/SceneTabs.h"
#include "NxWorld/Scene/Scene.h"

#include "gtest/gtest.h"

TEST(SceneTabsTests, TabNameIsThePlainSceneNameForOrdinaryScenes)
{
    NX::Scene scene;
    scene.setSceneName("Level"_atom);

    EXPECT_EQ(NX::SceneTabs::MakeTabName(scene, false), "Level");
}

TEST(SceneTabsTests, StartSceneTabNameCarriesAMarker)
{
    NX::Scene scene;
    scene.setSceneName("Level"_atom);

    const auto name = NX::SceneTabs::MakeTabName(scene, true);

    EXPECT_TRUE(name.starts_with(ICON_FA_FLAG));
    EXPECT_TRUE(name.ends_with("Level"));
    EXPECT_EQ(scene.getSceneName(), "Level"_atom);
}

TEST(SceneTabsTests, LongNamesAreShortenedBeforeTheMarkerIsAdded)
{
    NX::Scene scene;
    scene.setSceneName("AVeryLongSceneNameThatDoesNotFitOnATab"_atom);

    const auto plain = NX::SceneTabs::MakeTabName(scene, false);
    const auto marked = NX::SceneTabs::MakeTabName(scene, true);

    EXPECT_TRUE(plain.ends_with("..."));
    EXPECT_TRUE(marked.ends_with(plain));
}
