get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TARGET_NAME "__${CURRENT_DIR_NAME}")
set(CLEAR_TARGET_NAME "${CURRENT_DIR_NAME}")

file(GLOB_RECURSE SOURCES
    AssimpHelper.cpp
    BaseLog.cpp
    Configs.cpp
    DataStream.cpp
    FPSCounter.cpp
    JsonAdapter.cpp
    JsonCacheable.cpp
)
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)

add_library(${TARGET_NAME} STATIC ${SOURCES})
add_library(Nexium::${CLEAR_TARGET_NAME} ALIAS ${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PUBLIC
    Utils::Core
    Core::ImGui
    spdlog::spdlog
    nlohmann_json
    assimp::assimp
    JustReflectMe::Adapter
)
#target_compile_definitions(${TARGET_NAME} PRIVATE _POSIX_C_SOURCE=200809L)

CoreAddCompileOptionsTo(${TARGET_NAME})
