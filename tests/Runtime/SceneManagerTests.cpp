#include "NxWorld/Scene/Scene/SceneManager.h"

#include "gtest/gtest.h"

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
