# ============== GLFW/GLAD ===============

set(SOURCES
    Graphics.cpp
)

set(HEADERS
    Graphics.h
)

add_library(Nexium_Platform_Graphics STATIC)
add_library(Nexium::Platform::Graphics ALIAS Nexium_Platform_Graphics)
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)
target_sources(Nexium_Platform_Graphics PRIVATE ${SOURCES} PUBLIC ${HEADERS})

target_link_libraries(Nexium_Platform_Graphics PUBLIC glfw Core::Glad)
target_include_directories(Nexium_Platform_Graphics PUBLIC ../)

# ================ INPUT =================
set(SOURCES
    Keyboard.cpp
    Mouse.cpp
    Window.cpp
)

set(HEADERS
    Keyboard.h
    Mouse.h
    Window.h
)

add_library(Nexium_Platform_Input STATIC)
add_library(Nexium::Platform::Input ALIAS Nexium_Platform_Input)
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)
target_sources(Nexium_Platform_Input PRIVATE ${SOURCES} PUBLIC ${HEADERS})

target_include_directories(Nexium_Platform_Input PUBLIC ../)
target_link_libraries(Nexium_Platform_Input
    PUBLIC
    Nexium::Foundation
    Nexium::Platform::Graphics
    PRIVATE
    JustReflectMe::Adapter
)

CoreAddCompileOptionsTo(Nexium_Platform_Input)
