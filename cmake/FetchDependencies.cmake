include(FetchContent)

function(installBoostModule module_name boost_version)
    string(PREPEND full_module_name sw_boost_)
    set(full_module_name "${full_module_name}${module_name}")

    FetchContent_Declare(${full_module_name}
            GIT_REPOSITORY "https://github.com/boostorg/${module_name}.git"
            GIT_TAG ${boost_version}
            GIT_PROGRESS TRUE
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(${full_module_name})
endfunction()


set(BOOST_VERSION boost-1.88.0)
set(BOOST_MODULES
        assert
        static_assert
        cmake
        config
        core
        headers
        throw_exception
        smart_ptr
)

foreach (module IN LISTS BOOST_MODULES)
    installBoostModule(${module} ${BOOST_VERSION})
endforeach ()


FetchContent_Declare(NlohmannJson
        GIT_REPOSITORY "https://github.com/nlohmann/json.git"
        GIT_TAG v3.12.0
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(NlohmannJson)


FetchContent_Declare(Glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(Glfw)


FetchContent_Declare(SpdLog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.3
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SpdLog)


FetchContent_Declare(Assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v6.0.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_BUILD_ASSIMP_TOOLS ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)

FetchContent_MakeAvailable(Assimp)


FetchContent_Declare(Utils
    GIT_REPOSITORY https://github.com/ValeriiKoniushenko/Utils.git
    GIT_TAG origin/develop
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(Utils)
