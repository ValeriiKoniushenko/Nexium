
file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE HEADERS "*.h")

add_library(Nexium_Core ${CPP_SOURCES} ${HEADERS})
add_library(Nexium::Core ALIAS Nexium_Core)

target_link_libraries(Nexium_Core PUBLIC
    Utils::Core
    Core::Glad
    Core::ImGui
    glfw
    spdlog::spdlog
    assimp::assimp
    nlohmann_json
)

target_include_directories(Nexium_Core PUBLIC .)

CoreAddCompileOptionsTo(Nexium_Core)