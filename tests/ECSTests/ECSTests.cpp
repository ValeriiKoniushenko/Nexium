/*
 * MIT License
 *
 * Copyright (c) 2018-2026 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "DummyComponent.h"

#include "gtest/gtest.h"

using namespace Core;

namespace
{

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

TEST(ECSBaseTests, SerializationOfAbstractComponentClass)
{
    DummyComponent obj;
    obj.disable();
    obj.setNoTick(true);

    const auto out = R<AbstractComponent>::Serialize<RJsonResourceStream>(obj);
    const auto json = out.getData();
    ASSERT_FALSE(json["_isEnabled"].get<bool>());
    ASSERT_TRUE(json["_noTick"].get<bool>());
}

TEST(ECSBaseTests, DeserializationOfAbstractComponentClass)
{
    DummyComponent obj;
    obj.disable();
    obj.setNoTick(true);
    const auto out = R<AbstractComponent>::Serialize<RJsonResourceStream>(obj);

    DummyComponent restored;
    restored.enable();
    restored.setNoTick(false);

    R<AbstractComponent>::Deserialize(out, restored);
    ASSERT_FALSE(restored.isEnabled());
    ASSERT_TRUE(restored.getNoTick());
}

TEST(ECSBaseTests, SerializationOfBaseComponentClass)
{
    DummyComponent obj("SomeName");
    obj.disable();
    obj.setNoTick(true);

    const auto json = R<BaseComponent>::Serialize(obj).getData();
    ASSERT_FALSE(json["_isEnabled"].get<bool>());
    ASSERT_TRUE(json["_noTick"].get<bool>());
    ASSERT_EQ("DummyComponent", json["_type"].get<StringAtom>());
    ASSERT_EQ("SomeName", json["_name"].get<StringAtom>());
}

TEST(ECSBaseTests, SerializationOfComponentTree)
{
    DummyComponent obj("DummyName");
    obj.disable();
    obj.setNoTick(true);

    obj.addChildComponent<DummyComponent>("SubChild1");
    obj.addChildComponent<HardConstructorComponent>(123, "SubChild2", "text");

    const auto json = R<DummyComponent>::Serialize(obj).getData();
    // std::cout << json.dump(4) << std::endl;

    ASSERT_FALSE(json["_isEnabled"].get<bool>());
    ASSERT_TRUE(json["_noTick"].get<bool>());
    ASSERT_EQ("DummyComponent", json["_type"].get<StringAtom>());
    ASSERT_EQ("DummyName", json["_name"].get<StringAtom>());

    auto children = json["_children"];
    ASSERT_EQ(2, std::distance(children.items().begin(), children.items().end()));

    ASSERT_TRUE(children[0]["_isEnabled"].get<bool>());
    ASSERT_FALSE(children[0]["_noTick"].get<bool>());
    ASSERT_EQ("DummyComponent", children[0]["_type"].get<StringAtom>());
    ASSERT_EQ("SubChild1", children[0]["_name"].get<StringAtom>());

    ASSERT_TRUE(children[1]["_isEnabled"].get<bool>());
    ASSERT_FALSE(children[1]["_noTick"].get<bool>());
    ASSERT_EQ("HardConstructorComponent", children[1]["_type"].get<StringAtom>());
    ASSERT_EQ("SubChild2", children[1]["_name"].get<StringAtom>());
}

TEST(ECSBaseTests, DerializationOfComponentTree)
{
    nlohmann::json stream;

    {
        DummyComponent obj("DummyName");
        obj.disable();
        obj.setNoTick(true);

        obj.addChildComponent<DummyComponent>("SubChild1");
        obj.addChildComponent<HardConstructorComponent>(123, "SubChild2", "text");
        stream = obj.serialize();
    }

    std::cout << stream.dump(4) << std::endl;

    DummyComponent restored;
    restored.addChildComponent<DummyComponent>("OriginalSubChild1");

    restored.deserialize(stream);
}

TEST(ECSBaseTests, SerializationRoundTripForBaseComponentPart)
{
    DummyComponent source("SourceName");
    source.disable();
    source.setNoTick(true);

    const auto out = R<AbstractComponent>::Serialize<RJsonResourceStream>(
        static_cast<const AbstractComponent&>(source));

    DummyComponent restored("RestoredName");
    restored.enable();
    restored.setNoTick(false);

    R<AbstractComponent>::Deserialize(out, static_cast<AbstractComponent&>(restored));

    ASSERT_FALSE(restored.isEnabled());
    ASSERT_TRUE(restored.getNoTick());

    ASSERT_EQ("RestoredName", restored.getComponentName());
    ASSERT_EQ("DummyComponent", restored.getComponentType());
}

TEST(ECSBaseTests, SerializationRoundTripForBaseComponentPart2)
{
    DummyComponent source("SomeName");
    source.disable();
    source.setNoTick(true);

    source.addChildComponent<DummyComponent>("Child1");
    source.addChildComponent<HardConstructorComponent>(123, "Child2", "text");

    const auto out = R<BaseComponent>::Serialize<RJsonResourceStream>(source);
}

TEST(ECSBaseTests, SerializationDoesNotTouchNonReflectedDummyComponentState)
{
    DummyComponent source("SourceName");
    source.disable();
    source.setNoTick(true);

    const auto out = R<AbstractComponent>::Serialize<RJsonResourceStream>(source);

    DummyComponent restored("KeepMyOwnState");
    restored.enable();
    restored.setNoTick(false);

    R<AbstractComponent>::Deserialize(out, restored);

    ASSERT_FALSE(restored.isEnabled());
    ASSERT_TRUE(restored.getNoTick());

    ASSERT_EQ("KeepMyOwnState", restored.getComponentName());
    ASSERT_EQ("DummyComponent", restored.getComponentType());
    ASSERT_TRUE(restored.isTypeOf<DummyComponent>());
}

TEST(ECSBaseTests, SimpleCreation)
{
    {
        DummyComponent c;
        c.setComponentName("SomeName");
        ASSERT_EQ("SomeName", c.getComponentName());
        ASSERT_EQ("DummyComponent", c.getComponentType());
        ASSERT_TRUE(c.getComponentType().isStatic());
    }

    {
        DummyComponent c("SomeName");
        ASSERT_EQ("SomeName", c.getComponentName());
        ASSERT_EQ("DummyComponent", c.getComponentType());
        ASSERT_TRUE(c.getComponentType().isStatic());
    }
}

TEST(ECSBaseTests, SimpleCopy)
{
    DummyComponent c;
    c.setComponentName("SomeName");
    ASSERT_EQ("SomeName", c.getComponentName());
    ASSERT_EQ("DummyComponent", c.getComponentType());
    ASSERT_TRUE(c.getComponentType().isStatic());

    DummyComponent copy = c;
    ASSERT_EQ("SomeName", c.getComponentName());
    ASSERT_EQ("DummyComponent", c.getComponentType());
    ASSERT_TRUE(c.getComponentType().isStatic());

    ASSERT_EQ("SomeName", copy.getComponentName());
    ASSERT_EQ("DummyComponent", copy.getComponentType());
    ASSERT_TRUE(copy.getComponentType().isStatic());
}

TEST(ECSBaseTests, SimpleMove)
{
    DummyComponent c;
    c.setComponentName("SomeName");
    ASSERT_EQ("SomeName", c.getComponentName());
    ASSERT_EQ("DummyComponent", c.getComponentType());
    ASSERT_TRUE(c.getComponentType().isStatic());

    DummyComponent copy = std::move(c);
    ASSERT_TRUE(c.getComponentName().isEmpty());
    ASSERT_TRUE(c.getComponentType().isEmpty());

    ASSERT_EQ("SomeName", copy.getComponentName());
    ASSERT_EQ("DummyComponent", copy.getComponentType());
    ASSERT_TRUE(copy.getComponentType().isStatic());
}

TEST(ECSBaseTests, Misc)
{
    DummyComponent c("SomeName");
    ASSERT_TRUE(c.isTypeOf<DummyComponent>());
    ASSERT_TRUE(c.isValid());

    ASSERT_NE(nullptr, c.castTo<DummyComponent>());
    ASSERT_NE(0, c.makeHash());
}

TEST(ECSBaseTests, AddingNewChild)
{
    DummyComponent root("Root");
    ASSERT_EQ("Root", root.getComponentName());
    ASSERT_EQ("DummyComponent", root.getComponentType());

    auto top = root.addChildComponent<DummyComponent>();
    top->setComponentName("Top");
    ASSERT_EQ("Top", top->getComponentName());
    ASSERT_EQ("DummyComponent", top->getComponentType());

    ASSERT_FALSE(root.hasParent());
    ASSERT_TRUE(root.hasChildren());
    ASSERT_EQ(1, root.getChildrenCount());
    ASSERT_EQ(1, root.getChildren().size());

    ASSERT_TRUE(top->hasParent());
    ASSERT_FALSE(top->hasChildren());
    ASSERT_EQ(&root, top->getParent());
    ASSERT_EQ(root.getChildren().front().get(), top);
    ASSERT_NE(root, *top);
}

TEST(ECSBaseTests, AddingNewChildAndCopy)
{
    DummyComponent root("Root");
    ASSERT_EQ("Root", root.getComponentName());
    ASSERT_EQ("DummyComponent", root.getComponentType());

    {
        auto top = root.addChildComponent<DummyComponent>();
        top->setComponentName("Top");
        ASSERT_EQ("Top", top->getComponentName());
        ASSERT_EQ("DummyComponent", top->getComponentType());

        ASSERT_FALSE(root.hasParent());
        ASSERT_TRUE(root.hasChildren());
        ASSERT_EQ(1, root.getChildrenCount());
        ASSERT_EQ(1, root.getChildren().size());

        ASSERT_TRUE(top->hasParent());
        ASSERT_FALSE(top->hasChildren());
        ASSERT_EQ(&root, top->getParent());
        ASSERT_EQ(root.getChildren().front(), top);
        ASSERT_NE(root, *top);
    }

    {
        DummyComponent copy = root;

        auto top = copy.getChildAt(0);
        ASSERT_NE(top.get(), root.getChildAt(0).get());
        ASSERT_EQ("Top", top->getComponentName());
        ASSERT_EQ("DummyComponent", top->getComponentType());

        ASSERT_FALSE(copy.hasParent());
        ASSERT_TRUE(copy.hasChildren());
        ASSERT_EQ(1, copy.getChildrenCount());
        ASSERT_EQ(1, copy.getChildren().size());

        ASSERT_TRUE(top->hasParent());
        ASSERT_FALSE(top->hasChildren());
        ASSERT_EQ(&copy, top->getParent());
        ASSERT_EQ(copy.getChildren().front(), top);
        ASSERT_NE(copy, *top);
    }
}

TEST(ECSBaseTests, AdvancedAddingNewChild)
{
    DummyComponent root("Root");
    {
        auto comp = root.addChildComponent<HardConstructorComponent>();
        ASSERT_EQ("", comp->getComponentName());
        ASSERT_FALSE(comp->isValid());
        ASSERT_TRUE(comp->isTypeOf<HardConstructorComponent>());
        ASSERT_EQ("HardConstructorComponent", comp->getComponentType());
        comp->setComponentName("Wow");
        ASSERT_TRUE(comp->isValid());
        ASSERT_EQ(0, comp->_a);
        ASSERT_EQ("", comp->_b);
    }

    {
        auto comp
            = root.addChildComponent<HardConstructorComponent>(123, "SomeName", "DummyString");
        ASSERT_EQ("SomeName", comp->getComponentName());
        ASSERT_TRUE(comp->isValid());
        ASSERT_TRUE(comp->isTypeOf<HardConstructorComponent>());
        ASSERT_EQ("HardConstructorComponent", comp->getComponentType());
        comp->setComponentName("Wow");
        ASSERT_TRUE(comp->isValid());
        ASSERT_EQ(123, comp->_a);
        ASSERT_EQ("DummyString", comp->_b);
    }
}

TEST(ECSBaseTests, RemovingChild)
{
    DummyComponent root("Root");
    ASSERT_EQ("Root", root.getComponentName());
    ASSERT_EQ("DummyComponent", root.getComponentType());

    {
        auto top = root.addChildComponent<DummyComponent>();
        top->setComponentName("Top");
        ASSERT_EQ("Top", top->getComponentName());
        ASSERT_EQ("DummyComponent", top->getComponentType());

        ASSERT_FALSE(root.hasParent());
        ASSERT_TRUE(root.hasChildren());
        ASSERT_EQ(1, root.getChildrenCount());
        ASSERT_EQ(1, root.getChildren().size());

        ASSERT_TRUE(top->hasParent());
        ASSERT_FALSE(top->hasChildren());
        ASSERT_EQ(&root, top->getParent());
        ASSERT_EQ(root.getChildren().front(), top);
        ASSERT_NE(root, *top);

        root.removeChild(top);
    }

    ASSERT_EQ(0, root.getChildrenCount());
    ASSERT_EQ(0, root.getChildren().size());
}

TEST(ECSBaseTests, RemovingChildIf)
{
    DummyComponent root("Root");
    ASSERT_EQ("Root", root.getComponentName());
    ASSERT_EQ("DummyComponent", root.getComponentType());

    std::vector<StringAtom> names = { "Hello", "World", "How", "Are", "AYou", "Idk" };

    for (auto&& name : names)
    {
        (void)root.addChildComponent<DummyComponent>(name);
    }

    ASSERT_EQ(names.size(), root.getChildrenCount());

    root.removeChildIf([](const BaseComponent* c) { return c->getComponentName() == "How"; });

    ASSERT_EQ(names.size() - 1, root.getChildrenCount());

    root.removeChildIf([](const BaseComponent* c) { return c->getComponentName().front() == 'A'; });

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
    StringAtom trunk;

    root.forEach([&](BaseComponent* c) { trunk.pushBack(c->getComponentName()); });

    root.forEach([&](const BaseComponent* c) { trunk.pushBack(c->getComponentName()); });

    root.forEach(
        [&](const BaseComponent* c)
        {
            trunk.pushBack(c->getComponentName());
            return c->getComponentName() != "Middle2";
        });

    const auto& croot = static_cast<const decltype(root)&>(root);

    croot.forEach([&](const BaseComponent* c) { trunk.pushBack(c->getComponentName()); });

    croot.forEach(
        [&](const BaseComponent* c)
        {
            trunk.pushBack(c->getComponentName());
            return c->getComponentName() != "Middle2";
        });
}

TEST_F(ECSTreeTests, DFSIteratorTest)
{
    StringAtom trunk;

    root.forEachDFS([&](BaseComponent* c) { trunk.pushBack(c->getComponentName()); });

    root.forEachDFS([&](const BaseComponent* c) { trunk.pushBack(c->getComponentName()); });

    root.forEachDFS(
        [&](const BaseComponent* c)
        {
            trunk.pushBack(c->getComponentName());
            return c->getComponentName() != "Middle2";
        });

    const auto& croot = static_cast<const decltype(root)&>(root);

    croot.forEachDFS([&](const BaseComponent* c) { trunk.pushBack(c->getComponentName()); });

    croot.forEachDFS(
        [&](const BaseComponent* c)
        {
            trunk.pushBack(c->getComponentName());
            return c->getComponentName() != "Middle2";
        });
}

TEST_F(ECSTreeTests, DeepTreeCopy)
{
    std::vector<BaseComponent*> rootSet;
    std::vector<BaseComponent*> newRootSet;

    root.forEach([&](auto* c) { rootSet.push_back(c); });

    DummyComponent newRoot = root;

    newRoot.forEach([&](auto* c) { newRootSet.push_back(c); });

    ASSERT_FALSE(rootSet.empty());
    ASSERT_EQ(rootSet.size(), newRootSet.size());

    for (std::size_t i = 0; i < rootSet.size(); ++i)
    {
        auto* first = rootSet[i];
        auto* second = newRootSet[i];

        ASSERT_NE(first, second);
        ASSERT_EQ(first->getComponentName(), second->getComponentName());
        ASSERT_EQ(first->getComponentType(), second->getComponentType());
        ASSERT_EQ(first->getChildrenCount(), second->getChildrenCount());

        ASSERT_TRUE(first->getComponentType().isStatic());
        ASSERT_TRUE(second->getComponentType().isStatic());

        if (first->hasParent() || second->hasParent())
        {
            ASSERT_NE(first->getParent(), second->getParent());
            ASSERT_EQ(*first->getParent(), *second->getParent());
        }
    }
}

TEST_F(ECSTreeTests, DeepTreeCopyFromSpecificMidNode)
{
    std::vector<BaseComponent*> rootSet;
    std::vector<BaseComponent*> newRootSet;

    auto node = root.findFirstChildOf<DummyComponent>("Top2");
    ASSERT_NE(nullptr, node);

    node->forEach([&](auto* c) { rootSet.push_back(c); });

    DummyComponent newNode = *node;

    newNode.forEach([&](auto* c) { newRootSet.push_back(c); });

    ASSERT_EQ(nullptr, newNode.getParent());

    ASSERT_FALSE(rootSet.empty());
    ASSERT_EQ(rootSet.size(), newRootSet.size());

    for (std::size_t i = 0; i < rootSet.size(); ++i)
    {
        auto* first = rootSet[i];
        auto* second = newRootSet[i];

        ASSERT_NE(first, second);
        ASSERT_EQ(first->getComponentName(), second->getComponentName());
        ASSERT_EQ(first->getComponentType(), second->getComponentType());
        ASSERT_EQ(first->getChildrenCount(), second->getChildrenCount());

        ASSERT_TRUE(first->getComponentType().isStatic());
        ASSERT_TRUE(second->getComponentType().isStatic());

        if (first->hasParent() && second->hasParent())
        {
            ASSERT_NE(first->getParent(), second->getParent());
            ASSERT_EQ(*first->getParent(), *second->getParent());
        }
    }
}

TEST(ECSBaseTests, GlobalFactoryCanCreateRegisteredComponent)
{
    auto* created = GetGlobalComponentFactory().create(DummyComponent::componentType);
    ASSERT_NE(nullptr, created);
    ASSERT_EQ(created->getComponentType(), DummyComponent::componentType);

    delete created;
}

TEST(ECSBaseTests, FindFirstChildOfByTypeAndName)
{
    DummyComponent root("Root");

    (void)root.addChildComponent<DummyComponent>("A");
    (void)root.addChildComponent<DummyComponent>("B");

    auto* foundA = root.findFirstChildOf<DummyComponent>("A");
    ASSERT_NE(nullptr, foundA);
    ASSERT_EQ(foundA->getComponentName(), "A");

    auto* foundB = root.findFirstChildOf<DummyComponent>("B");
    ASSERT_NE(nullptr, foundB);
    ASSERT_EQ(foundB->getComponentName(), "B");

    ASSERT_EQ(nullptr, root.findFirstChildOf<DummyComponent>("DoesNotExist"));
}

TEST(ECSBaseTests, AddUniqueTypeChildComponentReturnsSameInstance)
{
    DummyComponent root("Root");

    auto* first = root.addUniqueTypeChildComponent<DummyComponent>("A");
    auto* second = root.addUniqueTypeChildComponent<DummyComponent>("B"); // should not add another

    ASSERT_NE(nullptr, first);
    ASSERT_NE(nullptr, second);
    ASSERT_EQ(first, second);
    ASSERT_EQ(1, root.getChildrenCount());
}

TEST(ECSBaseTests, GetOrAddChildComponentReturnsExisting)
{
    DummyComponent root("Root");

    auto* first = root.getOrAddChildComponent<DummyComponent>("A");
    auto* second = root.getOrAddChildComponent<DummyComponent>("B");

    ASSERT_NE(nullptr, first);
    ASSERT_NE(nullptr, second);
    ASSERT_EQ(first, second);
    ASSERT_EQ(1, root.getChildrenCount());
}

TEST(ECSBaseTests, AttachChildClonesAndSetsParent)
{
    DummyComponent root("Root");

    DummyComponent::Ptr externalChild = new DummyComponent("Child");
    ASSERT_FALSE(externalChild->hasParent());

    BaseComponent* attached = root.attachChild(externalChild);
    ASSERT_NE(nullptr, attached);

    ASSERT_NE(attached, externalChild.get()); // must be a clone
    ASSERT_EQ(attached->getComponentType(), externalChild->getComponentType());
    ASSERT_EQ(attached->getComponentName(), externalChild->getComponentName());
    ASSERT_EQ(attached->getParent(), &root);
    ASSERT_EQ(1, root.getChildrenCount());
}

TEST(ECSBaseTests, DetachChildRemovesFromChildrenList)
{
    DummyComponent root("Root");

    auto* child = root.addChildComponent<DummyComponent>("Child");
    ASSERT_NE(nullptr, child);
    ASSERT_EQ(1, root.getChildrenCount());

    root.detachChild(child);
    ASSERT_EQ(0, root.getChildrenCount());
    ASSERT_FALSE(root.hasChildren());
}

TEST(ECSBaseTests, RemoveChildOfRemovesAllMatchingTypes)
{
    DummyComponent root("Root");

    (void)root.addChildComponent<DummyComponent>("A");
    (void)root.addChildComponent<DummyComponent>("B");
    ASSERT_TRUE(root.hasChildrenAs<DummyComponent>());

    root.removeChildOf<DummyComponent>();

    ASSERT_EQ(0, root.getChildrenCount());
    ASSERT_FALSE(root.hasChildrenAs<DummyComponent>());
}

TEST(ECSBaseTests, GetOwnerReturnsTopmostParent)
{
    DummyComponent root("Root");
    auto* child = root.addChildComponent<DummyComponent>("Child");
    auto* grandChild = child->addChildComponent<DummyComponent>("GrandChild");

    ASSERT_NE(nullptr, grandChild);

    ASSERT_EQ(grandChild->getOwner(), &root);
    ASSERT_EQ(child->getOwner(), &root);
    ASSERT_EQ(root.getOwner(), &root);
}

TEST(ECSBaseTests, MakeHashDependsOnParentChain)
{
    DummyComponent rootA("RootA");
    DummyComponent rootB("RootB");

    auto* childA = rootA.addChildComponent<DummyComponent>("Child");
    auto* childB = rootB.addChildComponent<DummyComponent>("Child");

    ASSERT_NE(nullptr, childA);
    ASSERT_NE(nullptr, childB);

    ASSERT_NE(childA->makeHash(), childB->makeHash());
}

TEST(ECSBaseTests, InitializeIsCalledAndPropagatesToChildren)
{
    InitSpyComponent root("Root");
    ASSERT_FALSE(root.isInitialized());

    auto* child = root.addChildComponent<InitSpyComponent>("Child");
    ASSERT_NE(nullptr, child);

    ASSERT_TRUE(root.isInitialized());
    ASSERT_TRUE(child->isInitialized());

    ASSERT_EQ(1, root.preInitCalls);
    ASSERT_EQ(1, root.initCalls);
    ASSERT_EQ(1, child->preInitCalls);
    ASSERT_EQ(1, child->initCalls);

    root.invalidate();
    ASSERT_FALSE(root.isInitialized());

    root.initialize();
    ASSERT_TRUE(root.isInitialized());

    ASSERT_EQ(2, root.preInitCalls);
    ASSERT_EQ(2, root.initCalls);

    ASSERT_EQ(1, child->preInitCalls);
    ASSERT_EQ(1, child->initCalls);
}
