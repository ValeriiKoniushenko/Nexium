---
name: ecs-benchmarks
description: Build and run the ECS Google Benchmark suite when changing component-tree behavior or investigating ECS performance.
---

# ECS Benchmarks

Use this suite for performance work on `Core::BaseComponent` operations. The target is
`Nexium_ECS_Benchmarks`, built from `benchmarks/ECS/ECSBenchmarks.cpp` and written to
`build/bin/`.

## Build

Run the normal project build first:

```bash
cmake --build build -j$(nproc)
```

To rebuild only the suite while iterating:

```bash
cmake --build build -j$(nproc) --target Nexium_ECS_Benchmarks
```

## Run

List benchmark names before choosing a focused run:

```bash
build/bin/Nexium_ECS_Benchmarks --benchmark_list_tests
```

Use a regular-expression filter and enough repetitions to distinguish a real result from
normal machine variance:

```bash
build/bin/Nexium_ECS_Benchmarks \
  --benchmark_filter='BM_ECS_(BreadthFirstTraversal|DepthFirstTraversal)' \
  --benchmark_repetitions=5 \
  --benchmark_report_aggregates_only=true
```

Run the complete suite when establishing a baseline or after a broad ECS change:

```bash
build/bin/Nexium_ECS_Benchmarks --benchmark_repetitions=3
```

## Coverage

The suite measures component spawn/despawn, tree and gameplay-scene construction, BFS/DFS,
best- and worst-case lookup, get-or-add, unique-component lookup, attach/clone, deep removal,
factory creation, initialization, active/mixed ticking, and tree serialization/deserialization.

Benchmark fixtures must keep world construction, fixture reset, and other unrelated work outside
the timed region with `PauseTiming`/`ResumeTiming`. Report work using
`SetItemsProcessed` when a benchmark scales with entities or components. Use static `StringAtom`
values for repeated component names and types so string allocation does not contaminate ECS
measurements.

The benchmark-only components are deliberately not JRM-reflected. If a benchmark needs a new
engine component or changes an annotated engine header, follow the `codegen` skill before building.
