add_subdirectory(dependencies)

add_executable(Nexium_ECS_Benchmarks
    ECS/ECSBenchmarks.cpp
)

target_link_libraries(Nexium_ECS_Benchmarks
    PRIVATE
    Nexium::Fundamental::ECS
    Nexium::BenchmarkDependencies
)

add_dependencies(Nexium_ECS_Benchmarks CodeGenerator)
nexium_configure_target(Nexium_ECS_Benchmarks)
