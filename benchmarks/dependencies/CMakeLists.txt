# Google Benchmark is vendored by Utils and configured by the root dependency pipeline.
add_library(Nexium_BenchmarkDependencies INTERFACE)
add_library(Nexium::BenchmarkDependencies ALIAS Nexium_BenchmarkDependencies)

target_link_libraries(Nexium_BenchmarkDependencies
    INTERFACE
    benchmark::benchmark
)
