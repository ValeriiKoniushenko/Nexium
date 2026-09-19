// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Foundation/Configs.h"
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Scene/Rectangle.h"
#include "NxWorld/Scene/Scene.h"

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
    json["sceneObjects"][1]["referenceAsset"] = "missing.nx";
    auto data = RResourceStream<RJsonResourceStream>(json);
    EXPECT_THROW(source.deserialize(data), std::runtime_error);
    EXPECT_EQ(source.getSceneName(), "Default"_atom);
    ASSERT_EQ(source.getObjects().size(), 1u);
    EXPECT_EQ(source.getObjects()[0].get(), object.get());
}

TEST_F(ScenePersistenceTest, RejectsMissingAndUnsupportedVersions)
{
    NX::Scene scene;
    scene.addObjectToScene(NX::SceneObject::Create());
    const auto original = scene.serialize();
    for (const auto& version : { nlohmann::json(), nlohmann::json(0), nlohmann::json(999) })
    {
        auto json = original;
        json["formatVersion"] = version;
        auto data = RResourceStream<RJsonResourceStream>(json);
        EXPECT_THROW(scene.deserialize(data), std::runtime_error);
        EXPECT_EQ(scene.serialize(), original);
    }
    auto json = original;
    json.erase("formatVersion");
    auto data = RResourceStream<RJsonResourceStream>(json);
    EXPECT_THROW(scene.deserialize(data), std::runtime_error);
    EXPECT_EQ(scene.serialize(), original);
}

TEST_F(ScenePersistenceTest, RectangleStateSurvivesRepeatedLoading)
{
    NX::Scene source;
    auto object = NX::SceneObj::RectangleAnimated::Create();
    object->setComponentName("Tree"_atom);
    object->setTexture("default"_atom, "Tree_2.png"_atom);
    object->setPosition({ 356.f, 0.f, 0.f });
    object->setAnimationEnabled(false);
    source.addObjectToScene(object);
    auto saved = RResourceStream<RJsonResourceStream>(source.serialize());
    NX::Scene restored;
    restored.deserialize(saved);
    restored.deserialize(saved);
    EXPECT_EQ(restored.serialize(), source.serialize());
    ASSERT_EQ(restored.getObjects().size(), 1u);
    EXPECT_EQ(nlohmann::json(restored.getObjects()[0]->getSceneState()),
              nlohmann::json(object->getSceneState()));

    saved.getData()["sceneObjects"][0].erase("componentData");
    EXPECT_THROW(restored.deserialize(saved), std::runtime_error);
    EXPECT_EQ(restored.serialize(), source.serialize());
}

TEST_F(ScenePersistenceTest, DefaultSceneInlineObjectsUseCurrentFormat)
{
    std::ifstream file(Foundation::Config::Path::data / "scenes/Default.json");
    auto json = nlohmann::json::parse(file);
    ASSERT_EQ(json.at("formatVersion"), 1);
    auto& objects = json.at("sceneObjects");
    // Asset-backed game types are registered by TemplateGame, not the unit-test binary.
    auto& entries = objects.get_ref<nlohmann::json::array_t&>();
    std::erase_if(entries, [](const auto& object)
                  { return !object.at("referenceAsset").template get<std::string>().empty(); });
    ASSERT_FALSE(objects.empty());
    for (const auto& object : objects)
    {
        ASSERT_TRUE(object.contains("componentData"));
    }
    auto data = RResourceStream<RJsonResourceStream>(json);
    NX::Scene restored;
    restored.deserialize(data);
    ASSERT_EQ(restored.getObjects().size(), objects.size());
    for (std::size_t i = 0; i < objects.size(); ++i)
    {
        auto expected = objects[i];
        expected.erase("componentData");
        EXPECT_EQ(nlohmann::json(restored.getObjects()[i]->getSceneState()), expected);
    }
    const auto saved = restored.serialize();
    auto reload = RResourceStream<RJsonResourceStream>(saved);
    restored.deserialize(reload);
    EXPECT_EQ(restored.serialize(), saved);
}

TEST_F(ScenePersistenceTest, ObjectsAddedDuringLoadDoNotInvalidateInitialization)
{
    NX::Scene source;
    auto first = NX::SceneObject::Create();
    first->setComponentName("First"_atom);
    source.addObjectToScene(first);
    auto second = NX::SceneObject::Create();
    second->setComponentName("Second"_atom);
    source.addObjectToScene(second);
    auto data = RResourceStream<RJsonResourceStream>(source.serialize());
    NX::Scene restored;
    int added = 0;
    const auto subscription = restored.onObjectAdded->subscribeAndGetID(
        [&](NX::SceneObject* object)
        {
            ++added;
            if (object->getComponentName() == "First"_atom)
            {
                for (int i = 0; i < 128; ++i)
                {
                    restored.addObjectToScene(NX::SceneObject::Create());
                }
            }
        });
    restored.deserialize(data);
    EXPECT_EQ(restored.getObjects().size(), 130u);
    EXPECT_EQ(added, 130);
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
