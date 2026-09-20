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
#include "NxSubsystems/Input/InputManager.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/GameInstance.h"
#include "Platform/Window.h"

#include "gtest/gtest.h"
#include <array>

namespace
{
    class TestViewport : public NX::ApplicationIntegration
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
} // namespace

TEST(RuntimeTests, KeepsBoundGameInstance)
{
    auto executableName = std::to_array("Nexium_Tests");
    std::array<char*, 1> arguments{ executableName.data() };
    NX::GameInstance gameInstance{ 1, arguments.data() };
    NX::Runtime runtime{ gameInstance };

    EXPECT_EQ(&runtime.getGameInstance(), &gameInstance);
}

TEST(RuntimeTests, LinksWorldComponentRegistrars)
{
    const auto& factory = NX::GetGlobalComponentFactory();

    EXPECT_TRUE(factory.containsSuchType("NX::SceneObj::Rectangle"_atom));
    EXPECT_TRUE(factory.containsSuchType("NX::SceneObj::RectangleAnimated"_atom));
}

TEST(RuntimeTests, MouseInputManagerForwardsWheelEvents)
{
    NX::MouseInputManger input;
    glm::vec2 received{};
    auto subscription
        = input.onWheel->subscribeAndGetID([&](glm::vec2 offset) { received = offset; });

    Platform::GetWindow().onMouseWheel->trigger(glm::vec2(2.f, -3.f));

    EXPECT_FLOAT_EQ(received.x, 2.f);
    EXPECT_FLOAT_EQ(received.y, -3.f);
}

TEST(RuntimeTests, OrthographicCameraZoomUpdatesProjectionAroundFrameCenter)
{
    auto executableName = std::to_array("Nexium_Tests");
    std::array<char*, 1> arguments{ executableName.data() };
    gGameInstance = std::make_unique<NX::GameInstance>(1, arguments.data());
    TestViewport viewport;
    gGameInstance->setApplicationIntegration(&viewport);

    {
        NX::OrthographicCamera camera;
        const auto original = camera.getMatrix();
        camera.setZoom(2.f);
        const auto zoomed = camera.getMatrix();

        EXPECT_FLOAT_EQ(zoomed[0][0], original[0][0] * 2.f);
        EXPECT_FLOAT_EQ(zoomed[1][1], original[1][1] * 2.f);
        const auto center = zoomed * glm::vec4(400.f, 300.f, 0.f, 1.f);
        EXPECT_NEAR(center.x, 0.f, 0.0001f);
        EXPECT_NEAR(center.y, 0.f, 0.0001f);

        camera.applyTypeSpecificSceneData({ { "_zoom", 0.f } });
        EXPECT_FLOAT_EQ(camera.getZoom(), 0.001f);
    }

    gGameInstance.reset();
}
