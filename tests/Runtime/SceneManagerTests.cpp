// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Scene/Scene/SceneManager.h"

#include "gtest/gtest.h"

TEST(SceneManagerTests, WeakSceneReferenceExpiresAfterRemoval)
{
    NX::SceneManager manager;
    Core::WeakPtr<NX::Scene> pending = &manager.createNewScene();
    ASSERT_TRUE(pending.tryLoad());
    manager.removeScene(pending.get());
    EXPECT_FALSE(pending);
    EXPECT_FALSE(pending.tryLoad());
}

TEST(SceneManagerTests, LoadedSceneSurvivesRemovalUntilReleased)
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

TEST(SceneManagerTests, KeepsCurrentSceneStableWhenAddingScenes)
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

TEST(SceneManagerTests, SwitchesAndRemovesScenesWithoutBecomingEmpty)
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

TEST(SceneManagerTests, RemovesInactiveSceneAndResetsToDefault)
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

namespace
{
    class SceneLifecycleObject : public NX::SceneObject
    {
    public:
        int initializationCount = 0;
        int deinitializationCount = 0;

    protected:
        void onInitialize() override
        {
            SceneObject::onInitialize();
            ++initializationCount;
        }
        void onDeinitialize() override
        {
            SceneObject::onDeinitialize();
            ++deinitializationCount;
        }
    };
} // namespace

TEST(SceneManagerTests, InitializesOnOpenAndDeinitializesOnClose)
{
    NX::SceneManager manager;
    auto& scene = manager.createNewScene();
    Core::IntrusivePtr<SceneLifecycleObject> object = new SceneLifecycleObject;
    scene.addObjectToScene(object);
    EXPECT_FALSE(scene.isInitialized());
    EXPECT_FALSE(object->isInitialized());
    EXPECT_EQ(object->initializationCount, 0);

    ASSERT_TRUE(manager.setCurrentScene(&scene));
    EXPECT_TRUE(scene.isInitialized());
    EXPECT_TRUE(object->isInitialized());
    EXPECT_EQ(object->initializationCount, 1);
    ASSERT_TRUE(manager.setCurrentScene(&scene));
    EXPECT_EQ(object->initializationCount, 1);

    ASSERT_TRUE(manager.closeScene(&scene));
    EXPECT_FALSE(scene.isInitialized());
    EXPECT_FALSE(object->isInitialized());
    EXPECT_EQ(object->deinitializationCount, 1);
    EXPECT_EQ(scene.getObjects().size(), 1);
    ASSERT_TRUE(manager.setCurrentScene(&scene));
    EXPECT_TRUE(object->isInitialized());
    EXPECT_EQ(object->initializationCount, 2);
}

TEST(SceneManagerTests, DeinitializesRetainedSceneOnRemovalAndReset)
{
    NX::SceneManager manager;
    Core::IntrusivePtr<NX::Scene> scene = &manager.createNewScene();
    Core::IntrusivePtr<SceneLifecycleObject> object = new SceneLifecycleObject;
    scene->addObjectToScene(object);
    ASSERT_TRUE(manager.setCurrentScene(scene.get()));
    manager.removeScene(scene.get());
    EXPECT_FALSE(scene->isInitialized());
    EXPECT_FALSE(object->isInitialized());
    EXPECT_EQ(object->deinitializationCount, 1);

    scene = manager.getCurrentScene();
    scene->addObjectToScene(object);
    manager.removeAllScenes();
    EXPECT_FALSE(scene->isInitialized());
    EXPECT_FALSE(object->isInitialized());
    EXPECT_EQ(object->deinitializationCount, 2);
    EXPECT_TRUE(manager.getCurrentScene()->isInitialized());
}

TEST(SceneManagerTests, ShutdownDeinitializesScenesRetainedElsewhere)
{
    Core::IntrusivePtr<NX::Scene> scene;
    Core::IntrusivePtr<SceneLifecycleObject> object = new SceneLifecycleObject;
    {
        NX::SceneManager manager;
        scene = manager.getCurrentScene();
        scene->addObjectToScene(object);
        ASSERT_TRUE(object->isInitialized());
    }
    EXPECT_FALSE(scene->isInitialized());
    EXPECT_FALSE(object->isInitialized());
    EXPECT_EQ(object->deinitializationCount, 1);
}
