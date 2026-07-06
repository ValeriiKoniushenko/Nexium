add_subdirectory(Misc)

set(TARGET_NAME "__Nexium")

file(GLOB_RECURSE CPP_SOURCES
    CONFIGURE_DEPENDS
    "AssetsManager/*.cpp"
    "AssimpMisc/*.cpp"
    "ECS/*.cpp"
    "Editor/*.cpp"
    "GameplaySystem/*.cpp"
    "Graphics/*.cpp"
    "InputDevices/*.cpp"
    "ResourceManagement/*.cpp"
    "Scene/*.cpp"
)
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE CPP_HEADERS
    CONFIGURE_DEPENDS
    "AssetsManager/*.h"
    "AssimpMisc/*.h"
    "ECS/*.h"
    "Editor/*.h"
    "GameplaySystem/*.h"
    "Graphics/*.h"
    "InputDevices/*.h"
    "ResourceManagement/*.h"
    "Scene/*.h"
)

add_library(${TARGET_NAME} STATIC)
target_sources(${TARGET_NAME}
    PRIVATE
    ${CPP_SOURCES}
    PUBLIC
    ${FILTERED_HEADERS}
)

add_dependencies(${TARGET_NAME} CodeGenerator)

add_library(Nexium::Core ALIAS ${TARGET_NAME})

target_include_directories(${TARGET_NAME} PUBLIC .)

target_link_libraries(${TARGET_NAME} PUBLIC
    ${GRAPHICS_BACKEND_LIBS}
    Core::ImGui
    Core::Rectpack2D

    Utils::Core

    Nexium::Misc

    assimp::assimp
    nlohmann_json
    JustReflectMe::Adapter
)

CoreAddCompileOptionsTo(${TARGET_NAME})
