// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Foundation/Configs.h"
#include "NxFundamental/ResourceManagement/AtomicFile.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Scene/Scene/Scene.h"

#include "gtest/gtest.h"
#include <chrono>
#include <fstream>
#include <optional>

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
            for (const auto& [path, contents] : savedFiles)
            {
                if (contents)
                {
                    NX::WriteFileAtomically(path, *contents);
                }
                else
                {
                    std::filesystem::remove(path);
                }
            }
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

        void preserveFile(const std::filesystem::path& path)
        {
            std::optional<std::string> contents;
            if (std::filesystem::exists(path))
            {
                std::ifstream file(path, std::ios::binary);
                contents = std::string(std::istreambuf_iterator<char>(file),
                                       std::istreambuf_iterator<char>());
            }
            savedFiles.emplace_back(path, std::move(contents));
        }

        std::filesystem::path root;
        std::vector<std::pair<std::filesystem::path, std::optional<std::string>>> savedFiles;
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
    NX::SceneManager restored;
    restored.importScenes(root);
    restored.deserialize(tabs);
    auto* closed = restored.getScene("Default"_atom);
    ASSERT_NE(closed, nullptr);
    EXPECT_FALSE(restored.isSceneOpen(closed));
    EXPECT_FALSE(closed->isInitialized());
    ASSERT_EQ(closed->getObjects().size(), 1);
    EXPECT_EQ(closed->getObjects()[0]->getComponentName(), "Retained"_atom);
    EXPECT_EQ(closed->getObjects()[0]->getPosition(), glm::vec3(12.f, 34.f, 0.f));
    EXPECT_FALSE(closed->getObjects()[0]->isInitialized());
    ASSERT_TRUE(restored.setCurrentScene(closed));
    EXPECT_TRUE(closed->isInitialized());
    EXPECT_TRUE(closed->getObjects()[0]->isInitialized());
    EXPECT_EQ(restored.getCurrentScene(), closed);
}

TEST_F(ScenePersistenceTest, ImportsExistingSceneFilesWithoutDeletingThem)
{
    const NX::Scene first;
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
    manager.deserialize(tabs);
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
    EXPECT_THROW(manager.deserialize(tabs), std::exception);
    EXPECT_EQ(manager.getCurrentScene(), initial);
    EXPECT_EQ(manager.getOpenScenes().size(), 1u);
}

TEST_F(ScenePersistenceTest, RestoringTabsClosesAndReopensSceneComponents)
{
    NX::SceneManager manager;
    auto* initial = manager.getCurrentScene();
    auto object = NX::SceneObject::Create();
    initial->addObjectToScene(object);
    auto& added = manager.createNewScene();
    ASSERT_TRUE(manager.setCurrentScene(&added));
    const auto tabs = nlohmann::json{ { "formatVersion", 1 },
                                      { "open", { added.getSceneName().c_str() } },
                                      { "current", added.getSceneName().c_str() } };
    manager.deserialize(tabs);
    EXPECT_FALSE(initial->isInitialized());
    EXPECT_FALSE(object->isInitialized());
    EXPECT_TRUE(added.isInitialized());
    ASSERT_TRUE(manager.setCurrentScene(initial));
    EXPECT_TRUE(initial->isInitialized());
    EXPECT_TRUE(object->isInitialized());
}

TEST_F(ScenePersistenceTest, StreamAndJsonRestoreTheSameTabs)
{
    NX::SceneManager fromJson;
    NX::SceneManager fromStream;
    auto& jsonScene = fromJson.createNewScene();
    auto& streamScene = fromStream.createNewScene();
    ASSERT_EQ(jsonScene.getSceneName(), streamScene.getSceneName());
    const nlohmann::json tabs = { { "formatVersion", 1 },
                                  { "open", { jsonScene.getSceneName().c_str() } },
                                  { "current", jsonScene.getSceneName().c_str() } };
    fromJson.deserialize(tabs);
    auto stream = RResourceStream<RJsonResourceStream>(tabs);
    fromStream.deserialize(stream);
    EXPECT_EQ(fromJson.serialize(), tabs);
    EXPECT_EQ(fromStream.serialize(), fromJson.serialize());
    EXPECT_FALSE(fromJson.getScene("Default"_atom)->isInitialized());
    EXPECT_FALSE(fromStream.getScene("Default"_atom)->isInitialized());
    EXPECT_TRUE(jsonScene.isInitialized());
    EXPECT_TRUE(streamScene.isInitialized());
}

TEST_F(ScenePersistenceTest, SceneOwnsCameraSelectionWithoutGameInstanceInitialization)
{
    auto& manager = gGameInstance->scenes;
    auto* initial = manager.getCurrentScene();
    auto first = NX::SceneObject::Create();
    auto* firstCamera = first->addChildComponent<NX::OrthographicCamera>("FirstCamera"_atom);
    initial->addObjectToScene(first);
    EXPECT_EQ(gGameInstance->world.currentCamera, firstCamera);

    auto& next = manager.createNewScene();
    auto second = NX::SceneObject::Create();
    auto* secondCamera = second->addChildComponent<NX::OrthographicCamera>("SecondCamera"_atom);
    next.addObjectToScene(second);
    EXPECT_EQ(gGameInstance->world.currentCamera, firstCamera);
    ASSERT_TRUE(manager.setCurrentScene(&next));
    EXPECT_EQ(gGameInstance->world.currentCamera, secondCamera);
    ASSERT_TRUE(manager.removeScene(&next));
    EXPECT_EQ(gGameInstance->world.currentCamera, firstCamera);
}

TEST_F(ScenePersistenceTest, ReloadingCurrentSceneSelectsReplacementCamera)
{
    auto* scene = gGameInstance->scenes.getCurrentScene();
    auto original = NX::SceneObject::Create();
    auto* oldCamera = original->addChildComponent<NX::OrthographicCamera>("OriginalCamera"_atom);
    scene->addObjectToScene(original);
    ASSERT_EQ(gGameInstance->world.currentCamera, oldCamera);
    const auto saved = scene->serialize();
    auto stream = RResourceStream<RJsonResourceStream>(saved);
    scene->deserialize(stream);
    ASSERT_NE(gGameInstance->world.currentCamera, nullptr);
    EXPECT_NE(gGameInstance->world.currentCamera, oldCamera);
    EXPECT_EQ(gGameInstance->world.currentCamera,
              scene->getObjects().front()->findFirstChildOf<NX::OrthographicCamera>());

    auto empty = RResourceStream<RJsonResourceStream>(NX::Scene{}.serialize());
    scene->deserialize(empty);
    EXPECT_EQ(gGameInstance->world.currentCamera, nullptr);
}

TEST_F(ScenePersistenceTest, DuplicateImportNamesIdentifyNameAndFileAndCanRecover)
{
    NX::Scene source;
    source.setSceneName("Duplicated"_atom);
    std::ofstream(root / "First.json") << source.serialize().dump();
    std::ofstream(root / "Second.json") << source.serialize().dump();
    NX::SceneManager manager;
    auto* initial = manager.getCurrentScene();
    try
    {
        manager.importScenes(root);
        FAIL() << "Expected duplicate scene names to fail import";
    }
    catch (const std::runtime_error& error)
    {
        const std::string message = error.what();
        EXPECT_NE(message.find("Duplicated"), std::string::npos);
        EXPECT_NE(message.find((root / "Second.json").generic_string()), std::string::npos);
    }
    EXPECT_TRUE(manager.hasImportError());
    EXPECT_EQ(manager.getCurrentScene(), initial);
    std::filesystem::remove(root / "Second.json");
    ASSERT_NO_THROW(manager.importScenes(root));
    EXPECT_FALSE(manager.hasImportError());
    EXPECT_NE(manager.getScene("Duplicated"_atom), nullptr);
}

TEST_F(ScenePersistenceTest, InvalidReflectedTabFieldsPreserveCurrentSession)
{
    NX::SceneManager manager;
    auto* initial = manager.getCurrentScene();
    const auto valid = manager.serialize();
    for (const auto* field : { "formatVersion", "open", "current" })
    {
        auto missing = valid;
        missing.erase(field);
        EXPECT_THROW(manager.deserialize(missing), std::exception);
        EXPECT_EQ(manager.getCurrentScene(), initial);
        EXPECT_EQ(manager.serialize(), valid);
    }
    for (const auto& invalid :
         { nlohmann::json(1.5), nlohmann::json("1"), nlohmann::json(4'294'967'297ULL) })
    {
        auto tabs = valid;
        tabs["formatVersion"] = invalid;
        EXPECT_THROW(manager.deserialize(tabs), std::exception);
        EXPECT_EQ(manager.serialize(), valid);
    }
    for (const auto* field : { "open", "current" })
    {
        auto tabs = valid;
        tabs[field] = 42;
        EXPECT_THROW(manager.deserialize(tabs), std::exception);
        EXPECT_EQ(manager.serialize(), valid);
    }
}

TEST_F(ScenePersistenceTest, InvalidObjectReferencesAndNamesPreserveAuthoredScene)
{
    NX::Scene scene;
    auto object = NX::SceneObject::Create();
    object->setComponentName("Kept"_atom);
    scene.addObjectToScene(object);
    const auto saved = scene.serialize();
    auto duplicate = saved;
    duplicate["sceneObjects"].push_back(duplicate["sceneObjects"].front());
    auto emptyName = saved;
    emptyName["sceneObjects"][0]["name"] = "";
    auto missingObject = saved;
    missingObject["sceneObjects"][0].erase("componentData");
    for (const auto& invalid : { duplicate, emptyName, missingObject })
    {
        auto stream = RResourceStream<RJsonResourceStream>(invalid);
        EXPECT_THROW(scene.deserialize(stream), std::runtime_error);
        EXPECT_EQ(scene.serialize(), saved);
        EXPECT_EQ(scene.getObjects().front().get(), object.get());
    }
}

TEST_F(ScenePersistenceTest, ExplicitSceneSaveRecoversFailedImportAndReenablesAutosave)
{
    NX::SceneManager manager;
    auto* scene = manager.getCurrentScene();
    scene->setSceneName(Core::StringAtom::Intern(root.filename().generic_string()));
    const auto sceneFile = Foundation::Config::Path::data / "scenes"
                           / (scene->getSceneName().toStdString() + ".json");
    preserveFile(sceneFile);
    preserveFile(Foundation::Config::Path::data / "cache" / "SceneTabs.json");
    std::filesystem::create_directories(sceneFile.parent_path());
    NX::WriteFileAtomically(sceneFile, "unreadable scene");
    std::ofstream(root / "Broken.json") << "unreadable scene";
    ASSERT_THROW(manager.importScenes(root), std::exception);
    EXPECT_TRUE(manager.hasImportError());
    EXPECT_FALSE(manager.writeToCache());
    {
        std::ifstream file(sceneFile);
        const std::string contents{ std::istreambuf_iterator<char>(file),
                                    std::istreambuf_iterator<char>() };
        EXPECT_EQ(contents, "unreadable scene");
    }
    ASSERT_TRUE(manager.writeToCache(NX::CacheWriteMode::Explicit));
    EXPECT_FALSE(manager.hasImportError());
    scene->addObjectToScene(NX::SceneObject::Create());
    ASSERT_TRUE(manager.writeToCache());
    std::ifstream file(sceneFile);
    EXPECT_EQ(nlohmann::json::parse(file).at("sceneObjects").size(), 1u);
}
