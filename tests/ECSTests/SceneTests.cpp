#include "Scene/Scene.h"

#include "gtest/gtest.h"
#include <functional>

namespace
{
    class TickSceneObject : public Core::SceneObject
    {
    public:
        std::function<void()> update;
        int tickCount = 0;
        bool initialized = false;

    protected:
        void onInitialize() override { initialized = true; }
        void onTick(float) override
        {
            ++tickCount;
            if (update)
            {
                update();
            }
        }
    };
} // namespace

TEST(SceneTests, SpawnDuringTickIsPublishedAfterTraversalAndTicksNextFrame)
{
    Core::Scene scene;
    Core::IntrusivePtr<TickSceneObject> spawner = new TickSceneObject;
    Core::IntrusivePtr<TickSceneObject> observer = new TickSceneObject;
    Core::IntrusivePtr<TickSceneObject> spawned = new TickSceneObject;
    scene.addObjectToScene(spawner);
    scene.addObjectToScene(observer);
    ASSERT_TRUE(spawner->initialized);

    spawner->update = [&]
    {
        if (spawner->tickCount == 1)
        {
            scene.addObjectToScene(spawned);
            EXPECT_EQ(scene.getObjects().size(), 2U);
            EXPECT_FALSE(spawned->initialized);
            // Force reallocation if additions accidentally become immediate again.
            for (int i = 0; i < 128; ++i)
            {
                scene.addObjectToScene(new TickSceneObject);
            }
        }
    };
    scene.tick(0.01f);
    EXPECT_EQ(observer->tickCount, 1);
    EXPECT_EQ(spawned->tickCount, 0);
    EXPECT_TRUE(spawned->initialized);
    EXPECT_EQ(scene.getObjects().size(), 131U);

    scene.tick(0.01f);
    EXPECT_EQ(observer->tickCount, 2);
    EXPECT_EQ(spawned->tickCount, 1);
}

TEST(SceneTests, UniqueAdditionAlsoChecksPendingObjects)
{
    Core::Scene scene;
    Core::IntrusivePtr<TickSceneObject> spawner = new TickSceneObject;
    scene.addObjectToScene(spawner);
    spawner->update = [&]
    {
        Core::SceneObject::Ptr first = new TickSceneObject;
        Core::SceneObject::Ptr second = new TickSceneObject;
        first->setComponentName("Tree"_atom);
        second->setComponentName("Tree"_atom);
        scene.addUniqueObjectToScene(first);
        scene.addUniqueObjectToScene(second);
    };
    scene.tick(0.01f);
    ASSERT_EQ(scene.getObjects().size(), 2U);
    EXPECT_EQ(scene.getObjects().back()->getComponentName(), "Tree"_atom);
}
