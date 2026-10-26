// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Framework/World.h"
#include "NxWorld/Scene/SceneManager.h"
#include "NxWorld/Scene/SceneObjects/Spectator/Spectator.h"
#include "Platform/Input/Window.h"
#include "Runtime/SceneFilesGuard.h"

#include "gtest/gtest.h"
#include <array>

namespace
{
    class FocusedViewport : public NX::ApplicationIntegration
    {
    public:
        void preInitialize() override {}
        void initialize() override {}
        void readFromCache() override {}
        void writeToCache() override {}
        void updateInput() override {}
        void tick(float) override {}
        void updateSceneInteraction(NX::Scene&) override {}
        [[nodiscard]] bool isViewportFocused() const override { return true; }
        void beforeSceneDraw() override {}
        void afterSceneDraw() override {}
        void clearSceneRenderTarget() override {}
        [[nodiscard]] Core::ISize2 getRenderSize() const override
        {
            return Core::ISize2{ 800, 600 };
        }
    };

    class SceneInputTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            auto executableName = std::to_array("Nexium_Tests");
            std::array<char*, 1> arguments{ executableName.data() };
            gGameInstance = std::make_unique<NX::GameInstance>(1, arguments.data());
            gGameInstance->setApplicationIntegration(&viewport);
            // Makes a wheel step move a spectator by a measurable amount.
            gGameInstance->world.internal_UpdateTimeDelta(1.f);
        }

        void TearDown() override { gGameInstance.reset(); }

        static NX::Spectator2D::Ptr addSpectator(NX::Scene& scene)
        {
            auto spectator = NX::Spectator2D::Create();
            scene.addObjectToScene(spectator);
            return spectator;
        }

        static void scroll() { Platform::GetWindow().onMouseWheel->trigger(glm::vec2(1.f, 0.f)); }

        SceneFilesGuard sceneFiles;
        FocusedViewport viewport;
    };
} // namespace

TEST_F(SceneInputTests, WheelReachesOnlySpectatorOfCurrentScene)
{
    NX::SceneManager manager(&gGameInstance->world);
    auto& first = *manager.getCurrentScene();
    auto& second = manager.createNewScene();
    const auto firstSpectator = addSpectator(first);
    const auto secondSpectator = addSpectator(second);
    const auto firstStart = firstSpectator->getPosition();
    const auto secondStart = secondSpectator->getPosition();

    scroll();
    EXPECT_NE(firstSpectator->getPosition(), firstStart);
    EXPECT_EQ(secondSpectator->getPosition(), secondStart);

    ASSERT_TRUE(manager.setCurrentScene(&second));
    const auto firstAfterSwitch = firstSpectator->getPosition();
    scroll();
    EXPECT_EQ(firstSpectator->getPosition(), firstAfterSwitch);
    EXPECT_NE(secondSpectator->getPosition(), secondStart);
}

TEST_F(SceneInputTests, SpectatorAddedToCurrentSceneListensImmediately)
{
    NX::SceneManager manager(&gGameInstance->world);
    const auto spectator = addSpectator(*manager.getCurrentScene());
    const auto start = spectator->getPosition();

    scroll();

    EXPECT_NE(spectator->getPosition(), start);
}

TEST_F(SceneInputTests, SpectatorAddedToBackgroundSceneStaysSilent)
{
    NX::SceneManager manager(&gGameInstance->world);
    auto& background = manager.createNewScene();
    const auto spectator = addSpectator(background);
    const auto start = spectator->getPosition();

    scroll();

    EXPECT_EQ(spectator->getPosition(), start);
}

TEST_F(SceneInputTests, SpectatorRemovedFromSceneStopsListening)
{
    NX::SceneManager manager(&gGameInstance->world);
    auto& scene = *manager.getCurrentScene();
    const auto spectator = addSpectator(scene);
    ASSERT_TRUE(scene.deleteFromScene(spectator.get()));
    const auto start = spectator->getPosition();

    scroll();

    EXPECT_EQ(spectator->getPosition(), start);
}
