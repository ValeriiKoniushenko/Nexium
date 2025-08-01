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
    };

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
    EXPECT_EQ(1, root.getChildrenSize());
    EXPECT_EQ(1, root.getChildren().size());

    EXPECT_TRUE(top->hasParent());
    EXPECT_FALSE(top->hasChildren());
    EXPECT_EQ(&root, top->getParent());
    EXPECT_EQ(root.getChildren().front(), top);
    EXPECT_NE(root, *top);
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
        EXPECT_EQ(1, root.getChildrenSize());
        EXPECT_EQ(1, root.getChildren().size());

        EXPECT_TRUE(top->hasParent());
        EXPECT_FALSE(top->hasChildren());
        EXPECT_EQ(&root, top->getParent());
        EXPECT_EQ(root.getChildren().front(), top);
        EXPECT_NE(root, *top);

        root.removeChild(top);
    }

    ASSERT_EQ(0, root.getChildrenSize());
    ASSERT_EQ(0, root.getChildren().size());
}

TEST(ECSBaseTests, RemovingChildIf)
{
    DummyComponent root("Root");
    EXPECT_EQ("Root", root.getComponentName());
    EXPECT_EQ("DummyComponent", root.getComponentType());

    std::vector<Core::StringAtom> names = { "Hello", "World", "How", "Are", "You", "Idk" };

    for (auto&& name : names)
    {
        (void)root.addChildComponent<DummyComponent>(name);
    }

    ASSERT_EQ(names.size(), root.getChildrenSize());

    root.removeChildIf(
        [](const SW::BaseComponent* c)
        {
            return c->getComponentName() == "How";
        });

    ASSERT_EQ(names.size() - 1, root.getChildrenSize());
}
