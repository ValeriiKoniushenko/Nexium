add_subdirectory(ECS)
add_subdirectory(Misc)
add_subdirectory(RawDataManagement)


set(TARGET_NAME "__Nexium")

file(GLOB_RECURSE CPP_SOURCES
    CONFIGURE_DEPENDS
    "AssetsManager/*.cpp"
    "AssimpMisc/*.cpp"
    "Editor/*.cpp"
    "GameplaySystem/*.cpp"
    "Graphics/*.cpp"
    "InputDevices/*.cpp"
    "Scene/*.cpp"
)
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE CPP_HEADERS
    CONFIGURE_DEPENDS
    "AssetsManager/*.h"
    "AssimpMisc/*.h"
    "Editor/*.h"
    "GameplaySystem/*.h"
    "Graphics/*.h"
    "InputDevices/*.h"
    "Scene/*.h"
)
exclude_generated_files(FILTERED_HEADERS CPP_HEADERS)

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
    Utils::Core
    assimp::assimp
    nlohmann_json
    JustReflectMe::Adapter
    Core::Glad
    Core::ImGui
)

CoreAddCompileOptionsTo(${TARGET_NAME})
