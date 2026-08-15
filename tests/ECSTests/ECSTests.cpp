/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "DummyComponent.h"
#include "Scene/Rectangle.h"

#include "gtest/gtest.h"
#include <fstream>

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

    const auto json = R<decltype(obj)>::Serialize(obj).getData();
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

TEST(ECSBaseTests, DeserializationOfComponentTree)
{
    nlohmann::json stream;

    {
        DummyComponent obj("DummyName");
        obj.disable();
        obj.setNoTick(true);

        obj.addChildComponent<DummyComponent>("SubChild1");
        obj.addChildComponent<HardConstructorComponent>(123, "SubChild2", "text");
        stream = R<decltype(obj)>::Serialize(obj).getData();
    }

    // std::cout << stream.dump(4) << std::endl;

    DummyComponent restored;
    restored.addChildComponent<DummyComponent>("OriginalSubChild1");

    RResourceStream<RJsonResourceStream> data(stream);
    R<decltype(restored)>::Deserialize(data, restored);

    // --- Root assertions ---
    ASSERT_TRUE(stream.contains("_type"));
    ASSERT_EQ(stream["_type"].get<std::string>(), "DummyComponent");

    ASSERT_TRUE(stream.contains("_name"));
    ASSERT_EQ(stream["_name"].get<std::string>(), "DummyName");

    ASSERT_TRUE(stream.contains("_isEnabled"));
    ASSERT_EQ(stream["_isEnabled"].get<bool>(), false);

    ASSERT_TRUE(stream.contains("_noTick"));
    ASSERT_EQ(stream["_noTick"].get<bool>(), true);

    ASSERT_TRUE(stream.contains("a"));
    ASSERT_EQ(stream["a"].get<int>(), 123);

    ASSERT_TRUE(stream.contains("name"));
    ASSERT_EQ(stream["name"].get<std::string>(), "Lola");

    // --- Children array ---
    ASSERT_TRUE(stream.contains("_children"));
    ASSERT_TRUE(stream["_children"].is_array());
    ASSERT_EQ(stream["_children"].size(), 2u);

    // --- Child 0: DummyComponent "SubChild1" ---
    const auto& child0 = stream["_children"][0];

    ASSERT_TRUE(child0.contains("_type"));
    ASSERT_EQ(child0["_type"].get<std::string>(), "DummyComponent");

    ASSERT_TRUE(child0.contains("_name"));
    ASSERT_EQ(child0["_name"].get<std::string>(), "SubChild1");

    ASSERT_TRUE(child0.contains("_isEnabled"));
    ASSERT_EQ(child0["_isEnabled"].get<bool>(), true);

    ASSERT_TRUE(child0.contains("_noTick"));
    ASSERT_EQ(child0["_noTick"].get<bool>(), false);

    ASSERT_TRUE(child0.contains("_children"));
    ASSERT_TRUE(child0["_children"].is_null());

    ASSERT_TRUE(child0.contains("a"));
    ASSERT_EQ(child0["a"].get<int>(), 123);

    ASSERT_TRUE(child0.contains("name"));
    ASSERT_EQ(child0["name"].get<std::string>(), "Lola");

    // --- Child 1: HardConstructorComponent "SubChild2" ---
    const auto& child1 = stream["_children"][1];

    ASSERT_TRUE(child1.contains("_type"));
    ASSERT_EQ(child1["_type"].get<std::string>(), "HardConstructorComponent");

    ASSERT_TRUE(child1.contains("_name"));
    ASSERT_EQ(child1["_name"].get<std::string>(), "SubChild2");

    ASSERT_TRUE(child1.contains("_isEnabled"));
    ASSERT_EQ(child1["_isEnabled"].get<bool>(), true);

    ASSERT_TRUE(child1.contains("_noTick"));
    ASSERT_EQ(child1["_noTick"].get<bool>(), false);

    ASSERT_TRUE(child1.contains("_children"));
    ASSERT_FALSE(child1["_children"].is_null());
}

TEST(ECSBaseTests, DeserializationOfComponentTreeWithSkippedFields)
{
    nlohmann::json stream;

    {
        DummyComponent obj("DummyName");
        obj.disable();
        obj.setNoTick(true);
        obj.a = 1000;
        stream = R<decltype(obj)>::Serialize(obj).getData();
    }

    // std::cout << stream.dump(4) << std::endl;

    // Removing one field
    stream.erase("a");

    DummyComponent restored;
    RResourceStream<RJsonResourceStream> data(stream);
    R<decltype(restored)>::Deserialize(data, restored);

    ASSERT_EQ(123, restored.a); // 123 - default value
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

TEST_F(ECSTreeTests, ComplexDeSerealization)
{
    const auto serialized = R<decltype(root)>::Serialize(root).getData();

    DummyComponent restored;
    RResourceStream<RJsonResourceStream> data(serialized);
    R<decltype(restored)>::Deserialize(data, restored);

    ASSERT_EQ(root.getComponentName(), restored.getComponentName());
    ASSERT_EQ(root.getComponentType(), restored.getComponentType());
    ASSERT_EQ(root.getChildrenCount(), restored.getChildrenCount());
    ASSERT_EQ(root.getChildren().size(), restored.getChildren().size());

    ASSERT_EQ(serialized.dump(4), R<decltype(restored)>::Serialize(restored).getData().dump(4));
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

///////////////////////////////////////////////////////////

namespace
{

    class ECSTreeVehicleTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            root.setComponentName("Volvo_B5");
            root.vin = "VIN-12345";
            root.mileage = 12000;
            root.running = true;
            root.horsepower = 250;
            root.fuelLevel = 0.75f;
            root.doors = 4;
            root.hasSunroof = true;

            //
            // Engine
            //
            auto engine = root.addChildComponent<TurboEngine>("Engine_Main");
            engine->id = 1;
            engine->manufacturer = "Volvo";
            engine->cylinders = 4;
            engine->volume = 2.0f;
            engine->started = true;
            engine->boostPressure = 1.5f;
            engine->turboEnabled = true;

            //
            // Wheels (4)
            //
            for (int i = 0; i < 4; ++i)
            {
                auto wheel = root.addChildComponent<Wheel>(("Wheel_" + std::to_string(i)).c_str());
                wheel->id = 10 + i;
                wheel->manufacturer = "Michelin";
                wheel->radius = 18.0f;
                wheel->pressure = 2.5f;
            }

            //
            // Cabin
            //
            auto cabin = root.addChildComponent<Interior>("Cabin");
            cabin->color = "black";

            auto seat1 = cabin->addChildComponent<Seat>("FrontLeft");
            seat1->heated = true;
            seat1->position = 5;

            auto seat2 = cabin->addChildComponent<Seat>("FrontRight");
            seat2->heated = true;
            seat2->position = 5;

            auto dashboard = cabin->addChildComponent<Dashboard>("Dashboard");
            dashboard->hasDisplay = true;
            dashboard->brightness = 80;

            //
            // Electronics
            //
            auto adas = root.addChildComponent<Electronics>("ADAS");
            adas->firmwareVersion = 42;

            auto cam = adas->addChildComponent<Camera>("FrontCamera");
            cam->type = "vision";
            cam->value = 1.0f;
            cam->resolution = 2160;
            cam->hdr = true;
        }

        Sedan root;
    };

} // namespace

TEST_F(ECSTreeVehicleTests, FullSerializationCheck)
{
    const auto serialized = R<decltype(root)>::Serialize(root).getData();

    // ROOT FIELDS
    ASSERT_EQ(serialized["vin"], "VIN-12345");
    ASSERT_EQ(serialized["mileage"], 12000);
    ASSERT_EQ(serialized["running"], true);
    ASSERT_EQ(serialized["horsepower"], 250);
    ASSERT_FLOAT_EQ(serialized["fuelLevel"], 0.75f);
    ASSERT_EQ(serialized["doors"], 4);
    ASSERT_EQ(serialized["hasSunroof"], true);

    // CHILDREN COUNT
    ASSERT_EQ(serialized["_children"].size(), root.getChildrenCount());

    // ENGINE
    const auto& engine = serialized["_children"][0];
    ASSERT_EQ(engine["manufacturer"], "Volvo");
    ASSERT_EQ(engine["cylinders"], 4);
    ASSERT_FLOAT_EQ(engine["volume"], 2.0f);
    ASSERT_EQ(engine["started"], true);
    ASSERT_FLOAT_EQ(engine["boostPressure"], 1.5f);
    ASSERT_EQ(engine["turboEnabled"], true);

    // WHEELS
    for (int i = 1; i <= 4; ++i)
    {
        const auto& wheel = serialized["_children"][i];
        ASSERT_EQ(wheel["manufacturer"], "Michelin");
        ASSERT_FLOAT_EQ(wheel["radius"], 18.0f);
        ASSERT_FLOAT_EQ(wheel["pressure"], 2.5f);
    }

    // CABIN
    const auto& cabin = serialized["_children"][5];
    ASSERT_EQ(cabin["color"], "black");
    ASSERT_EQ(cabin["_children"].size(), 3);

    const auto& seat1 = cabin["_children"][0];
    ASSERT_EQ(seat1["heated"], true);
    ASSERT_EQ(seat1["position"], 5);

    const auto& seat2 = cabin["_children"][1];
    ASSERT_EQ(seat2["heated"], true);
    ASSERT_EQ(seat2["position"], 5);

    const auto& dashboard = cabin["_children"][2];
    ASSERT_EQ(dashboard["hasDisplay"], true);
    ASSERT_EQ(dashboard["brightness"], 80);

    // ADAS
    const auto& adas = serialized["_children"][6];
    ASSERT_EQ(adas["firmwareVersion"], 42);

    const auto& cam = adas["_children"][0];
    ASSERT_EQ(cam["type"], "vision");
    ASSERT_FLOAT_EQ(cam["value"], 1.0f);
    ASSERT_EQ(cam["resolution"], 2160);
    ASSERT_EQ(cam["hdr"], true);
}

TEST_F(ECSTreeVehicleTests, FullDeserializationCheck)
{
    const auto serialized = R<decltype(root)>::Serialize(root).getData();

    Sedan restored;
    RResourceStream<RJsonResourceStream> data(serialized);
    R<decltype(restored)>::Deserialize(data, restored);

    // ROOT
    ASSERT_EQ(restored.vin, root.vin);
    ASSERT_EQ(restored.mileage, root.mileage);
    ASSERT_EQ(restored.running, root.running);
    ASSERT_EQ(restored.horsepower, root.horsepower);
    ASSERT_FLOAT_EQ(restored.fuelLevel, root.fuelLevel);
    ASSERT_EQ(restored.doors, root.doors);
    ASSERT_EQ(restored.hasSunroof, root.hasSunroof);

    ASSERT_EQ(restored.getChildrenCount(), root.getChildrenCount());

    // ENGINE
    auto* engineR = static_cast<TurboEngine*>(restored.getChildren()[0].get());
    auto* engineO = static_cast<TurboEngine*>(root.getChildren()[0].get());

    ASSERT_EQ(engineR->manufacturer, engineO->manufacturer);
    ASSERT_EQ(engineR->cylinders, engineO->cylinders);
    ASSERT_FLOAT_EQ(engineR->volume, engineO->volume);
    ASSERT_EQ(engineR->started, engineO->started);
    ASSERT_FLOAT_EQ(engineR->boostPressure, engineO->boostPressure);
    ASSERT_EQ(engineR->turboEnabled, engineO->turboEnabled);

    // WHEELS
    for (int i = 1; i <= 4; ++i)
    {
        auto* wR = static_cast<Wheel*>(restored.getChildren()[i].get());
        auto* wO = static_cast<Wheel*>(root.getChildren()[i].get());

        ASSERT_EQ(wR->manufacturer, wO->manufacturer);
        ASSERT_FLOAT_EQ(wR->radius, wO->radius);
        ASSERT_FLOAT_EQ(wR->pressure, wO->pressure);
    }

    // CABIN
    auto* cabinR = static_cast<Interior*>(restored.getChildren()[5].get());
    auto* cabinO = static_cast<Interior*>(root.getChildren()[5].get());

    ASSERT_EQ(cabinR->color, cabinO->color);

    auto* seatR = static_cast<Seat*>(cabinR->getChildren()[0].get());
    auto* seatO = static_cast<Seat*>(cabinO->getChildren()[0].get());

    ASSERT_EQ(seatR->heated, seatO->heated);
    ASSERT_EQ(seatR->position, seatO->position);

    // DASHBOARD
    auto* dashR = static_cast<Dashboard*>(cabinR->getChildren()[2].get());
    auto* dashO = static_cast<Dashboard*>(cabinO->getChildren()[2].get());

    ASSERT_EQ(dashR->hasDisplay, dashO->hasDisplay);
    ASSERT_EQ(dashR->brightness, dashO->brightness);

    // ADAS + CAMERA
    auto* adasR = static_cast<Electronics*>(restored.getChildren()[6].get());
    auto* adasO = static_cast<Electronics*>(root.getChildren()[6].get());

    ASSERT_EQ(adasR->firmwareVersion, adasO->firmwareVersion);

    auto* camR = static_cast<Camera*>(adasR->getChildren()[0].get());
    auto* camO = static_cast<Camera*>(adasO->getChildren()[0].get());

    ASSERT_EQ(camR->type, camO->type);
    ASSERT_FLOAT_EQ(camR->value, camO->value);
    ASSERT_EQ(camR->resolution, camO->resolution);
    ASSERT_EQ(camR->hdr, camO->hdr);

    // FINAL STRICT CHECK
    ASSERT_EQ(serialized.dump(4), R<decltype(restored)>::Serialize(restored).getData().dump(4));
}

TEST(FrameByFrameAnimatorTests, AddedAnimationIsSerializedWithAnimatedPlayer)
{
    std::ifstream input{ NEXIUM_PROJECT_DIR "/data/assets/AnimatedPlayer.nx" };
    const auto assetJson = nlohmann::json::parse(input);

    SceneObj::Rectangle player;
    RResourceStream<RJsonResourceStream> stream{ assetJson["data"] };
    player.deserialize(stream);

    auto* animator = player.findFirstChildOf<Animation::FrameByFrameAnimator>();
    ASSERT_NE(animator, nullptr);

    Animation::FrameByFrameAnimation animation{ "Serialization test"_atom };
    animation.setAtlasName("default"_atom);
    ASSERT_TRUE(animation.addFrame({ 0.f, 0.f }, { 1.f, 1.f }));
    ASSERT_TRUE(animator->addAnimation(std::move(animation)));

    const auto serializedPlayer = player.serialize();
    const auto& serializedAnimations = serializedPlayer["_children"][0]["_animations"];
    ASSERT_TRUE(serializedAnimations.contains("Serialization test"));
    EXPECT_EQ(serializedAnimations["Serialization test"]["_type"],
              Animation::FrameByFrameAnimation::componentType);
}
