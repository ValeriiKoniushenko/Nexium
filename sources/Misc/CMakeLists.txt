
get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TARGET_NAME "__Nexium_Core_${FOLDER_NAME}")

add_library(${TARGET_NAME}
    AssimpHelper.cpp
    BaseLog.cpp
    Configs.cpp
    DataStream.cpp
    FPSCounter.cpp
    JsonAdapter.cpp
    JsonCacheable.cpp
)

add_library(Nexium::${FOLDER_NAME} ALIAS ${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PRIVATE
    Utils::Core
    spdlog::spdlog
    assimp::assimp
    nlohmann_json
)

target_include_directories(${TARGET_NAME} PUBLIC ../)

CoreAddCompileOptionsTo(${TARGET_NAME})