include(FetchContent)

set(FETCHCONTENT_BASE_DIR deps)

FetchContent_Declare(Utils
        GIT_REPOSITORY https://github.com/ValeriiKoniushenko/Utils.git
        GIT_TAG origin/develop
)

FetchContent_MakeAvailable(Utils)
FetchContent_GetProperties(Utils SOURCE_DIR Utils_SOURCES)

list(APPEND CMAKE_MODULE_PATH "${Utils_SOURCES}/cmake/FetchDependencies.cmake")
include(${Utils_SOURCES}/cmake/FetchDependencies.cmake)


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


# FetchContent_Declare(Assimp
#         GIT_REPOSITORY https://github.com/assimp/assimp.git
#         GIT_TAG v6.0.2
#         GIT_SHALLOW TRUE
#         GIT_PROGRESS TRUE
# )
# set(ASSIMP_BUILD_ZLIB ON)
# set(ASSIMP_BUILD_ASSIMP_TOOLS ON)
# set(ASSIMP_BUILD_TESTS OFF)
#
# FetchContent_MakeAvailable(Assimp)