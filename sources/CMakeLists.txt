add_executable(Main main.cpp)

target_link_libraries(Main PUBLIC Utils::Utils SW::Glad glfw SW::BetterEnums)
add_dependencies(Main deploy)