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

#include "ECS/BaseComponent.h"

#include <benchmark/benchmark.h>
#include <cstddef>
#include <mutex>
#include <vector>

using namespace Core;

// clang-format off
/*
-------------------------------------------------------------------------------------------------
Benchmark                                       Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------------------------
BM_ECS_ComponentSpawnDespawn/64              1223 ns         1219 ns       557411 items_per_second=52.4986M/s
BM_ECS_ComponentSpawnDespawn/256             5236 ns         5220 ns       134715 items_per_second=49.0436M/s
BM_ECS_ComponentSpawnDespawn/1024           21266 ns        21204 ns        32909 items_per_second=48.2923M/s
BM_ECS_ComponentSpawnDespawn/4096          233091 ns       232167 ns         3015 items_per_second=17.6424M/s
BM_ECS_HierarchyConstruction/64              2196 ns         2189 ns       315381 items_per_second=29.2312M/s
BM_ECS_HierarchyConstruction/256             8129 ns         8105 ns        85862 items_per_second=31.5848M/s
BM_ECS_HierarchyConstruction/1024           28417 ns        28337 ns        24670 items_per_second=36.1367M/s
BM_ECS_HierarchyConstruction/4096          230625 ns       229692 ns         3039 items_per_second=17.8326M/s
BM_ECS_GameplaySceneConstruction/16          2245 ns         2241 ns       313380 items_per_second=7.14097M/s
BM_ECS_GameplaySceneConstruction/64          7600 ns         7580 ns        93718 items_per_second=8.44347M/s
BM_ECS_GameplaySceneConstruction/256        28031 ns        27952 ns        24899 items_per_second=9.15867M/s
BM_ECS_GameplaySceneConstruction/1024      111799 ns       111422 ns         6294 items_per_second=9.19025M/s
BM_ECS_BreadthFirstTraversal/64              1836 ns         1831 ns       368535 items_per_second=34.4147M/s
BM_ECS_BreadthFirstTraversal/256             6620 ns         6599 ns       106260 items_per_second=38.6419M/s
BM_ECS_BreadthFirstTraversal/1024           27250 ns        27163 ns        25707 items_per_second=37.6622M/s
BM_ECS_BreadthFirstTraversal/4096          123117 ns       122678 ns         5582 items_per_second=33.38M/s
BM_ECS_DepthFirstTraversal/64                1813 ns         1807 ns       364373 items_per_second=34.8708M/s
BM_ECS_DepthFirstTraversal/256               7103 ns         7083 ns        98747 items_per_second=36.002M/s
BM_ECS_DepthFirstTraversal/1024             28058 ns        27970 ns        25272 items_per_second=36.5748M/s
BM_ECS_DepthFirstTraversal/4096            118120 ns       117713 ns         6012 items_per_second=34.788M/s
BM_ECS_ComponentLookupBestCase/64            1933 ns         1926 ns       395544
BM_ECS_ComponentLookupBestCase/256           6724 ns         6703 ns       104838
BM_ECS_ComponentLookupBestCase/1024         26288 ns        26208 ns        26650
BM_ECS_ComponentLookupBestCase/4096        108309 ns       107943 ns         6614
BM_ECS_ComponentLookupWorstCase/64           2436 ns         2429 ns       289615
BM_ECS_ComponentLookupWorstCase/256          8943 ns         8916 ns        78519
BM_ECS_ComponentLookupWorstCase/1024        35061 ns        34955 ns        19720
BM_ECS_ComponentLookupWorstCase/4096       144087 ns       143535 ns         4819
BM_ECS_GetOrAddExistingComponent/64          2457 ns         2449 ns       280413
BM_ECS_GetOrAddExistingComponent/256         8998 ns         8970 ns        78255
BM_ECS_GetOrAddExistingComponent/1024       35285 ns        35175 ns        19834
BM_ECS_GetOrAddExistingComponent/4096      147238 ns       146691 ns         4815
BM_ECS_AddUniqueExistingComponent/64         1469 ns         1464 ns       483456
BM_ECS_AddUniqueExistingComponent/256        5869 ns         5852 ns       119893
BM_ECS_AddUniqueExistingComponent/1024      23520 ns        23456 ns        29930
BM_ECS_AddUniqueExistingComponent/4096      93772 ns        93509 ns         7483
BM_ECS_AttachAndCloneTree/64                 2106 ns         2103 ns       335460 items_per_second=30.4291M/s
BM_ECS_AttachAndCloneTree/256                5585 ns         5576 ns       124452 items_per_second=45.9118M/s
BM_ECS_AttachAndCloneTree/1024              18825 ns        18767 ns        36870 items_per_second=54.5646M/s
BM_ECS_AttachAndCloneTree/4096             177979 ns       177030 ns         3943 items_per_second=23.1373M/s
BM_ECS_RemoveDeepDescendant/64               3763 ns         3754 ns       189863 items_per_second=16.7806M/s
BM_ECS_RemoveDeepDescendant/256             11018 ns        10985 ns        64346 items_per_second=23.2128M/s
BM_ECS_RemoveDeepDescendant/1024            37627 ns        37500 ns        18501 items_per_second=27.2797M/s
BM_ECS_RemoveDeepDescendant/4096           148356 ns       147797 ns         4758 items_per_second=27.707M/s
BM_ECS_FactoryCreate                          531 ns          530 ns      1328839
BM_ECS_InitializeInvalidatedTree/64           668 ns          669 ns      1052564 items_per_second=95.7285M/s
BM_ECS_InitializeInvalidatedTree/256         1074 ns         1068 ns       656556 items_per_second=239.728M/s
BM_ECS_InitializeInvalidatedTree/1024        2736 ns         2720 ns       259123 items_per_second=376.486M/s
BM_ECS_InitializeInvalidatedTree/4096       10889 ns        10821 ns        64449 items_per_second=378.528M/s
BM_ECS_TickActiveGameplayScene/16             109 ns          109 ns      6422631 items_per_second=524.345M/s
BM_ECS_TickActiveGameplayScene/64             524 ns          522 ns      1000000 items_per_second=430.841M/s
BM_ECS_TickActiveGameplayScene/256           2128 ns         2122 ns       331940 items_per_second=422.699M/s
BM_ECS_TickActiveGameplayScene/1024          9134 ns         9101 ns        76993 items_per_second=393.922M/s
BM_ECS_TickMixedGameplayScene/16             72.1 ns         71.9 ns      9571598 items_per_second=792.766M/s
BM_ECS_TickMixedGameplayScene/64              358 ns          356 ns      2139266 items_per_second=631.203M/s
BM_ECS_TickMixedGameplayScene/256            1575 ns         1569 ns       446955 items_per_second=571.62M/s
BM_ECS_TickMixedGameplayScene/1024           6892 ns         6865 ns       105780 items_per_second=522.191M/s
BM_ECS_SerializeTree/64                     67310 ns        67092 ns        10488 items_per_second=953.908k/s
BM_ECS_SerializeTree/256                   323547 ns       322539 ns         2173 items_per_second=793.703k/s
BM_ECS_SerializeTree/1024                 1699268 ns      1691458 ns          415 items_per_second=605.395k/s
BM_ECS_DeserializeTree/64                   61626 ns        61386 ns        11429 items_per_second=1.04258M/s
BM_ECS_DeserializeTree/256                 302578 ns       301625 ns         2314 items_per_second=848.736k/s
BM_ECS_DeserializeTree/1024               1413447 ns      1407603 ns          501 items_per_second=727.478k/s
*/
// clang-format on

namespace
{
    class BenchmarkNode : public BaseComponent
    {
    public:
        using Ptr = IntrusivePtr<BenchmarkNode>;

        static const StringAtom componentType;

        explicit BenchmarkNode(const StringAtom& name = ""_atom)
            : BaseComponent(componentType, name)
        {
        }

        [[nodiscard]] BaseComponent::Ptr clone() const override
        {
            auto* out = new BenchmarkNode(*this);
            out->invalidate();
            return out;
        }

        [[nodiscard]] float getTickAccumulator() const noexcept { return _tickAccumulator; }
        [[nodiscard]] std::size_t getInitializationCount() const noexcept
        {
            return _initializationCount;
        }

    protected:
        BenchmarkNode(const StringAtom& type, const StringAtom& name)
            : BaseComponent(type, name)
        {
        }

        void onTick(float delta) override
        {
            _tickAccumulator += delta;
            BaseComponent::onTick(delta);
        }

        void onInitialize() override { ++_initializationCount; }

    private:
        float _tickAccumulator = 0.0F;
        std::size_t _initializationCount = 0;
    };

    class TransformBenchmarkComponent final : public BenchmarkNode
    {
    public:
        using Ptr = IntrusivePtr<TransformBenchmarkComponent>;

        static const StringAtom componentType;

        explicit TransformBenchmarkComponent(const StringAtom& name = ""_atom)
            : BenchmarkNode(componentType, name)
        {
        }

        [[nodiscard]] BaseComponent::Ptr clone() const override
        {
            auto* out = new TransformBenchmarkComponent(*this);
            out->invalidate();
            return out;
        }
    };

    class RenderBenchmarkComponent final : public BenchmarkNode
    {
    public:
        using Ptr = IntrusivePtr<RenderBenchmarkComponent>;

        static const StringAtom componentType;

        explicit RenderBenchmarkComponent(const StringAtom& name = ""_atom)
            : BenchmarkNode(componentType, name)
        {
        }

        [[nodiscard]] BaseComponent::Ptr clone() const override
        {
            auto* out = new RenderBenchmarkComponent(*this);
            out->invalidate();
            return out;
        }
    };

    class ScriptBenchmarkComponent final : public BenchmarkNode
    {
    public:
        using Ptr = IntrusivePtr<ScriptBenchmarkComponent>;

        static const StringAtom componentType;

        explicit ScriptBenchmarkComponent(const StringAtom& name = ""_atom)
            : BenchmarkNode(componentType, name)
        {
        }

        [[nodiscard]] BaseComponent::Ptr clone() const override
        {
            auto* out = new ScriptBenchmarkComponent(*this);
            out->invalidate();
            return out;
        }
    };

    const StringAtom BenchmarkNode::componentType = "EcsBenchmarkNode"_atom;
    const StringAtom TransformBenchmarkComponent::componentType = "EcsBenchmarkTransform"_atom;
    const StringAtom RenderBenchmarkComponent::componentType = "EcsBenchmarkRender"_atom;
    const StringAtom ScriptBenchmarkComponent::componentType = "EcsBenchmarkScript"_atom;

    struct BenchmarkTree
    {
        BenchmarkNode::Ptr root = new BenchmarkNode("Root"_atom);
        std::vector<BenchmarkNode*> nodes{ root.get() };
    };

    void registerBenchmarkComponentTypes()
    {
        static std::once_flag registrationFlag;
        std::call_once(registrationFlag,
                       []()
                       {
                           auto& factory = GetGlobalComponentFactory();
                           factory.registerNewType<BenchmarkNode>(BenchmarkNode::componentType);
                           factory.registerNewType<TransformBenchmarkComponent>(
                               TransformBenchmarkComponent::componentType);
                           factory.registerNewType<RenderBenchmarkComponent>(
                               RenderBenchmarkComponent::componentType);
                           factory.registerNewType<ScriptBenchmarkComponent>(
                               ScriptBenchmarkComponent::componentType);
                       });
    }

    BenchmarkTree buildBalancedTree(std::size_t nodeCount, std::size_t branchFactor = 8)
    {
        BenchmarkTree tree;
        tree.nodes.reserve(nodeCount);

        for (std::size_t nodeIndex = 1; nodeIndex < nodeCount; ++nodeIndex)
        {
            const auto parentIndex = (nodeIndex - 1) / branchFactor;
            auto* child = tree.nodes[parentIndex]->addChildComponent<BenchmarkNode>("Node"_atom);
            tree.nodes.push_back(child);
        }

        return tree;
    }

    BenchmarkTree buildGameplayScene(std::size_t entityCount)
    {
        BenchmarkTree scene;
        scene.nodes.reserve(1 + entityCount * 4);

        for (std::size_t entityIndex = 0; entityIndex < entityCount; ++entityIndex)
        {
            auto* entity = scene.root->addChildComponent<BenchmarkNode>("Entity"_atom);
            scene.nodes.push_back(entity);
            scene.nodes.push_back(
                entity->addChildComponent<TransformBenchmarkComponent>("Transform"_atom));
            scene.nodes.push_back(
                entity->addChildComponent<RenderBenchmarkComponent>("Renderer"_atom));

            if (entityIndex % 2 == 0)
            {
                scene.nodes.push_back(
                    entity->addChildComponent<ScriptBenchmarkComponent>("Script"_atom));
            }
        }

        return scene;
    }

    void configureMixedActivity(BenchmarkTree& scene)
    {
        auto& entities = scene.root->getChildren();
        for (std::size_t entityIndex = 0; entityIndex < entities.size(); ++entityIndex)
        {
            if (entityIndex % 4 == 0)
            {
                entities[entityIndex]->setNoTick(true);
            }
        }
    }

    void BM_ECS_ComponentSpawnDespawn(benchmark::State& state)
    {
        const auto componentCount = static_cast<std::size_t>(state.range(0));

        for (auto _ : state)
        {
            std::vector<BenchmarkNode::Ptr> components;
            components.reserve(componentCount);

            for (std::size_t componentIndex = 0; componentIndex < componentCount; ++componentIndex)
            {
                components.emplace_back(new BenchmarkNode("Entity"_atom));
            }

            benchmark::DoNotOptimize(components.data());
        }

        state.SetItemsProcessed(state.iterations() * componentCount);
    }

    void BM_ECS_HierarchyConstruction(benchmark::State& state)
    {
        const auto nodeCount = static_cast<std::size_t>(state.range(0));
        BenchmarkTree tree;

        for (auto _ : state)
        {
            tree = buildBalancedTree(nodeCount);
            benchmark::DoNotOptimize(tree.root.get());

            state.PauseTiming();
            tree = BenchmarkTree{};
            state.ResumeTiming();
        }

        state.SetItemsProcessed(state.iterations() * nodeCount);
    }

    void BM_ECS_GameplaySceneConstruction(benchmark::State& state)
    {
        const auto entityCount = static_cast<std::size_t>(state.range(0));
        BenchmarkTree scene;

        for (auto _ : state)
        {
            scene = buildGameplayScene(entityCount);
            benchmark::DoNotOptimize(scene.root.get());

            state.PauseTiming();
            scene = BenchmarkTree{};
            state.ResumeTiming();
        }

        state.SetItemsProcessed(state.iterations() * entityCount);
    }

    void BM_ECS_BreadthFirstTraversal(benchmark::State& state)
    {
        const auto nodeCount = static_cast<std::size_t>(state.range(0));
        const auto tree = buildBalancedTree(nodeCount);

        for (auto _ : state)
        {
            std::size_t visited = 0;
            tree.root->forEach([&visited](const BaseComponent*) { ++visited; });
            benchmark::DoNotOptimize(visited);
        }

        state.SetItemsProcessed(state.iterations() * (nodeCount - 1));
    }

    void BM_ECS_DepthFirstTraversal(benchmark::State& state)
    {
        const auto nodeCount = static_cast<std::size_t>(state.range(0));
        const auto tree = buildBalancedTree(nodeCount);

        for (auto _ : state)
        {
            std::size_t visited = 0;
            tree.root->forEachDFS([&visited](const BaseComponent*) { ++visited; });
            benchmark::DoNotOptimize(visited);
        }

        state.SetItemsProcessed(state.iterations() * (nodeCount - 1));
    }

    void BM_ECS_ComponentLookupBestCase(benchmark::State& state)
    {
        const auto childCount = static_cast<std::size_t>(state.range(0));
        BenchmarkTree tree;
        tree.nodes.reserve(childCount + 1);
        tree.nodes.push_back(tree.root->addChildComponent<RenderBenchmarkComponent>("Target"_atom));
        for (std::size_t childIndex = 1; childIndex < childCount; ++childIndex)
        {
            tree.nodes.push_back(tree.root->addChildComponent<BenchmarkNode>("Entity"_atom));
        }

        for (auto _ : state)
        {
            auto* found = tree.root->findFirstChildOf<RenderBenchmarkComponent>("Target"_atom);
            benchmark::DoNotOptimize(found);
        }
    }

    void BM_ECS_ComponentLookupWorstCase(benchmark::State& state)
    {
        const auto childCount = static_cast<std::size_t>(state.range(0));
        BenchmarkTree tree;
        tree.nodes.reserve(childCount + 1);
        for (std::size_t childIndex = 1; childIndex < childCount; ++childIndex)
        {
            tree.nodes.push_back(tree.root->addChildComponent<BenchmarkNode>("Entity"_atom));
        }
        tree.nodes.push_back(tree.root->addChildComponent<RenderBenchmarkComponent>("Target"_atom));

        for (auto _ : state)
        {
            auto* found = tree.root->findFirstChildOf<RenderBenchmarkComponent>("Target"_atom);
            benchmark::DoNotOptimize(found);
        }
    }

    void BM_ECS_GetOrAddExistingComponent(benchmark::State& state)
    {
        const auto childCount = static_cast<std::size_t>(state.range(0));
        BenchmarkTree tree;
        tree.nodes.reserve(childCount + 1);
        for (std::size_t childIndex = 1; childIndex < childCount; ++childIndex)
        {
            tree.nodes.push_back(tree.root->addChildComponent<BenchmarkNode>("Entity"_atom));
        }
        tree.nodes.push_back(
            tree.root->addChildComponent<RenderBenchmarkComponent>("Renderer"_atom));

        for (auto _ : state)
        {
            auto* component
                = tree.root->getOrAddChildComponent<RenderBenchmarkComponent>("Renderer"_atom);
            benchmark::DoNotOptimize(component);
        }
    }

    void BM_ECS_AddUniqueExistingComponent(benchmark::State& state)
    {
        const auto childCount = static_cast<std::size_t>(state.range(0));
        BenchmarkTree tree;
        tree.nodes.reserve(childCount + 1);
        for (std::size_t childIndex = 1; childIndex < childCount; ++childIndex)
        {
            tree.nodes.push_back(tree.root->addChildComponent<BenchmarkNode>("Entity"_atom));
        }
        tree.nodes.push_back(
            tree.root->addChildComponent<RenderBenchmarkComponent>("Renderer"_atom));

        for (auto _ : state)
        {
            auto* component
                = tree.root->addUniqueTypeChildComponent<RenderBenchmarkComponent>("Renderer"_atom);
            benchmark::DoNotOptimize(component);
        }
    }

    void BM_ECS_AttachAndCloneTree(benchmark::State& state)
    {
        const auto nodeCount = static_cast<std::size_t>(state.range(0));
        const auto blueprint = buildBalancedTree(nodeCount);
        BenchmarkNode::Ptr destination;

        for (auto _ : state)
        {
            state.PauseTiming();
            destination = new BenchmarkNode("Destination"_atom);
            state.ResumeTiming();

            auto* attached = destination->attachChild(blueprint.root);
            benchmark::DoNotOptimize(attached);

            state.PauseTiming();
            destination = nullptr;
            state.ResumeTiming();
        }

        state.SetItemsProcessed(state.iterations() * nodeCount);
    }

    void BM_ECS_RemoveDeepDescendant(benchmark::State& state)
    {
        const auto nodeCount = static_cast<std::size_t>(state.range(0));
        BenchmarkTree tree;

        for (auto _ : state)
        {
            state.PauseTiming();
            tree = buildBalancedTree(nodeCount);
            auto* target = tree.nodes.back();
            state.ResumeTiming();

            bool removed = tree.root->removeChildDeep(target);
            benchmark::DoNotOptimize(removed);

            state.PauseTiming();
            tree = BenchmarkTree{};
            state.ResumeTiming();
        }

        state.SetItemsProcessed(state.iterations() * (nodeCount - 1));
    }

    void BM_ECS_FactoryCreate(benchmark::State& state)
    {
        registerBenchmarkComponentTypes();
        auto& factory = GetGlobalComponentFactory();

        for (auto _ : state)
        {
            auto* component = factory.create(BenchmarkNode::componentType);
            benchmark::DoNotOptimize(component);

            state.PauseTiming();
            delete component;
            state.ResumeTiming();
        }
    }

    void BM_ECS_InitializeInvalidatedTree(benchmark::State& state)
    {
        const auto nodeCount = static_cast<std::size_t>(state.range(0));
        auto tree = buildBalancedTree(nodeCount);

        for (auto _ : state)
        {
            state.PauseTiming();
            tree.root->invalidate();
            tree.root->forEach([](BaseComponent* component) { component->invalidate(); });
            state.ResumeTiming();

            tree.root->initialize();
            benchmark::DoNotOptimize(tree.root->getInitializationCount());
        }

        state.SetItemsProcessed(state.iterations() * nodeCount);
    }

    void BM_ECS_TickActiveGameplayScene(benchmark::State& state)
    {
        const auto entityCount = static_cast<std::size_t>(state.range(0));
        auto scene = buildGameplayScene(entityCount);

        for (auto _ : state)
        {
            scene.root->tick(1.0F / 60.0F);
            benchmark::DoNotOptimize(scene.root->getTickAccumulator());
        }

        state.SetItemsProcessed(state.iterations() * scene.nodes.size());
    }

    void BM_ECS_TickMixedGameplayScene(benchmark::State& state)
    {
        const auto entityCount = static_cast<std::size_t>(state.range(0));
        auto scene = buildGameplayScene(entityCount);
        configureMixedActivity(scene);

        for (auto _ : state)
        {
            scene.root->tick(1.0F / 60.0F);
            benchmark::DoNotOptimize(scene.root->getTickAccumulator());
        }

        state.SetItemsProcessed(state.iterations() * scene.nodes.size());
    }

    void BM_ECS_SerializeTree(benchmark::State& state)
    {
        const auto nodeCount = static_cast<std::size_t>(state.range(0));
        const auto tree = buildBalancedTree(nodeCount);

        for (auto _ : state)
        {
            auto serialized = tree.root->serialize();
            benchmark::DoNotOptimize(serialized);
            benchmark::ClobberMemory();
        }

        state.SetItemsProcessed(state.iterations() * nodeCount);
    }

    void BM_ECS_DeserializeTree(benchmark::State& state)
    {
        registerBenchmarkComponentTypes();

        const auto nodeCount = static_cast<std::size_t>(state.range(0));
        const auto tree = buildBalancedTree(nodeCount);
        const auto serialized = tree.root->serialize();

        for (auto _ : state)
        {
            BenchmarkNode restored("Restored"_atom);
            RResourceStream<RJsonResourceStream> stream(serialized);
            restored.deserialize(stream);
            benchmark::DoNotOptimize(restored.getChildrenCount());
            benchmark::ClobberMemory();
        }

        state.SetItemsProcessed(state.iterations() * nodeCount);
    }
} // namespace

BENCHMARK(BM_ECS_ComponentSpawnDespawn)->RangeMultiplier(4)->Range(64, 4096);
BENCHMARK(BM_ECS_HierarchyConstruction)->RangeMultiplier(4)->Range(64, 4096);
BENCHMARK(BM_ECS_GameplaySceneConstruction)->RangeMultiplier(4)->Range(16, 1024);
BENCHMARK(BM_ECS_BreadthFirstTraversal)->RangeMultiplier(4)->Range(64, 4096);
BENCHMARK(BM_ECS_DepthFirstTraversal)->RangeMultiplier(4)->Range(64, 4096);
BENCHMARK(BM_ECS_ComponentLookupBestCase)->RangeMultiplier(4)->Range(64, 4096);
BENCHMARK(BM_ECS_ComponentLookupWorstCase)->RangeMultiplier(4)->Range(64, 4096);
BENCHMARK(BM_ECS_GetOrAddExistingComponent)->RangeMultiplier(4)->Range(64, 4096);
BENCHMARK(BM_ECS_AddUniqueExistingComponent)->RangeMultiplier(4)->Range(64, 4096);
BENCHMARK(BM_ECS_AttachAndCloneTree)->RangeMultiplier(4)->Range(64, 4096);
BENCHMARK(BM_ECS_RemoveDeepDescendant)->RangeMultiplier(4)->Range(64, 4096);
BENCHMARK(BM_ECS_FactoryCreate);
BENCHMARK(BM_ECS_InitializeInvalidatedTree)->RangeMultiplier(4)->Range(64, 4096);
BENCHMARK(BM_ECS_TickActiveGameplayScene)->RangeMultiplier(4)->Range(16, 1024);
BENCHMARK(BM_ECS_TickMixedGameplayScene)->RangeMultiplier(4)->Range(16, 1024);
BENCHMARK(BM_ECS_SerializeTree)->RangeMultiplier(4)->Range(64, 1024);
BENCHMARK(BM_ECS_DeserializeTree)->RangeMultiplier(4)->Range(64, 1024);

BENCHMARK_MAIN();
