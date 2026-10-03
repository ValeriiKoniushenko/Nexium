// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/EditorIntegration.h"
#include "Foundation/Configs.h"
#include "NxFundamental/ECS/BaseComponent.h"
#include "NxFundamental/ResourceManagement/DataStream.h"
#include "NxRuntime/Runtime.h"
#include "NxSubsystems/Input/InputManager.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Scene/SceneObjects/Rectangle/Rectangle.h"
#include "NxWorld/Scene/SceneObjects/Spectator/Spectator.h"
#include "Platform/Window.h"

#include "gtest/gtest.h"
#include <array>
#include <chrono>
#include <fstream>

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

    class TestDirectory final
    {
    public:
        TestDirectory()
            : path(
                  std::filesystem::temp_directory_path()
                  / ("nexium_runtime_tests_"
                     + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
        {
            std::filesystem::create_directories(path);
            previousCacheRoot = NX::GetCacheSystem().getCacheRoot();
            NX::GetCacheSystem().setCacheRoot(path);
        }

        ~TestDirectory()
        {
            NX::GetCacheSystem().setCacheRoot(previousCacheRoot);
            std::filesystem::remove_all(path);
        }

        std::filesystem::path path;
        std::filesystem::path previousCacheRoot;
    };
} // namespace

TEST(RuntimeTests, KeepsBoundGameInstance)
{
    auto executableName = std::to_array("Nexium_Tests");
    std::array<char*, 1> arguments{ executableName.data() };
    TestDirectory sceneDirectory;
    NX::GameInstance gameInstance{ 1, arguments.data() };
    NX::Runtime runtime{ gameInstance };

    EXPECT_EQ(&runtime.getGameInstance(), &gameInstance);
}

TEST(RuntimeTests, SceneChangesClearSelectionWithoutEditorWindows)
{
    auto executableName = std::to_array("Nexium_Tests");
    std::array<char*, 1> arguments{ executableName.data() };
    TestDirectory sceneDirectory;
    NX::GameInstance gameInstance{ 1, arguments.data() };
    NX::Runtime runtime{ gameInstance };
    NX::EditorIntegration editor{ runtime };
    auto& selector = editor.getObjectSelectorManager();
    auto object = NX::SceneObject::Create();
    auto& scenes = gameInstance.scenes;
    auto& next = scenes.createNewScene();

    selector.selectSingleObject(object.get());
    ASSERT_TRUE(selector.isSelected(object.get()));
    EXPECT_TRUE(scenes.setCurrentScene(scenes.getCurrentScene()));
    EXPECT_TRUE(selector.isSelected(object.get()));
    EXPECT_TRUE(scenes.setCurrentScene(&next));
    EXPECT_TRUE(selector.getSelectedObjects().empty());

    selector.selectSingleObject(object.get());
    scenes.removeScene(&next);
    EXPECT_TRUE(selector.getSelectedObjects().empty());

    selector.selectSingleObject(object.get());
    scenes.removeAllScenes();
    EXPECT_TRUE(selector.getSelectedObjects().empty());
}

TEST(RuntimeTests, SceneSwitchRestoresSelectedMainCamera)
{
    auto executableName = std::to_array("Nexium_Tests");
    std::array<char*, 1> arguments{ executableName.data() };
    TestDirectory sceneDirectory;
    NX::GameInstance gameInstance{ 1, arguments.data() };
    auto* initial = gameInstance.scenes.getCurrentScene();
    auto initialRoot = NX::SceneObject::Create();
    auto* firstCamera = initialRoot->addChildComponent<NX::OrthographicCamera>("First"_atom);
    auto* selectedCamera = initialRoot->addChildComponent<NX::OrthographicCamera>("Selected"_atom);
    initial->addObjectToScene(initialRoot);
    EXPECT_EQ(gameInstance.world.getCurrentCamera(), firstCamera);
    gameInstance.world.setCurrentCamera(selectedCamera);

    auto& next = gameInstance.scenes.createNewScene();
    auto nextRoot = NX::SceneObject::Create();
    auto* nextCamera = nextRoot->addChildComponent<NX::OrthographicCamera>("Next"_atom);
    next.addObjectToScene(nextRoot);

    EXPECT_TRUE(gameInstance.scenes.setCurrentScene(&next));
    EXPECT_EQ(gameInstance.world.getCurrentCamera(), nextCamera);
    EXPECT_TRUE(gameInstance.scenes.setCurrentScene(initial));
    EXPECT_EQ(gameInstance.world.getCurrentCamera(), selectedCamera);

    gameInstance.scenes.removeScene(&next);
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

TEST(RuntimeTests, DefaultRectangleIsInsideSpectatorAssetDepthRange)
{
    auto executableName = std::to_array("Nexium_Tests");
    std::array<char*, 1> arguments{ executableName.data() };
    gGameInstance = std::make_unique<NX::GameInstance>(1, arguments.data());
    TestViewport viewport;
    gGameInstance->setApplicationIntegration(&viewport);

    {
        std::ifstream spectatorFile(Foundation::Config::Path::assets / "2D_Spectator.nx");
        std::ifstream rectangleFile(Foundation::Config::Path::assets / "BaseRectangle.nx");
        const auto spectatorData = nlohmann::json::parse(spectatorFile);
        const auto rectangleData = nlohmann::json::parse(rectangleFile);
        NX::Spectator2D spectator;
        NX::SceneObj::Rectangle rectangle;
        auto spectatorStream = RResourceStream<RJsonResourceStream>(spectatorData.at("data"));
        auto rectangleStream = RResourceStream<RJsonResourceStream>(rectangleData.at("data"));
        spectator.deserialize(spectatorStream);
        rectangle.deserialize(rectangleStream);

        auto* camera = spectator.findFirstChildOf<NX::OrthographicCamera>();
        EXPECT_NE(camera, nullptr);
        if (camera)
        {
            const auto clip = camera->getMatrix() * glm::vec4(rectangle.getPosition(), 1.f);
            EXPECT_GE(clip.z, -clip.w);
            EXPECT_LE(clip.z, clip.w);
        }
    }

    gGameInstance.reset();
}
