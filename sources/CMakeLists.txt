
# file(GLOB_RECURSE CPP_SOURCES "*.cpp")
# set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)
# 
# file(GLOB_RECURSE HEADERS "*.h")
# 
# add_library(Nexium_Core ${CPP_SOURCES} ${HEADERS})
# add_library(Nexium::Core ALIAS Nexium_Core)
# 
# target_link_libraries(Nexium_Core PUBLIC
#     Utils::Core
#     Core::Glad
#     Core::ImGui
#     glfw
#     spdlog::spdlog
#     assimp::assimp
#     nlohmann_json
#     JustReflectMe::Adapter
# )
# 
# target_include_directories(Nexium_Core PUBLIC .)
# 
# add_custom_target(CodeGenerator ALL
#     COMMAND jrm ${CMAKE_SOURCE_DIR}
# )
# add_dependencies(Nexium_Core CodeGenerator)
# 
# CoreAddCompileOptionsTo(Nexium_Core)

# Standalone ECS build target (independent from `Nexium::Core`).

add_subdirectory(GameplaySystem)
add_subdirectory(Graphics)
add_subdirectory(InputDevices)
add_subdirectory(Misc)
