#include "ECS/BaseComponent.h"

#include "gtest/gtest.h"

using namespace Core;

namespace
{
    class DummyComponent : public BaseComponent
    {
        ECS_COMPONENT_DECL(DummyComponent, BaseComponent);
        int a = 123;
        std::string name = "Lola";

        void ioFieldsUpdate(DataStream& out) override
        {
            BaseComponent::ioFieldsUpdate(out);

            auto stream = out.dedicatedNesting("DummyComponent");

            stream.field("a", a);
            stream.field("name", name);
        }
    };

    ECS_COMPONENT_IMPL(DummyComponent);

    class HardConstructorComponent : public BaseComponent
    {
        ECS_COMPONENT_DECL(HardConstructorComponent, BaseComponent);

        HardConstructorComponent(int a, const StringAtom& name, std::string b)
            : BaseComponent(componentType, name),
              _a(a),
              _b(std::move(b)) {};

        int _a = 0;
        std::string _b;
    };

    ECS_COMPONENT_IMPL(HardConstructorComponent);

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

    class InitSpyComponent : public BaseComponent
    {
        ECS_COMPONENT_DECL(InitSpyComponent, BaseComponent);

    public:
        int preInitCalls = 0;
        int initCalls = 0;

    protected:
        void onPreInitialize() override { ++preInitCalls; }
        void onInitialize() override { ++initCalls; }
    };

    ECS_COMPONENT_IMPL(InitSpyComponent);
} // namespace

TEST(ECSBaseTests, SerializationOfAbstractClass)
{
    DummyComponent obj;
    obj.disable();
    obj.setNoTick(true);

    const auto out = R<Core::AbstractComponent>::Serialize<RJsonResourceStream>(obj);

    DummyComponent restored;
    restored.enable();
    restored.setNoTick(false);

    R<Core::AbstractComponent>::Deserialize(out, restored);

    EXPECT_FALSE(restored.isEnabled());
    EXPECT_TRUE(restored.getNoTick());
}

TEST(ECSBaseTests, SerializationRoundTripForAbstractComponent)
{
    DummyComponent source;
    source.disable();
    source.setNoTick(true);

    const auto out = R<Core::AbstractComponent>::Serialize<RJsonResourceStream>(source);

    DummyComponent restored;
    restored.enable();
    restored.setNoTick(false);

    R<Core::AbstractComponent>::Deserialize(out, restored);

    EXPECT_FALSE(restored.isEnabled());
    EXPECT_TRUE(restored.getNoTick());
}

TEST(ECSBaseTests, SerializationRoundTripForBaseComponentPart)
{
    DummyComponent source("SourceName");
    source.disable();
    source.setNoTick(true);

    const auto out = R<Core::AbstractComponent>::Serialize<RJsonResourceStream>(
        static_cast<const Core::AbstractComponent&>(source));

    DummyComponent restored("RestoredName");
    restored.enable();
    restored.setNoTick(false);

    R<Core::AbstractComponent>::Deserialize(out, static_cast<Core::AbstractComponent&>(restored));

    EXPECT_FALSE(restored.isEnabled());
    EXPECT_TRUE(restored.getNoTick());

    EXPECT_EQ("RestoredName", restored.getComponentName());
    EXPECT_EQ("DummyComponent", restored.getComponentType());
}

TEST(ECSBaseTests, SerializationRoundTripForBaseComponentPart2)
{
    DummyComponent source("SomeName");
    source.disable();
    source.setNoTick(true);

    source.addChildComponent<DummyComponent>("Child1");
    source.addChildComponent<HardConstructorComponent>(123, "Child2", "text");

    const auto out = R<Core::BaseComponent>::Serialize<RJsonResourceStream>(source);
}

TEST(ECSBaseTests, SerializationDoesNotTouchNonReflectedDummyComponentState)
{
    DummyComponent source("SourceName");
    source.disable();
    source.setNoTick(true);

    const auto out = R<Core::AbstractComponent>::Serialize<RJsonResourceStream>(source);

    DummyComponent restored("KeepMyOwnState");
    restored.enable();
    restored.setNoTick(false);

    R<Core::AbstractComponent>::Deserialize(out, restored);

    EXPECT_FALSE(restored.isEnabled());
    EXPECT_TRUE(restored.getNoTick());

    EXPECT_EQ("KeepMyOwnState", restored.getComponentName());
    EXPECT_EQ("DummyComponent", restored.getComponentType());
    EXPECT_TRUE(restored.isTypeOf<DummyComponent>());
}

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

TEST(ECSBaseTests, SimpleCopy)
{
    DummyComponent c;
    c.setComponentName("SomeName");
    EXPECT_EQ("SomeName", c.getComponentName());
    EXPECT_EQ("DummyComponent", c.getComponentType());
    EXPECT_TRUE(c.getComponentType().isStatic());

    DummyComponent copy = c;
    EXPECT_EQ("SomeName", c.getComponentName());
    EXPECT_EQ("DummyComponent", c.getComponentType());
    EXPECT_TRUE(c.getComponentType().isStatic());

    EXPECT_EQ("SomeName", copy.getComponentName());
    EXPECT_EQ("DummyComponent", copy.getComponentType());
    EXPECT_TRUE(copy.getComponentType().isStatic());
}

TEST(ECSBaseTests, SimpleMove)
{
    DummyComponent c;
    c.setComponentName("SomeName");
    EXPECT_EQ("SomeName", c.getComponentName());
    EXPECT_EQ("DummyComponent", c.getComponentType());
    EXPECT_TRUE(c.getComponentType().isStatic());

    DummyComponent copy = std::move(c);
    EXPECT_TRUE(c.getComponentName().isEmpty());
    EXPECT_TRUE(c.getComponentType().isEmpty());

    EXPECT_EQ("SomeName", copy.getComponentName());
    EXPECT_EQ("DummyComponent", copy.getComponentType());
    EXPECT_TRUE(copy.getComponentType().isStatic());
}

TEST(ECSBaseTests, Misc)
{
    DummyComponent c("SomeName");
    EXPECT_TRUE(c.isTypeOf<DummyComponent>());
    EXPECT_TRUE(c.isValid());

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
    EXPECT_EQ(root.getChildren().front().get(), top);
    EXPECT_NE(root, *top);
}

TEST(ECSBaseTests, AddingNewChildAndCopy)
{
    DummyComponent root("Root");
    EXPECT_EQ("Root", root.getComponentName());
    EXPECT_EQ("DummyComponent", root.getComponentType());

    {
        auto top = root.addChildComponent<DummyComponent>();
        top->setComponentName("Top");
        EXPECT_EQ("Top", top->getComponentName());
        EXPECT_EQ("DummyComponent", top->getComponentType());

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
        EXPECT_NE(top.get(), root.getChildAt(0).get());
        EXPECT_EQ("Top", top->getComponentName());
        EXPECT_EQ("DummyComponent", top->getComponentType());

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
    EXPECT_EQ(created->getComponentType(), DummyComponent::componentType);

    delete created;
}

TEST(ECSBaseTests, FindFirstChildOfByTypeAndName)
{
    DummyComponent root("Root");

    (void)root.addChildComponent<DummyComponent>("A");
    (void)root.addChildComponent<DummyComponent>("B");

    auto* foundA = root.findFirstChildOf<DummyComponent>("A");
    ASSERT_NE(nullptr, foundA);
    EXPECT_EQ(foundA->getComponentName(), "A");

    auto* foundB = root.findFirstChildOf<DummyComponent>("B");
    ASSERT_NE(nullptr, foundB);
    EXPECT_EQ(foundB->getComponentName(), "B");

    EXPECT_EQ(nullptr, root.findFirstChildOf<DummyComponent>("DoesNotExist"));
}

TEST(ECSBaseTests, AddUniqueTypeChildComponentReturnsSameInstance)
{
    DummyComponent root("Root");

    auto* first = root.addUniqueTypeChildComponent<DummyComponent>("A");
    auto* second = root.addUniqueTypeChildComponent<DummyComponent>("B"); // should not add another

    ASSERT_NE(nullptr, first);
    ASSERT_NE(nullptr, second);
    EXPECT_EQ(first, second);
    EXPECT_EQ(1, root.getChildrenCount());
}

TEST(ECSBaseTests, GetOrAddChildComponentReturnsExisting)
{
    DummyComponent root("Root");

    auto* first = root.getOrAddChildComponent<DummyComponent>("A");
    auto* second = root.getOrAddChildComponent<DummyComponent>("B");

    ASSERT_NE(nullptr, first);
    ASSERT_NE(nullptr, second);
    EXPECT_EQ(first, second);
    EXPECT_EQ(1, root.getChildrenCount());
}

TEST(ECSBaseTests, AttachChildClonesAndSetsParent)
{
    DummyComponent root("Root");

    DummyComponent::Ptr externalChild = new DummyComponent("Child");
    ASSERT_FALSE(externalChild->hasParent());

    BaseComponent* attached = root.attachChild(externalChild);
    ASSERT_NE(nullptr, attached);

    EXPECT_NE(attached, externalChild.get()); // must be a clone
    EXPECT_EQ(attached->getComponentType(), externalChild->getComponentType());
    EXPECT_EQ(attached->getComponentName(), externalChild->getComponentName());
    EXPECT_EQ(attached->getParent(), &root);
    EXPECT_EQ(1, root.getChildrenCount());
}

TEST(ECSBaseTests, DetachChildRemovesFromChildrenList)
{
    DummyComponent root("Root");

    auto* child = root.addChildComponent<DummyComponent>("Child");
    ASSERT_NE(nullptr, child);
    ASSERT_EQ(1, root.getChildrenCount());

    root.detachChild(child);
    EXPECT_EQ(0, root.getChildrenCount());
    EXPECT_FALSE(root.hasChildren());
}

TEST(ECSBaseTests, RemoveChildOfRemovesAllMatchingTypes)
{
    DummyComponent root("Root");

    (void)root.addChildComponent<DummyComponent>("A");
    (void)root.addChildComponent<DummyComponent>("B");
    ASSERT_TRUE(root.hasChildrenAs<DummyComponent>());

    root.removeChildOf<DummyComponent>();

    EXPECT_EQ(0, root.getChildrenCount());
    EXPECT_FALSE(root.hasChildrenAs<DummyComponent>());
}

TEST(ECSBaseTests, GetOwnerReturnsTopmostParent)
{
    DummyComponent root("Root");
    auto* child = root.addChildComponent<DummyComponent>("Child");
    auto* grandChild = child->addChildComponent<DummyComponent>("GrandChild");

    ASSERT_NE(nullptr, grandChild);

    EXPECT_EQ(grandChild->getOwner(), &root);
    EXPECT_EQ(child->getOwner(), &root);
    EXPECT_EQ(root.getOwner(), &root);
}

TEST(ECSBaseTests, MakeHashDependsOnParentChain)
{
    DummyComponent rootA("RootA");
    DummyComponent rootB("RootB");

    auto* childA = rootA.addChildComponent<DummyComponent>("Child");
    auto* childB = rootB.addChildComponent<DummyComponent>("Child");

    ASSERT_NE(nullptr, childA);
    ASSERT_NE(nullptr, childB);

    EXPECT_NE(childA->makeHash(), childB->makeHash());
}

TEST(ECSBaseTests, InitializeIsCalledAndPropagatesToChildren)
{
    InitSpyComponent root("Root");
    EXPECT_FALSE(root.isInitialized());

    auto* child = root.addChildComponent<InitSpyComponent>("Child");
    ASSERT_NE(nullptr, child);

    EXPECT_TRUE(root.isInitialized());
    EXPECT_TRUE(child->isInitialized());

    EXPECT_EQ(1, root.preInitCalls);
    EXPECT_EQ(1, root.initCalls);
    EXPECT_EQ(1, child->preInitCalls);
    EXPECT_EQ(1, child->initCalls);

    root.invalidate();
    EXPECT_FALSE(root.isInitialized());

    root.initialize();
    EXPECT_TRUE(root.isInitialized());

    EXPECT_EQ(2, root.preInitCalls);
    EXPECT_EQ(2, root.initCalls);

    EXPECT_EQ(1, child->preInitCalls);
    EXPECT_EQ(1, child->initCalls);
}
