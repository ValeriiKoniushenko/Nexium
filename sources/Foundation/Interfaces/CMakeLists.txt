add_library(Nexium_Foundation_Interfaces STATIC)
add_library(Nexium::Foundation::Interfaces ALIAS Nexium_Foundation_Interfaces)
target_sources(Nexium_Foundation_Interfaces PRIVATE
    DataStream.cpp
)
nexium_add_public_headers(Nexium_Foundation_Interfaces
    Foundation/Interfaces/DataStream.h
    Foundation/Interfaces/IOutliner.h
    Foundation/Interfaces/IOutliner.generated.h
)

target_link_libraries(Nexium_Foundation_Interfaces
    PUBLIC
    JustReflectMe::Adapter
    Utils::Core
    PRIVATE
    Nexium::Foundation
)

nexium_configure_target(Nexium_Foundation_Interfaces)
add_dependencies(Nexium_Foundation_Interfaces CodeGenerator)
