add_executable(Main main.cpp)

target_link_libraries(Main PUBLIC SW::RawGraphics)
add_dependencies(Main deploy)