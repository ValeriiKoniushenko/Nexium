// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "../../sources/NxWorld/Scene/Scene/Scene.h"
#include "Foundation/Configs.h"
#include "NxWorld/Framework/GameInstance.h"

#include "gtest/gtest.h"
#include <chrono>
#include <fstream>

namespace
{
    class ScenePersistenceTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            char executable[] = "Nexium_Tests";
            char* arguments[] = { executable };
            gGameInstance = std::make_unique<NX::GameInstance>(1, arguments);
            root
                = std::filesystem::temp_directory_path()
                  / ("nexium_scene_tests_"
                     + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
            std::filesystem::create_directories(root);
        }

        void TearDown() override
        {
            gGameInstance.reset();
            std::filesystem::remove_all(root);
        }

        Core::StringAtom makeAsset()
        {
            const NX::SceneObject object("Template"_atom);
            const auto path = root / "object.nx";
            const nlohmann::json data = { { "type", NX::SceneObject::componentType },
                                          { "name", "Template" },
                                          { "tags", "WorldObject" },
                                          { "data", object.serialize() } };
            std::ofstream(path) << data.dump();
            gGameInstance->assets.registerNewAssetPath(root);
            gGameInstance->assets.initScanFileSystem();
            return Core::StringAtom::Intern(
                std::filesystem::relative(path, Foundation::Config::Path::projectAbsPath)
                    .generic_string());
        }

        std::filesystem::path root;
    };
} // namespace

TEST_F(ScenePersistenceTest, AssetInstanceNamesAndTransformsSurviveRepeatedLoading)
{
    const auto asset = makeAsset();
    NX::Scene source;
    for (const auto& name : { "First"_atom, "Second"_atom })
    {
        auto object = NX::SceneObject::Create();
        object->setComponentName(name);
        object->_setReferencedAsset(asset);
        object->setPosition({ 12.f, 34.f, 0.f });
        source.addObjectToScene(object);
    }
    auto data = RResourceStream<RJsonResourceStream>(source.serialize());
    NX::Scene restored;
    restored.addObjectToScene(NX::SceneObject::Create());
    restored.deserialize(data);
    restored.deserialize(data);
    ASSERT_EQ(restored.getObjects().size(), 2u);
    EXPECT_EQ(restored.getObjects()[0]->getComponentName(), "First"_atom);
    EXPECT_EQ(restored.getObjects()[1]->getComponentName(), "Second"_atom);
    EXPECT_EQ(restored.getObjects()[1]->getPosition(), glm::vec3(12.f, 34.f, 0.f));
}

TEST_F(ScenePersistenceTest, InlineObjectsPreserveTheirComponentTrees)
{
    NX::Scene source;
    auto object = NX::SceneObject::Create();
    object->setComponentName("Inline"_atom);
    object->addChildComponent<NX::SceneObject>("Child"_atom)->disable();
    source.addObjectToScene(object);
    auto data = RResourceStream<RJsonResourceStream>(source.serialize());
    NX::Scene restored;
    restored.deserialize(data);
    ASSERT_EQ(restored.getObjects().size(), 1u);
    EXPECT_EQ(restored.getObjects()[0]->serialize(), object->serialize());
    EXPECT_FALSE(restored.getObjects()[0]->hasReferencedAsset());
}

TEST_F(ScenePersistenceTest, InvalidLoadPreservesExistingScene)
{
    NX::Scene source;
    auto object = NX::SceneObject::Create();
    object->setComponentName("Kept"_atom);
    source.addObjectToScene(object);
    auto json = source.serialize();
    json["_sceneName"] = "Do not apply";
    json["sceneObjects"].push_back(json["sceneObjects"][0]);
    json["sceneObjects"][1]["componentData"]["_type"] = "UnregisteredSceneObject";
    auto data = RResourceStream<RJsonResourceStream>(json);
    EXPECT_THROW(source.deserialize(data), std::runtime_error);
    EXPECT_EQ(source.getSceneName(), "Default"_atom);
    ASSERT_EQ(source.getObjects().size(), 1u);
    EXPECT_EQ(source.getObjects()[0].get(), object.get());
}

TEST_F(ScenePersistenceTest, ReadsLegacyAssetsAndRejectsFutureVersions)
{
    const auto asset = makeAsset();
    auto object = NX::SceneObject::Create();
    object->setComponentName("Legacy"_atom);
    object->_setReferencedAsset(asset);
    nlohmann::json json
        = { { "_sceneName", "LegacyScene" }, { "sceneObjects", { object->getSceneState() } } };
    NX::Scene restored;
    auto legacy = RResourceStream<RJsonResourceStream>(json);
    restored.deserialize(legacy);
    ASSERT_EQ(restored.getObjects().size(), 1u);
    EXPECT_EQ(restored.getObjects()[0]->getComponentName(), "Legacy"_atom);
    json["formatVersion"] = 999;
    auto future = RResourceStream<RJsonResourceStream>(json);
    EXPECT_THROW(restored.deserialize(future), std::runtime_error);
    EXPECT_EQ(restored.getObjects().size(), 1u);
}

TEST_F(ScenePersistenceTest, EmptyDocumentReplacesSceneAndMalformedDocumentDoesNot)
{
    NX::Scene scene;
    scene.addObjectToScene(NX::SceneObject::Create());
    auto invalid = RResourceStream<RJsonResourceStream>(nlohmann::json::object());
    EXPECT_THROW(scene.deserialize(invalid), std::runtime_error);
    ASSERT_EQ(scene.getObjects().size(), 1u);
    NX::Scene empty;
    auto data = RResourceStream<RJsonResourceStream>(empty.serialize());
    scene.deserialize(data);
    EXPECT_TRUE(scene.getObjects().empty());
}

TEST_F(ScenePersistenceTest, SceneFilesPreserveObjectsInClosedScenes)
{
    NX::SceneManager manager;
    auto* initial = manager.getCurrentScene();
    auto object = NX::SceneObject::Create();
    object->setComponentName("Retained"_atom);
    object->setPosition({ 12.f, 34.f, 0.f });
    initial->addObjectToScene(object);
    auto& added = manager.createNewScene();
    manager.setCurrentScene(&added);
    ASSERT_TRUE(manager.closeScene(initial));
    const auto tabs = manager.serialize();
    EXPECT_FALSE(tabs.contains("scenes"));
    EXPECT_EQ(tabs.at("current"), added.getSceneName().c_str());
    for (const auto& scene : manager.getScenes())
    {
        std::ofstream(root / (std::string(scene->getSceneName().c_str()) + ".json"))
            << scene->serialize().dump();
    }
    auto data = RResourceStream<RJsonResourceStream>(tabs);
    NX::SceneManager restored;
    restored.importScenes(root);
    restored.deserialize(data);
    auto* closed = restored.getScene("Default"_atom);
    ASSERT_NE(closed, nullptr);
    EXPECT_FALSE(restored.isSceneOpen(closed));
    ASSERT_EQ(closed->getObjects().size(), 1);
    EXPECT_EQ(closed->getObjects()[0]->getComponentName(), "Retained"_atom);
    EXPECT_EQ(closed->getObjects()[0]->getPosition(), glm::vec3(12.f, 34.f, 0.f));
    ASSERT_TRUE(restored.setCurrentScene(closed));
    EXPECT_EQ(restored.getCurrentScene(), closed);
}

TEST_F(ScenePersistenceTest, ImportsExistingSceneFilesWithoutDeletingThem)
{
    NX::Scene first;
    NX::Scene second;
    second.setSceneName("Another"_atom);
    std::ofstream(root / "Default.json") << first.serialize().dump();
    std::ofstream(root / "Another.json") << second.serialize().dump();
    NX::SceneManager manager;
    manager.importScenes(root);
    EXPECT_EQ(manager.getScenes().size(), 2);
    EXPECT_NE(manager.getScene("Default"_atom), nullptr);
    EXPECT_NE(manager.getScene("Another"_atom), nullptr);
    EXPECT_TRUE(std::filesystem::exists(root / "Default.json"));
    EXPECT_TRUE(std::filesystem::exists(root / "Another.json"));
    auto* current = manager.getCurrentScene();
    std::ofstream(root / "Broken.json") << "invalid JSON";
    EXPECT_THROW(manager.importScenes(root), std::exception);
    EXPECT_EQ(manager.getCurrentScene(), current);
}

TEST_F(ScenePersistenceTest, MissingSceneReferencesDoNotDiscardLoadedScenes)
{
    NX::SceneManager manager;
    auto* initial = manager.getCurrentScene();
    const nlohmann::json tabs = { { "formatVersion", 1 },
                                  { "open", { "DeletedScene", "Default" } },
                                  { "current", "DeletedScene" } };
    auto data = RResourceStream<RJsonResourceStream>(tabs);
    manager.deserialize(data);
    EXPECT_EQ(manager.getCurrentScene(), initial);
    ASSERT_EQ(manager.getOpenScenes().size(), 1u);
    EXPECT_EQ(manager.getOpenScenes().front(), initial);
}

TEST_F(ScenePersistenceTest, MalformedTabsPreserveCurrentSession)
{
    NX::SceneManager manager;
    auto* initial = manager.getCurrentScene();
    const nlohmann::json tabs
        = { { "formatVersion", 1 }, { "open", { "Default", 42 } }, { "current", "Default" } };
    auto data = RResourceStream<RJsonResourceStream>(tabs);
    EXPECT_THROW(manager.deserialize(data), std::exception);
    EXPECT_EQ(manager.getCurrentScene(), initial);
    EXPECT_EQ(manager.getOpenScenes().size(), 1u);
}
