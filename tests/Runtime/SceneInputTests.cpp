// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Framework/InputSystem.h"
#include "NxWorld/Framework/World.h"
#include "NxWorld/Scene/SceneManager.h"
#include "NxWorld/Scene/SceneObjects/Spectator/Spectator.h"
#include "Platform/Input/InputDevices/Joystick.h"
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
            NX::GetInputSystem().resetInput();
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

        static void scroll(std::initializer_list<NX::Spectator2D*> spectators)
        {
            NX::MouseScrollEvent event;
            event.setOffset(glm::vec2(1.f, 0.f));
            NX::GetInputSystem().queueEvent(event);
            NX::GetInputSystem().processEvents();

            for (auto* spectator : spectators)
            {
                spectator->tick(1.f);
            }
        }

        static void gamepadAxis(Platform::Joystick::GamepadAxis axis, float value,
                                NX::Spectator2D& spectator)
        {
            NX::GamepadAxisEvent event(1);
            event.setAxis(static_cast<std::uint16_t>(axis));
            event.setValue(value);
            NX::GetInputSystem().queueEvent(event);
            NX::GetInputSystem().processEvents();
            spectator.tick(1.f);
        }

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

    scroll({ firstSpectator.get(), secondSpectator.get() });
    EXPECT_NE(firstSpectator->getPosition(), firstStart);
    EXPECT_EQ(secondSpectator->getPosition(), secondStart);

    ASSERT_TRUE(manager.setCurrentScene(&second));
    const auto firstAfterSwitch = firstSpectator->getPosition();
    scroll({ firstSpectator.get(), secondSpectator.get() });
    EXPECT_EQ(firstSpectator->getPosition(), firstAfterSwitch);
    EXPECT_NE(secondSpectator->getPosition(), secondStart);
}

TEST_F(SceneInputTests, SpectatorAddedToCurrentSceneListensImmediately)
{
    NX::SceneManager manager(&gGameInstance->world);
    const auto spectator = addSpectator(*manager.getCurrentScene());
    const auto start = spectator->getPosition();

    scroll({ spectator.get() });

    EXPECT_NE(spectator->getPosition(), start);
}

TEST_F(SceneInputTests, SpectatorSurvivesRemovalOfItsInputController)
{
    NX::SceneManager manager(&gGameInstance->world);
    auto spectator = addSpectator(*manager.getCurrentScene());
    auto* input = spectator->findFirstChildOf<NX::InputController>();
    ASSERT_NE(input, nullptr);

    ASSERT_TRUE(spectator->removeChild(input));
    ASSERT_EQ(spectator->findFirstChildOf<NX::InputController>(), nullptr);
    const auto start = spectator->getPosition();

    scroll({ spectator.get() });

    EXPECT_EQ(spectator->getPosition(), start);
}

TEST_F(SceneInputTests, SpectatorAddedToBackgroundSceneStaysSilent)
{
    NX::SceneManager manager(&gGameInstance->world);
    auto& background = manager.createNewScene();
    const auto spectator = addSpectator(background);
    const auto start = spectator->getPosition();

    scroll({ spectator.get() });

    EXPECT_EQ(spectator->getPosition(), start);
}

TEST_F(SceneInputTests, SpectatorRemovedFromSceneStopsListening)
{
    NX::SceneManager manager(&gGameInstance->world);
    auto& scene = *manager.getCurrentScene();
    const auto spectator = addSpectator(scene);
    ASSERT_TRUE(scene.deleteFromScene(spectator.get()));
    const auto start = spectator->getPosition();

    scroll({ spectator.get() });

    EXPECT_EQ(spectator->getPosition(), start);
}

TEST_F(SceneInputTests, GamepadStickPansSpectator)
{
    NX::SceneManager manager(&gGameInstance->world);
    auto spectator = addSpectator(*manager.getCurrentScene());
    const auto start = spectator->getPosition();

    gamepadAxis(Platform::Joystick::GamepadAxis::LeftX, 1.f, *spectator);

    EXPECT_NE(spectator->getPosition(), start);
}

TEST_F(SceneInputTests, GamepadStickDriftInsideDeadzoneIsIgnored)
{
    NX::SceneManager manager(&gGameInstance->world);
    auto spectator = addSpectator(*manager.getCurrentScene());
    const auto start = spectator->getPosition();

    gamepadAxis(Platform::Joystick::GamepadAxis::LeftX, 0.05f, *spectator);

    EXPECT_EQ(spectator->getPosition(), start);
}

TEST_F(SceneInputTests, GamepadTriggersZoomCamera)
{
    NX::SceneManager manager(&gGameInstance->world);
    auto spectator = addSpectator(*manager.getCurrentScene());
    auto* camera = spectator->addChildComponent<NX::OrthographicCamera>("Camera"_atom);
    ASSERT_NE(camera, nullptr);
    const auto startZoom = camera->getZoom();

    gamepadAxis(Platform::Joystick::GamepadAxis::RightTrigger, 1.f, *spectator);
    EXPECT_GT(camera->getZoom(), startZoom);

    const auto zoomedIn = camera->getZoom();
    gamepadAxis(Platform::Joystick::GamepadAxis::RightTrigger, -1.f, *spectator);
    gamepadAxis(Platform::Joystick::GamepadAxis::LeftTrigger, 1.f, *spectator);
    EXPECT_LT(camera->getZoom(), zoomedIn);
}
