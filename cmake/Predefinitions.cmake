include_guard()

function(InstallDebugLevel)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_definitions(PW_DEBUG)
    endif()
endfunction()

function(InstallBenchmarkDependenciesTesting)
    set(BENCHMARK_ENABLE_TESTING OFF PARENT_SCOPE)
endfunction()