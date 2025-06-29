add_executable(Main main.cpp)

target_link_libraries(Main PUBLIC Utils::Core SW::Glad glfw)