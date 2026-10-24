add_library(Nexium_Platform_Resources INTERFACE)
add_library(Nexium::Platform::Resources ALIAS Nexium_Platform_Resources)
nexium_add_public_headers(Nexium_Platform_Resources
    Platform/Resources/Resources.h
)
