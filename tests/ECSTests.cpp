// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "GameplaySystem/ECS/BaseComponent.h"

#include "gtest/gtest.h"

namespace
{
    class DummyComponent : public SW::BaseComponent
    {
        ECS_REGISTER_NEW_COMPONENT(DummyComponent, SW::BaseComponent);
        int a = 123;
        [[nodiscard]] nlohmann::json toJson() const override
        {
            auto json = SW::BaseComponent::toJson();
            json["a"] = a;
            return json;
        }
        void fromJson(const nlohmann::json& json, bool isIgnoreChildren = false) override
        {
            SW::BaseComponent::fromJson(json, isIgnoreChildren);
            a = json["a"].get<int>();
        }
    };

    ECS_REGISTER_NEW_COMPONENT_TYPE(DummyComponent);

    class HardConstructorComponent : public SW::BaseComponent
    {
        ECS_REGISTER_NEW_COMPONENT(HardConstructorComponent, SW::BaseComponent);

        HardConstructorComponent(int a, const Core::StringAtom& name, std::string b)
            : BaseComponent(componentType, name),
              _a(a),
              _b(b) {};

        int _a = 0;
        std::string _b;
    };

    ECS_REGISTER_NEW_COMPONENT_TYPE(HardConstructorComponent);

    class ECSTreeTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            root.setComponentName("RootComponent");
            {
                auto top = root.addChildComponent<DummyComponent>("Top1");
                auto middle1 = top->addChildComponent<DummyComponent>("Middle1");
            }
            {
                auto top = root.addChildComponent<DummyComponent>("Top2");
                {
                    auto middle1 = top->addChildComponent<DummyComponent>("Middle1");
                    auto middle2 = top->addChildComponent<DummyComponent>("Middle2");
                    auto bottom1 = middle2->addChildComponent<DummyComponent>("Bottom1");
                    auto super_bottom1 = bottom1->addChildComponent<DummyComponent>("SuperBottom1");
                }
            }
        }

        DummyComponent root;
    };
} // namespace

TEST(ECSBaseTests, SimpleCreation)
{
    {
        DummyComponent c;
        c.setComponentName("SomeName");
        EXPECT_EQ("SomeName", c.getComponentName());
        EXPECT_EQ("DummyComponent", c.getComponentType());
        EXPECT_TRUE(c.getComponentType().isStatic());
    }

    {
        DummyComponent c("SomeName");
        EXPECT_EQ("SomeName", c.getComponentName());
        EXPECT_EQ("DummyComponent", c.getComponentType());
        EXPECT_TRUE(c.getComponentType().isStatic());
    }
}

TEST(ECSBaseTests, Misc)
{
    DummyComponent c("SomeName");
    EXPECT_TRUE(c.isTypeOf<DummyComponent>());
    EXPECT_TRUE(c.isValid());
    // EXPECT_EQ(nullptr, c.castTo<HardConstructorComponent>());
    EXPECT_NE(nullptr, c.castTo<DummyComponent>());
    EXPECT_NE(0, c.makeHash());
}

TEST(ECSBaseTests, AddingNewChild)
{
    DummyComponent root("Root");
    EXPECT_EQ("Root", root.getComponentName());
    EXPECT_EQ("DummyComponent", root.getComponentType());

    auto top = root.addChildComponent<DummyComponent>();
    top->setComponentName("Top");
    EXPECT_EQ("Top", top->getComponentName());
    EXPECT_EQ("DummyComponent", top->getComponentType());

    EXPECT_FALSE(root.hasParent());
    EXPECT_TRUE(root.hasChildren());
    EXPECT_EQ(1, root.getChildrenCount());
    EXPECT_EQ(1, root.getChildren().size());

    EXPECT_TRUE(top->hasParent());
    EXPECT_FALSE(top->hasChildren());
    EXPECT_EQ(&root, top->getParent());
    EXPECT_EQ(root.getChildren().front(), top);
    EXPECT_NE(root, *top);
}

TEST(ECSBaseTests, AdvancedAddingNewChild)
{
    DummyComponent root("Root");
    {
        auto comp = root.addChildComponent<HardConstructorComponent>();
        EXPECT_EQ("", comp->getComponentName());
        EXPECT_FALSE(comp->isValid());
        EXPECT_TRUE(comp->isTypeOf<HardConstructorComponent>());
        EXPECT_EQ("HardConstructorComponent", comp->getComponentType());
        comp->setComponentName("Wow");
        EXPECT_TRUE(comp->isValid());
        EXPECT_EQ(0, comp->_a);
        EXPECT_EQ("", comp->_b);
    }

    {
        auto comp
            = root.addChildComponent<HardConstructorComponent>(123, "SomeName", "DummyString");
        EXPECT_EQ("SomeName", comp->getComponentName());
        EXPECT_TRUE(comp->isValid());
        EXPECT_TRUE(comp->isTypeOf<HardConstructorComponent>());
        EXPECT_EQ("HardConstructorComponent", comp->getComponentType());
        comp->setComponentName("Wow");
        EXPECT_TRUE(comp->isValid());
        EXPECT_EQ(123, comp->_a);
        EXPECT_EQ("DummyString", comp->_b);
    }
}

TEST(ECSBaseTests, RemovingChild)
{
    DummyComponent root("Root");
    EXPECT_EQ("Root", root.getComponentName());
    EXPECT_EQ("DummyComponent", root.getComponentType());

    {
        auto top = root.addChildComponent<DummyComponent>();
        top->setComponentName("Top");
        EXPECT_EQ("Top", top->getComponentName());
        EXPECT_EQ("DummyComponent", top->getComponentType());

        EXPECT_FALSE(root.hasParent());
        EXPECT_TRUE(root.hasChildren());
        EXPECT_EQ(1, root.getChildrenCount());
        EXPECT_EQ(1, root.getChildren().size());

        EXPECT_TRUE(top->hasParent());
        EXPECT_FALSE(top->hasChildren());
        EXPECT_EQ(&root, top->getParent());
        EXPECT_EQ(root.getChildren().front(), top);
        EXPECT_NE(root, *top);

        root.removeChild(top);
    }

    ASSERT_EQ(0, root.getChildrenCount());
    ASSERT_EQ(0, root.getChildren().size());
}

TEST(ECSBaseTests, RemovingChildIf)
{
    DummyComponent root("Root");
    EXPECT_EQ("Root", root.getComponentName());
    EXPECT_EQ("DummyComponent", root.getComponentType());

    std::vector<Core::StringAtom> names = { "Hello", "World", "How", "Are", "AYou", "Idk" };

    for (auto&& name : names)
    {
        (void)root.addChildComponent<DummyComponent>(name);
    }

    ASSERT_EQ(names.size(), root.getChildrenCount());

    root.removeChildIf(
        [](const SW::BaseComponent* c)
        {
            return c->getComponentName() == "How";
        });

    ASSERT_EQ(names.size() - 1, root.getChildrenCount());

    root.removeChildIf(
        [](const SW::BaseComponent* c)
        {
            return c->getComponentName().front() == 'A';
        });

    ASSERT_EQ(names.size() - 1 - 2, root.getChildrenCount());
}

TEST(ECSBaseTests, ParentInvalidating)
{
    DummyComponent root("Root");
    auto* world = root.addChildComponent<DummyComponent>("World");

    ASSERT_EQ(root.getChildren().size(), 1);
    ASSERT_FALSE(root.getComponentType().isEmpty());
    ASSERT_EQ(root.getChildAt(0)->getComponentName(), "World");
    ASSERT_EQ(world->getParent(), &root);
    ASSERT_EQ(world->getParent()->getComponentName(), "Root");

    DummyComponent movedRoot = std::move(root);

    ASSERT_EQ(root.getChildren().size(), 0);
    ASSERT_TRUE(root.getComponentType().isEmpty());

    ASSERT_EQ(movedRoot.getChildren().size(), 1);
    ASSERT_FALSE(movedRoot.getComponentType().isEmpty());
    ASSERT_EQ(movedRoot.getChildAt(0)->getComponentName(), "World");
    ASSERT_EQ(world->getParent(), &movedRoot);
    ASSERT_EQ(world->getParent()->getComponentName(), "Root");
}

TEST_F(ECSTreeTests, BFSIteratorTest)
{
    Core::StringAtom trunk;

    root.forEach(
        [&](SW::BaseComponent* c)
        {
            trunk.push_back(c->getComponentName());
        });

    root.forEach(
        [&](const SW::BaseComponent* c)
        {
            trunk.push_back(c->getComponentName());
        });

    root.forEach(
        [&](const SW::BaseComponent* c)
        {
            trunk.push_back(c->getComponentName());
            return c->getComponentName() != "Middle2";
        });

    const auto& croot = static_cast<const decltype(root)&>(root);

    croot.forEach(
        [&](const SW::BaseComponent* c)
        {
            trunk.push_back(c->getComponentName());
        });

    croot.forEach(
        [&](const SW::BaseComponent* c)
        {
            trunk.push_back(c->getComponentName());
            return c->getComponentName() != "Middle2";
        });
}

TEST_F(ECSTreeTests, DFSIteratorTest)
{
    Core::StringAtom trunk;

    root.forEachDFS(
        [&](SW::BaseComponent* c)
        {
            trunk.push_back(c->getComponentName());
        });

    root.forEachDFS(
        [&](const SW::BaseComponent* c)
        {
            trunk.push_back(c->getComponentName());
        });

    root.forEachDFS(
        [&](const SW::BaseComponent* c)
        {
            trunk.push_back(c->getComponentName());
            return c->getComponentName() != "Middle2";
        });

    const auto& croot = static_cast<const decltype(root)&>(root);

    croot.forEachDFS(
        [&](const SW::BaseComponent* c)
        {
            trunk.push_back(c->getComponentName());
        });

    croot.forEachDFS(
        [&](const SW::BaseComponent* c)
        {
            trunk.push_back(c->getComponentName());
            return c->getComponentName() != "Middle2";
        });
}

TEST_F(ECSTreeTests, exportingToJson)
{
    (void)root.getChildAt(1)
        ->addChildComponent<HardConstructorComponent>("Hello")
        ->addChildComponent<HardConstructorComponent>("Hello1")
        ->addChildComponent<HardConstructorComponent>("Hello2");

    const auto json = root.toJson();

    const auto dump = Core::StringAtom(json.dump(4));

    DummyComponent newRoot;
    newRoot.fromJson(json);
    const auto newDump = Core::StringAtom(newRoot.toJson().dump(4));

    std::cout << dump << std::endl;
    EXPECT_EQ(dump, newDump);
}