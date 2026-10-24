add_library(Nexium_Subsystem_AssimpImporter STATIC)
add_library(Nexium::Subsystem::AssimpImporter ALIAS Nexium_Subsystem_AssimpImporter)
target_sources(Nexium_Subsystem_AssimpImporter PRIVATE
    AssimpMeshImporter.cpp
)
nexium_add_public_headers(Nexium_Subsystem_AssimpImporter
    NxSubsystems/Graphics/Import/AssimpMeshImporter.h
)
target_link_libraries(Nexium_Subsystem_AssimpImporter
    PUBLIC Nexium::Subsystem::Graphics
    PRIVATE assimp::assimp
)

nexium_configure_target(Nexium_Subsystem_AssimpImporter)
add_dependencies(Nexium_Subsystem_AssimpImporter CodeGenerator)
