// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/Windows/Scenes/SceneTabs/SceneTabsCache.h"
#include "Foundation/Configs.h"
#include "NxFundamental/ResourceManagement/AtomicFile.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Framework/World.h"
#include "NxWorld/Scene/Scene.h"
#include "Runtime/SceneFilesGuard.h"

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

        [[nodiscard]] static std::string readFile(const std::filesystem::path& path)
        {
            std::ifstream file(path, std::ios::binary);
            return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
        }

        [[nodiscard]] static std::filesystem::path sceneFile(const std::string& name)
        {
            return Foundation::Config::Path::data / "scenes" / (name + ".json");
        }

        // Unique per test run, so it never matches a real scene file.
        [[nodiscard]] std::string uniqueName(const std::string& suffix) const
        {
            return root.filename().generic_string() + "_" + suffix;
        }

        SceneFilesGuard sceneFiles;
        std::filesystem::path root;
        std::vector<std::pair<std::filesystem::path, std::optional<std::string>>> savedFiles;
    };
} // namespace

TEST_F(ScenePersistenceTest, NewScenesSkipNamesOfUnloadedSceneFiles)
{
    NX::SceneManager manager;
    const auto unloaded = sceneFile("Scene_1");
    preserveFile(unloaded);
    std::filesystem::create_directories(unloaded.parent_path());
    NX::WriteFileAtomically(unloaded, "unloaded scene");

    const auto& added = manager.createNewScene();

    EXPECT_NE(added.getSceneName(), "Scene_1"_atom);
    EXPECT_EQ(readFile(unloaded), "unloaded scene");
    EXPECT_TRUE(std::filesystem::exists(sceneFile(added.getSceneName().toStdString())));
}

TEST_F(ScenePersistenceTest, RenameDoesNotReplaceUnloadedSceneFiles)
{
    NX::SceneManager manager;
    auto& added = manager.createNewScene();
    const auto oldName = added.getSceneName();
    const auto taken = uniqueName("Taken");
    const auto unloaded = sceneFile(taken);
    preserveFile(unloaded);
    NX::WriteFileAtomically(unloaded, "unloaded scene");

    EXPECT_FALSE(manager.renameScene(&added, Core::StringAtom::Intern(taken)));

    EXPECT_EQ(added.getSceneName(), oldName);
    EXPECT_EQ(readFile(unloaded), "unloaded scene");
    EXPECT_TRUE(std::filesystem::exists(sceneFile(oldName.toStdString())));
}

TEST_F(ScenePersistenceTest, SceneWithoutOwnFileNeverDeletesTheFileMatchingItsName)
{
    const auto name = Core::StringAtom::Intern(uniqueName("Unloaded"));
    const auto unloaded = sceneFile(name.toStdString());
    preserveFile(unloaded);
    NX::WriteFileAtomically(unloaded, "unloaded scene");
    {
        // Mirrors the placeholder scene kept after a failed import.
        NX::SceneManager manager;
        auto* placeholder = manager.getCurrentScene();
        placeholder->setSceneName(name);
        ASSERT_TRUE(
            manager.renameScene(placeholder, Core::StringAtom::Intern(uniqueName("Renamed"))));
        EXPECT_EQ(readFile(unloaded), "unloaded scene");
    }
    {
        NX::SceneManager manager;
        auto* placeholder = manager.getCurrentScene();
        placeholder->setSceneName(name);
        manager.createNewScene();
        ASSERT_TRUE(manager.removeScene(placeholder));
        EXPECT_EQ(readFile(unloaded), "unloaded scene");
    }
}

TEST_F(ScenePersistenceTest, RenameMovesOwnedSceneFileIncludingToTheSamePath)
{
    NX::SceneManager manager;
    auto& added = manager.createNewScene();
    const auto oldFile = sceneFile(added.getSceneName().toStdString());
    const auto spaced = uniqueName("Main Menu");
    const auto underscored = uniqueName("Main_Menu");

    ASSERT_TRUE(manager.renameScene(&added, Core::StringAtom::Intern(spaced)));
    EXPECT_FALSE(std::filesystem::exists(oldFile));
    EXPECT_TRUE(std::filesystem::exists(sceneFile(underscored)));

    // Both names share one file, which already belongs to this scene.
    ASSERT_TRUE(manager.renameScene(&added, Core::StringAtom::Intern(underscored)));
    EXPECT_TRUE(std::filesystem::exists(sceneFile(underscored)));
}

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
    const auto tabs = NX::SceneTabsCache(manager).serialize();
    EXPECT_FALSE(tabs.contains("scenes"));
    EXPECT_EQ(tabs.at("current"), added.getSceneName().c_str());
    for (const auto& scene : manager.getScenes())
    {
        std::ofstream(root / (std::string(scene->getSceneName().c_str()) + ".json"))
            << scene->serialize().dump();
    }
    NX::SceneManager restored;
    restored.importScenes(root);
    NX::SceneTabsCache(restored).deserialize(tabs);
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

TEST_F(ScenePersistenceTest, ImportSkipsBrokenSceneFilesWithoutDeletingAnything)
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
    EXPECT_FALSE(manager.hasImportError());
    std::ofstream(root / "Broken.json") << "invalid JSON";
    ASSERT_NO_THROW(manager.importScenes(root));
    EXPECT_TRUE(manager.hasImportError());
    EXPECT_EQ(manager.getSkippedSceneFiles(), std::vector{ root / "Broken.json" });
    EXPECT_EQ(manager.getScenes().size(), 2);
    EXPECT_NE(manager.getScene("Default"_atom), nullptr);
    EXPECT_NE(manager.getScene("Another"_atom), nullptr);
    EXPECT_TRUE(std::filesystem::exists(root / "Broken.json"));
}

TEST_F(ScenePersistenceTest, OnlyBrokenSceneFilesKeepTheCurrentScenes)
{
    std::ofstream(root / "Broken.json") << "invalid JSON";
    NX::SceneManager manager;
    auto* current = manager.getCurrentScene();
    ASSERT_NO_THROW(manager.importScenes(root));
    EXPECT_TRUE(manager.hasImportError());
    EXPECT_EQ(manager.getCurrentScene(), current);
}

TEST_F(ScenePersistenceTest, MissingSceneReferencesDoNotDiscardLoadedScenes)
{
    NX::SceneManager manager;
    auto* initial = manager.getCurrentScene();
    const nlohmann::json tabs = { { "formatVersion", 1 },
                                  { "open", { "DeletedScene", "Default" } },
                                  { "current", "DeletedScene" } };
    NX::SceneTabsCache(manager).deserialize(tabs);
    EXPECT_EQ(manager.getCurrentScene(), initial);
    ASSERT_EQ(manager.getOpenScenes().size(), 1u);
    EXPECT_EQ(manager.getOpenScenes().front(), initial);
}

TEST_F(ScenePersistenceTest, RestoringTabsSelectsTheStartSceneEvenWhenAllTabsAreOpen)
{
    NX::World world;
    NX::SceneManager manager{ &world };
    auto* first = manager.getCurrentScene();
    auto& start = manager.createNewScene();
    auto& last = manager.createNewScene();
    world.startScene = start.getSceneName();
    const auto tabs
        = nlohmann::json{ { "formatVersion", 1 },
                          { "open",
                            { first->getSceneName().c_str(), start.getSceneName().c_str(),
                              last.getSceneName().c_str() } },
                          { "current", last.getSceneName().c_str() } };

    NX::SceneTabsCache(manager).deserialize(tabs);

    EXPECT_EQ(manager.getCurrentScene(), &start);
    EXPECT_EQ(manager.getOpenScenes(), (std::vector<NX::Scene*>{ first, &start, &last }));
}

TEST_F(ScenePersistenceTest, RestoringTabsOpensTheStartSceneWhenItWasClosed)
{
    NX::World world;
    NX::SceneManager manager{ &world };
    auto* first = manager.getCurrentScene();
    auto& start = manager.createNewScene();
    world.startScene = start.getSceneName();
    const auto tabs = nlohmann::json{ { "formatVersion", 1 },
                                      { "open", { first->getSceneName().c_str() } },
                                      { "current", first->getSceneName().c_str() } };

    NX::SceneTabsCache(manager).deserialize(tabs);

    EXPECT_EQ(manager.getCurrentScene(), &start);
    EXPECT_TRUE(manager.isSceneOpen(first));
    EXPECT_TRUE(manager.isSceneOpen(&start));
}

TEST_F(ScenePersistenceTest, RestoringTabsKeepsRememberedSelectionWithoutStartScene)
{
    NX::World world;
    NX::SceneManager manager{ &world };
    auto* first = manager.getCurrentScene();
    auto& last = manager.createNewScene();
    world.startScene = "missing"_atom;
    const auto tabs
        = nlohmann::json{ { "formatVersion", 1 },
                          { "open",
                            { first->getSceneName().c_str(), last.getSceneName().c_str() } },
                          { "current", last.getSceneName().c_str() } };

    NX::SceneTabsCache(manager).deserialize(tabs);

    EXPECT_EQ(manager.getCurrentScene(), &last);
}

TEST_F(ScenePersistenceTest, MalformedTabsPreserveCurrentSession)
{
    NX::SceneManager manager;
    auto* initial = manager.getCurrentScene();
    const nlohmann::json tabs
        = { { "formatVersion", 1 }, { "open", { "Default", 42 } }, { "current", "Default" } };
    EXPECT_THROW(NX::SceneTabsCache(manager).deserialize(tabs), std::exception);
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
    NX::SceneTabsCache(manager).deserialize(tabs);
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
    // The second manager cannot take the first one's file, so align the names in memory.
    streamScene.setSceneName(jsonScene.getSceneName());
    const nlohmann::json tabs = { { "formatVersion", 1 },
                                  { "open", { jsonScene.getSceneName().c_str() } },
                                  { "current", jsonScene.getSceneName().c_str() } };
    NX::SceneTabsCache(fromJson).deserialize(tabs);
    auto stream = RResourceStream<RJsonResourceStream>(tabs);
    NX::SceneTabsCache(fromStream).deserialize(stream);
    EXPECT_EQ(NX::SceneTabsCache(fromJson).serialize(), tabs);
    EXPECT_EQ(NX::SceneTabsCache(fromStream).serialize(), NX::SceneTabsCache(fromJson).serialize());
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

TEST_F(ScenePersistenceTest, DuplicateImportNamesPreferTheFileNamedAfterTheScene)
{
    NX::Scene original;
    original.setSceneName("Duplicated"_atom);
    NX::Scene copy;
    copy.setSceneName("Duplicated"_atom);
    copy.addObjectToScene(NX::SceneObject::Create());
    // The copy sorts before the original file.
    std::ofstream(root / "Duplicated copy.json") << copy.serialize().dump();
    std::ofstream(root / "Duplicated.json") << original.serialize().dump();
    NX::SceneManager manager;
    ASSERT_NO_THROW(manager.importScenes(root));
    EXPECT_TRUE(manager.hasImportError());
    EXPECT_EQ(manager.getSkippedSceneFiles(), std::vector{ root / "Duplicated copy.json" });
    ASSERT_EQ(manager.getScenes().size(), 1);
    EXPECT_TRUE(manager.getScene("Duplicated"_atom)->getObjects().empty());
}

TEST_F(ScenePersistenceTest, SceneRestoresItsMainCameraAfterSwitchingScenes)
{
    auto& manager = gGameInstance->scenes;
    auto& world = gGameInstance->world;
    auto* initial = manager.getCurrentScene();
    auto first = NX::SceneObject::Create();
    auto* firstCamera = first->addChildComponent<NX::OrthographicCamera>("FirstCamera"_atom);
    auto second = NX::SceneObject::Create();
    auto* secondCamera = second->addChildComponent<NX::OrthographicCamera>("SecondCamera"_atom);
    initial->addObjectToScene(first);
    initial->addObjectToScene(second);
    ASSERT_EQ(world.currentCamera, firstCamera);

    initial->setMainCamera(secondCamera);
    EXPECT_EQ(world.currentCamera, secondCamera);
    auto& next = manager.createNewScene();
    ASSERT_TRUE(manager.setCurrentScene(&next));
    EXPECT_EQ(world.currentCamera, nullptr);
    ASSERT_TRUE(manager.setCurrentScene(initial));
    EXPECT_EQ(world.currentCamera, secondCamera);

    // A camera chosen directly through the world is remembered too.
    world.currentCamera = firstCamera;
    ASSERT_TRUE(manager.setCurrentScene(&next));
    ASSERT_TRUE(manager.setCurrentScene(initial));
    EXPECT_EQ(world.currentCamera, firstCamera);
}

TEST_F(ScenePersistenceTest, MainCameraFallsBackWhenRemovedAndRejectsForeignCameras)
{
    auto& manager = gGameInstance->scenes;
    auto& world = gGameInstance->world;
    auto* initial = manager.getCurrentScene();
    auto first = NX::SceneObject::Create();
    auto* firstCamera = first->addChildComponent<NX::OrthographicCamera>("FirstCamera"_atom);
    auto second = NX::SceneObject::Create();
    auto* secondCamera = second->addChildComponent<NX::OrthographicCamera>("SecondCamera"_atom);
    initial->addObjectToScene(first);
    initial->addObjectToScene(second);
    auto foreign = NX::SceneObject::Create();
    auto* foreignCamera = foreign->addChildComponent<NX::OrthographicCamera>("Foreign"_atom);

    initial->setMainCamera(foreignCamera);
    EXPECT_EQ(world.currentCamera, firstCamera);

    initial->setMainCamera(secondCamera);
    ASSERT_TRUE(initial->deleteFromScene(second.get()));
    auto& next = manager.createNewScene();
    ASSERT_TRUE(manager.setCurrentScene(&next));
    ASSERT_TRUE(manager.setCurrentScene(initial));
    EXPECT_EQ(world.currentCamera, firstCamera);
}

TEST_F(ScenePersistenceTest, DuplicateImportNamesSkipTheLaterFileAndCanRecover)
{
    NX::Scene source;
    source.setSceneName("Duplicated"_atom);
    std::ofstream(root / "First.json") << source.serialize().dump();
    std::ofstream(root / "Second.json") << source.serialize().dump();
    NX::SceneManager manager;
    ASSERT_NO_THROW(manager.importScenes(root));
    EXPECT_TRUE(manager.hasImportError());
    EXPECT_EQ(manager.getSkippedSceneFiles(), std::vector{ root / "Second.json" });
    EXPECT_EQ(manager.getScenes().size(), 1);
    EXPECT_NE(manager.getScene("Duplicated"_atom), nullptr);
    std::filesystem::remove(root / "Second.json");
    ASSERT_NO_THROW(manager.importScenes(root));
    EXPECT_FALSE(manager.hasImportError());
    EXPECT_NE(manager.getScene("Duplicated"_atom), nullptr);
}

TEST_F(ScenePersistenceTest, InvalidReflectedTabFieldsPreserveCurrentSession)
{
    NX::SceneManager manager;
    auto* initial = manager.getCurrentScene();
    const auto valid = NX::SceneTabsCache(manager).serialize();
    for (const auto* field : { "formatVersion", "open", "current" })
    {
        auto missing = valid;
        missing.erase(field);
        EXPECT_THROW(NX::SceneTabsCache(manager).deserialize(missing), std::exception);
        EXPECT_EQ(manager.getCurrentScene(), initial);
        EXPECT_EQ(NX::SceneTabsCache(manager).serialize(), valid);
    }
    for (const auto& invalid :
         { nlohmann::json(1.5), nlohmann::json("1"), nlohmann::json(4'294'967'297ULL) })
    {
        auto tabs = valid;
        tabs["formatVersion"] = invalid;
        EXPECT_THROW(NX::SceneTabsCache(manager).deserialize(tabs), std::exception);
        EXPECT_EQ(NX::SceneTabsCache(manager).serialize(), valid);
    }
    for (const auto* field : { "open", "current" })
    {
        auto tabs = valid;
        tabs[field] = 42;
        EXPECT_THROW(NX::SceneTabsCache(manager).deserialize(tabs), std::exception);
        EXPECT_EQ(NX::SceneTabsCache(manager).serialize(), valid);
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

TEST_F(ScenePersistenceTest, SavesWorkAfterSkippedFilesButNeverOverwriteUnloadedFiles)
{
    const auto good = Core::StringAtom::Intern(uniqueName("Good"));
    NX::Scene source;
    source.setSceneName(good);
    std::ofstream(root / "Good.json") << source.serialize().dump();
    std::ofstream(root / "Broken.json") << "unreadable scene";
    NX::SceneManager manager;
    ASSERT_NO_THROW(manager.importScenes(root));
    EXPECT_TRUE(manager.hasImportError());
    auto* loaded = manager.getScene(good);
    ASSERT_NE(loaded, nullptr);

    // A file this manager never loaded sits at the loaded scene's save path.
    const auto unloaded = sceneFile(good.toStdString());
    preserveFile(unloaded);
    preserveFile(Foundation::Config::Path::data / "cache" / "SceneTabs.json");
    NX::WriteFileAtomically(unloaded, "unloaded scene");
    EXPECT_FALSE(manager.writeToCache());
    EXPECT_FALSE(manager.writeToCache(NX::CacheWriteMode::Explicit));
    EXPECT_EQ(readFile(unloaded), "unloaded scene");

    std::filesystem::remove(unloaded);
    loaded->addObjectToScene(NX::SceneObject::Create());
    ASSERT_TRUE(manager.writeToCache());
    EXPECT_EQ(nlohmann::json::parse(readFile(unloaded)).at("sceneObjects").size(), 1u);
    EXPECT_EQ(readFile(root / "Broken.json"), "unreadable scene");
}
