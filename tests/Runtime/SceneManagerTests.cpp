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
#include "NxWorld/Framework/World.h"
#include "NxWorld/Scene/SceneManager.h"
#include "Runtime/SceneFilesGuard.h"
#include "Runtime/TickActionComponent.h"

#include "gtest/gtest.h"

namespace
{
    class SceneManagerTests : public ::testing::Test
    {
    protected:
        SceneFilesGuard sceneFiles;
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
    EXPECT_FALSE(manager.removeScene("missing"_atom));
    EXPECT_EQ(manager.getCurrentScene(), &added);
    EXPECT_TRUE(manager.removeScene(name));
    EXPECT_EQ(manager.getScene(name), nullptr);
    EXPECT_EQ(manager.getCurrentScene(), initial);
    EXPECT_FALSE(manager.removeScene("Default"_atom));
    EXPECT_EQ(manager.getScenes().size(), 1);
    EXPECT_EQ(manager.getCurrentScene(), initial);
}

TEST_F(SceneManagerTests, ClosingCurrentSceneSelectsNextThenPreviousTab)
{
    NX::SceneManager manager;
    auto* first = manager.getCurrentScene();
    auto& middle = manager.createNewScene();
    auto& last = manager.createNewScene();
    ASSERT_TRUE(manager.setCurrentScene(&middle));
    ASSERT_TRUE(manager.setCurrentScene(&last));
    ASSERT_TRUE(manager.setCurrentScene(&middle));

    EXPECT_TRUE(manager.closeScene(&middle));
    EXPECT_EQ(manager.getCurrentScene(), &last);
    EXPECT_TRUE(manager.closeScene(&last));
    EXPECT_EQ(manager.getCurrentScene(), first);
    EXPECT_FALSE(manager.closeScene(first));
}

TEST_F(SceneManagerTests, RemovingCurrentSceneSelectsAdjacentOpenTab)
{
    NX::SceneManager manager;
    auto* first = manager.getCurrentScene();
    auto& middle = manager.createNewScene();
    auto& last = manager.createNewScene();
    ASSERT_TRUE(manager.setCurrentScene(&middle));
    ASSERT_TRUE(manager.setCurrentScene(&last));
    ASSERT_TRUE(manager.setCurrentScene(&middle));

    EXPECT_TRUE(manager.removeScene(&middle));
    EXPECT_EQ(manager.getCurrentScene(), &last);
    EXPECT_TRUE(manager.removeScene(&last));
    EXPECT_EQ(manager.getCurrentScene(), first);
    EXPECT_FALSE(manager.removeScene(nullptr));
    EXPECT_FALSE(manager.removeScene(first));
}

TEST_F(SceneManagerTests, RemovingOnlyOpenSceneOpensAnotherOwnedScene)
{
    NX::SceneManager manager;
    auto* first = manager.getCurrentScene();
    auto& replacement = manager.createNewScene();

    EXPECT_TRUE(manager.removeScene(first));
    EXPECT_EQ(manager.getCurrentScene(), &replacement);
    EXPECT_TRUE(replacement.isInitialized());
    ASSERT_EQ(manager.getOpenScenes().size(), 1);
    EXPECT_EQ(manager.getOpenScenes().front(), &replacement);
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
    manager.resetScenes();
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

TEST_F(SceneManagerTests, InitializesOnOpenAndDeinitializesOnClose)
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

TEST_F(SceneManagerTests, DeinitializesRetainedSceneOnRemovalAndReset)
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
    manager.resetScenes();
    EXPECT_FALSE(scene->isInitialized());
    EXPECT_FALSE(object->isInitialized());
    EXPECT_EQ(object->deinitializationCount, 2);
    EXPECT_TRUE(manager.getCurrentScene()->isInitialized());
}

TEST_F(SceneManagerTests, ShutdownDeinitializesScenesRetainedElsewhere)
{
    Core::IntrusivePtr<NX::Scene> scene;
    Core::IntrusivePtr<SceneLifecycleObject> object = new SceneLifecycleObject;
    {
        const NX::SceneManager manager;
        scene = manager.getCurrentScene();
        scene->addObjectToScene(object);
        ASSERT_TRUE(object->isInitialized());
    }
    EXPECT_FALSE(scene->isInitialized());
    EXPECT_FALSE(object->isInitialized());
    EXPECT_EQ(object->deinitializationCount, 1);
}

TEST_F(SceneManagerTests, HasSceneOnlyForOwnedScenes)
{
    NX::SceneManager manager;
    auto* initial = manager.getCurrentScene();
    const NX::Scene foreign;
    EXPECT_TRUE(manager.hasScene(initial));
    EXPECT_FALSE(manager.hasScene(nullptr));
    EXPECT_FALSE(manager.hasScene(&foreign));
    auto* added = &manager.createNewScene();
    EXPECT_TRUE(manager.hasScene(added));
    ASSERT_TRUE(manager.removeScene(added));
    EXPECT_FALSE(manager.hasScene(added));
}

namespace
{
    TickActionComponent* addTickAction(NX::Scene& scene)
    {
        auto object = NX::SceneObject::Create();
        auto* action = object->addChildComponent<TickActionComponent>("Action"_atom);
        scene.addObjectToScene(object);
        return action;
    }
} // namespace

TEST_F(SceneManagerTests, TickingSceneIsRemovedOnlyAfterItsTickFinishes)
{
    NX::SceneManager manager;
    auto* ticking = manager.getCurrentScene();
    auto* next = &manager.createNewScene();
    bool immediateResult = true;
    addTickAction(*ticking)->action = [&]
    {
        immediateResult = manager.removeScene(ticking);
        manager.requestRemoveScene(ticking);
    };

    manager.tickCurrentScene(0.f);
    EXPECT_FALSE(immediateResult);
    EXPECT_TRUE(manager.hasScene(ticking));
    EXPECT_EQ(manager.getCurrentScene(), ticking);
    EXPECT_TRUE(manager.hasPendingChanges());

    manager.applyPendingChanges();
    EXPECT_FALSE(manager.hasPendingChanges());
    EXPECT_FALSE(manager.hasScene(ticking));
    EXPECT_EQ(manager.getCurrentScene(), next);
}

TEST_F(SceneManagerTests, RequestsFromTickApplyInOrderAtTheEndOfTheFrame)
{
    // tag::scene_requests_from_tick[]
    NX::SceneManager manager;
    auto* menu = manager.getCurrentScene();
    auto* game = &manager.createNewScene();
    bool immediateResult = true;
    addTickAction(*menu)->action = [&]
    {
        immediateResult = manager.setCurrentScene(game) || manager.closeScene(menu);
        manager.requestCurrentScene(game->getSceneName());
        manager.requestCloseScene(menu);
    };

    manager.tickCurrentScene(0.f);
    EXPECT_FALSE(immediateResult);
    EXPECT_EQ(manager.getCurrentScene(), menu);
    EXPECT_TRUE(menu->isInitialized());

    manager.applyPendingChanges();
    EXPECT_EQ(manager.getCurrentScene(), game);
    EXPECT_FALSE(manager.isSceneOpen(menu));
    EXPECT_FALSE(menu->isInitialized());
    EXPECT_TRUE(manager.hasScene(menu));
    // end::scene_requests_from_tick[]
}

TEST_F(SceneManagerTests, RequestsForScenesRemovedMeanwhileAreIgnored)
{
    NX::SceneManager manager;
    auto* initial = manager.getCurrentScene();
    auto* removed = &manager.createNewScene();
    manager.requestCurrentScene(removed);
    manager.requestCurrentScene("missing"_atom);
    ASSERT_TRUE(manager.removeScene(removed));

    manager.applyPendingChanges();
    EXPECT_FALSE(manager.hasPendingChanges());
    EXPECT_EQ(manager.getCurrentScene(), initial);
}

TEST_F(SceneManagerTests, ActivationDrivesTheGivenWorldWithoutAGameInstance)
{
    ASSERT_EQ(gGameInstance, nullptr);
    NX::World world;
    NX::SceneManager manager{ &world };
    auto* first = manager.getCurrentScene();
    auto object = NX::SceneObject::Create();
    auto* camera = object->addChildComponent<NX::OrthographicCamera>("Camera"_atom);
    first->addObjectToScene(object);
    EXPECT_TRUE(first->isActive());
    EXPECT_EQ(world.currentCamera, camera);

    auto* second = &manager.createNewScene();
    auto other = NX::SceneObject::Create();
    auto* otherCamera = other->addChildComponent<NX::OrthographicCamera>("Other"_atom);
    second->addObjectToScene(other);
    EXPECT_FALSE(second->isActive());
    EXPECT_EQ(world.currentCamera, camera);

    ASSERT_TRUE(manager.setCurrentScene(second));
    EXPECT_FALSE(first->isActive());
    EXPECT_EQ(world.currentCamera, otherCamera);
    ASSERT_TRUE(manager.setCurrentScene(first));
    EXPECT_EQ(world.currentCamera, camera);
}

TEST_F(SceneManagerTests, OpensOnlyTheWorldStartSceneAndFallsBackToTheFirstScene)
{
    NX::World world;
    NX::SceneManager manager{ &world };
    auto* first = manager.getCurrentScene();
    auto* start = &manager.createNewScene();
    ASSERT_TRUE(manager.setCurrentScene(start));
    ASSERT_TRUE(manager.setCurrentScene(first));

    world.startScene = start->getSceneName();
    manager.openStartScene();
    EXPECT_EQ(manager.getCurrentScene(), start);
    EXPECT_EQ(manager.getOpenScenes(), std::vector<NX::Scene*>{ start });
    EXPECT_FALSE(first->isInitialized());

    world.startScene = "missing"_atom;
    manager.openStartScene();
    EXPECT_EQ(manager.getCurrentScene(), first);
    EXPECT_EQ(world.startScene, "missing"_atom);

    world.startScene = Core::StringAtom{};
    ASSERT_TRUE(manager.setCurrentScene(start));
    manager.openStartScene();
    EXPECT_EQ(manager.getCurrentScene(), first);
}

TEST_F(SceneManagerTests, StartSceneFollowsRenameAndIsClearedOnRemoval)
{
    NX::World world;
    NX::SceneManager manager{ &world };
    auto* start = &manager.createNewScene();
    world.startScene = start->getSceneName();

    const auto renamed = Core::StringAtom::Intern(start->getSceneName().toStdString() + "_Renamed");
    ASSERT_TRUE(manager.renameScene(start, renamed));
    EXPECT_EQ(world.startScene, renamed);

    ASSERT_TRUE(manager.removeScene(start));
    EXPECT_TRUE(world.startScene.isEmpty());
}
