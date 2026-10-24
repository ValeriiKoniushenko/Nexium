add_library(Nexium_Foundation_Debug STATIC)
add_library(Nexium::Foundation::Debug ALIAS Nexium_Foundation_Debug)
target_sources(Nexium_Foundation_Debug PRIVATE
    Latency.cpp
    LatencyExtractor.cpp
    LatencyTerminalPrinter.cpp
)
nexium_add_public_headers(Nexium_Foundation_Debug
    Foundation/Debug/Latency.h
    Foundation/Debug/LatencyExtractor.h
    Foundation/Debug/LatencyTerminalPrinter.h
)


nexium_configure_target(Nexium_Foundation_Debug)
add_dependencies(Nexium_Foundation_Debug CodeGenerator)
