add_library(Nexium_Platform_Graphics STATIC)
add_library(Nexium::Platform::Graphics ALIAS Nexium_Platform_Graphics)
target_sources(Nexium_Platform_Graphics PRIVATE
    Graphics.cpp
)
nexium_add_public_headers(Nexium_Platform_Graphics
    Platform/Graphics/Graphics.h
)

target_link_libraries(Nexium_Platform_Graphics
    PUBLIC
    Core::Glad
)

nexium_configure_target(Nexium_Platform_Graphics)
add_dependencies(Nexium_Platform_Graphics CodeGenerator)
