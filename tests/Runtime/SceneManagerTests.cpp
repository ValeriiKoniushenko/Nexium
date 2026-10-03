// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxFundamental/ResourceManagement/DataStream.h"
#include "NxWorld/Scene/Scene/SceneManager.h"

#include "gtest/gtest.h"
#include <chrono>
#include <fstream>

namespace
{
    class SceneManagerTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            root
                = std::filesystem::temp_directory_path()
                  / ("nexium_scene_manager_tests_"
                     + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
            std::filesystem::create_directories(root);
            previousCacheRoot = NX::GetCacheSystem().getCacheRoot();
            NX::GetCacheSystem().setCacheRoot(root);
        }

        void TearDown() override
        {
            NX::GetCacheSystem().setCacheRoot(previousCacheRoot);
            std::filesystem::remove_all(root);
        }

        std::filesystem::path root;
        std::filesystem::path previousCacheRoot;
    };
} // namespace

TEST_F(SceneManagerTests, WeakSceneReferenceExpiresAfterRemoval)
{
    NX::SceneManager manager;
    Core::WeakPtr<NX::Scene> pending = &manager.createNewScene();
    ASSERT_TRUE(pending.tryLoad());
    manager.removeScene(pending.get());
    EXPECT_FALSE(pending);
    EXPECT_FALSE(pending.tryLoad());
}

TEST_F(SceneManagerTests, LoadedSceneSurvivesRemovalUntilReleased)
{
    NX::SceneManager manager;
    Core::WeakPtr<NX::Scene> pending = &manager.createNewScene();
    {
        auto scene = pending.tryLoad();
        ASSERT_TRUE(scene);
        manager.removeScene(scene.get());
        EXPECT_TRUE(pending);
        EXPECT_EQ(manager.getScene(scene->getSceneName()), nullptr);
    }
    EXPECT_FALSE(pending);
}

TEST_F(SceneManagerTests, KeepsCurrentSceneStableWhenAddingScenes)
{
    NX::SceneManager manager;
    auto* initial = manager.getCurrentScene();
    ASSERT_NE(initial, nullptr);
    EXPECT_EQ(manager.getScene("Default"_atom), initial);
    for (int i = 0; i < 32; ++i)
    {
        manager.createNewScene();
    }
    EXPECT_EQ(manager.getCurrentScene(), initial);
    EXPECT_EQ(manager.getScene("Default"_atom), initial);
}

TEST_F(SceneManagerTests, SwitchesAndRemovesScenesWithoutBecomingEmpty)
{
    NX::SceneManager manager;
    auto* initial = manager.getCurrentScene();
    auto& added = manager.createNewScene();
    const auto name = added.getSceneName();
    EXPECT_TRUE(manager.setCurrentScene(name));
    EXPECT_EQ(manager.getCurrentScene(), &added);
    EXPECT_FALSE(manager.setCurrentScene("missing"_atom));
    EXPECT_EQ(manager.getCurrentScene(), &added);
    manager.removeScene("missing"_atom);
    EXPECT_EQ(manager.getCurrentScene(), &added);
    manager.removeScene(name);
    EXPECT_EQ(manager.getScene(name), nullptr);
    EXPECT_EQ(manager.getCurrentScene(), initial);
    manager.removeScene("Default"_atom);
    EXPECT_EQ(manager.getScenes().size(), 1);
    EXPECT_EQ(manager.getCurrentScene(), initial);
}

TEST_F(SceneManagerTests, RemovesInactiveSceneAndResetsToDefault)
{
    NX::SceneManager manager;
    auto& added = manager.createNewScene();
    const auto name = added.getSceneName();
    ASSERT_TRUE(manager.setCurrentScene(name));
    manager.removeScene("Default"_atom);
    EXPECT_EQ(manager.getScenes().size(), 1);
    EXPECT_EQ(manager.getCurrentScene(), &added);
    EXPECT_EQ(manager.getScene("Default"_atom), nullptr);
    manager.removeAllScenes();
    ASSERT_NE(manager.getCurrentScene(), nullptr);
    EXPECT_EQ(manager.getScene("Default"_atom), manager.getCurrentScene());
    EXPECT_EQ(manager.getScene(name), nullptr);
}

TEST_F(SceneManagerTests, RejectsNamesThatShareAPortableCacheFile)
{
    NX::SceneManager manager;
    auto& first = manager.createNewScene();
    auto& second = manager.createNewScene();

    ASSERT_TRUE(manager.renameScene(&first, "Level 1"_atom));
    EXPECT_FALSE(manager.renameScene(&second, "Level-1"_atom));
    EXPECT_FALSE(manager.renameScene(&second, "LEVEL 1"_atom));
    EXPECT_TRUE(manager.renameScene(&first, "Level-1"_atom));
    EXPECT_EQ(first.getSceneName(), "Level-1"_atom);
    EXPECT_EQ(second.getSceneName(), "Scene_2"_atom);
}

TEST_F(SceneManagerTests, GeneratedNamesSkipExistingPortableCacheFiles)
{
    NX::SceneManager manager;
    auto& renamed = manager.createNewScene();
    ASSERT_TRUE(manager.renameScene(&renamed, "Scene-3"_atom));

    EXPECT_EQ(manager.createNewScene().getSceneName(), "Scene_1"_atom);
    EXPECT_EQ(manager.createNewScene().getSceneName(), "Scene_2"_atom);
    EXPECT_EQ(manager.createNewScene().getSceneName(), "Scene_4"_atom);
}

TEST_F(SceneManagerTests, SuccessfulRenamePublishesNewFileBeforeRemovingOldFile)
{
    NX::SceneManager manager;
    auto& scene = manager.createNewScene();
    auto& cache = NX::GetCacheSystem();
    const auto oldPath = cache.getCacheFilePath(scene);

    ASSERT_TRUE(std::filesystem::exists(oldPath));
    ASSERT_TRUE(manager.renameScene(&scene, "Renamed"_atom));
    EXPECT_FALSE(std::filesystem::exists(oldPath));
    EXPECT_TRUE(std::filesystem::exists(cache.getCacheFilePath(scene)));
}

TEST_F(SceneManagerTests, FailedRenameKeepsNameAndPreviousFile)
{
    NX::SceneManager manager;
    auto& scene = manager.createNewScene();
    auto& cache = NX::GetCacheSystem();
    const auto oldName = scene.getSceneName();
    const auto oldPath = cache.getCacheFilePath(scene);
    const auto blockedPath = root / "scenes/Blocked.json";
    std::filesystem::create_directory(blockedPath);

    EXPECT_FALSE(manager.renameScene(&scene, "Blocked"_atom));
    EXPECT_EQ(scene.getSceneName(), oldName);
    EXPECT_TRUE(std::filesystem::exists(oldPath));
    EXPECT_TRUE(std::filesystem::is_directory(blockedPath));
}

TEST_F(SceneManagerTests, RemoveAllScenesDeletesOldFilesAndWritesNewDefault)
{
    NX::SceneManager manager;
    auto& cache = NX::GetCacheSystem();
    auto* initial = manager.getCurrentScene();
    initial->addObjectToScene(NX::SceneObject::Create());
    ASSERT_TRUE(cache.write(*initial));
    auto& first = manager.createNewScene();
    auto& second = manager.createNewScene();
    const auto firstPath = cache.getCacheFilePath(first);
    const auto secondPath = cache.getCacheFilePath(second);

    manager.removeAllScenes();

    ASSERT_EQ(manager.getScenes().size(), 1u);
    EXPECT_EQ(manager.getCurrentScene()->getSceneName(), "Default"_atom);
    EXPECT_FALSE(std::filesystem::exists(firstPath));
    EXPECT_FALSE(std::filesystem::exists(secondPath));
    const auto defaultPath = cache.getCacheFilePath(*manager.getCurrentScene());
    ASSERT_TRUE(std::filesystem::exists(defaultPath));
    std::ifstream defaultFile(defaultPath);
    EXPECT_TRUE(nlohmann::json::parse(defaultFile)["sceneObjects"].empty());
}
