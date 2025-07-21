include(FetchContent)

set(FETCHCONTENT_BASE_DIR deps)

FetchContent_Declare(Boost
        GIT_REPOSITORY "https://github.com/boostorg/boost.git"
        GIT_TAG boost-1.88.0
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE TRUE # needed to find correct Boost
        EXCLUDE_FROM_ALL # compile only what you need
)

FetchContent_MakeAvailable(Boost)

find_package(Boost 1.88.0 EXACT REQUIRED COMPONENTS filesystem)


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
)

FetchContent_MakeAvailable(Utils)
