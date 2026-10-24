add_library(Nexium_Fundamental_ResourceManagement STATIC)
add_library(Nexium::Fundamental::ResourceManagement
    ALIAS Nexium_Fundamental_ResourceManagement
)
target_sources(Nexium_Fundamental_ResourceManagement PRIVATE
    AtomicFile.cpp
    DataStream.cpp
    JsonAdapter.cpp
)
nexium_add_public_headers(Nexium_Fundamental_ResourceManagement
    NxFundamental/ResourceManagement/AtomicFile.h
    NxFundamental/ResourceManagement/DataStream.h
    NxFundamental/ResourceManagement/DataStream.generated.h
    NxFundamental/ResourceManagement/JsonAdapter.h
)
target_link_libraries(Nexium_Fundamental_ResourceManagement
    PUBLIC
    Nexium::Foundation::Base
    Nexium::Foundation::Interfaces
    Nexium::Fundamental::Utils
    JustReflectMe::Adapter
    Utils::Core
    PRIVATE
    spdlog::spdlog
    Nexium_Fundamental_PrivateModule
)

nexium_configure_target(Nexium_Fundamental_ResourceManagement)
add_dependencies(Nexium_Fundamental_ResourceManagement CodeGenerator)
