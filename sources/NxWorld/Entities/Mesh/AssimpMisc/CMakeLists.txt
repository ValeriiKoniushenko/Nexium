add_library(Nexium_World_AssimpMisc STATIC)
add_library(Nexium::World::AssimpMisc ALIAS Nexium_World_AssimpMisc)
target_sources(Nexium_World_AssimpMisc PRIVATE
    AssimpHelper.cpp
)
nexium_add_public_headers(Nexium_World_AssimpMisc
    NxWorld/Entities/Mesh/AssimpMisc/AssimpHelper.h
)
target_link_libraries(Nexium_World_AssimpMisc
    PUBLIC
    Utils::Core
    assimp::assimp
    PRIVATE
    Nexium::Foundation::Base
)

nexium_configure_target(Nexium_World_AssimpMisc)
add_dependencies(Nexium_World_AssimpMisc CodeGenerator)
