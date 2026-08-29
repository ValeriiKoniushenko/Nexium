add_subdirectory(dependencies)

add_executable(Nexium_ECS_Benchmarks
    ECS/ECSBenchmarks.cpp
)

target_link_libraries(Nexium_ECS_Benchmarks
    PRIVATE
    Nexium::Core
    Nexium::BenchmarkDependencies
)

add_dependencies(Nexium_ECS_Benchmarks CodeGenerator)
CoreAddCompileOptionsTo(Nexium_ECS_Benchmarks)
