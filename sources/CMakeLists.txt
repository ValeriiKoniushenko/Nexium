add_subdirectory(Misc)
add_subdirectory(RawGraphics)

add_executable(SomeGameApp SomeGameApp.cpp)
target_link_libraries(SomeGameApp PUBLIC
        SW::RawGraphics
)