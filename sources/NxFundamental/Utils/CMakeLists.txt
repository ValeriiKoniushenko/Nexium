add_library(Nexium_Fundamental_Utils STATIC)
add_library(Nexium::Fundamental::Utils ALIAS Nexium_Fundamental_Utils)
target_sources(Nexium_Fundamental_Utils PRIVATE
    Stringifier.cpp
)
nexium_add_public_headers(Nexium_Fundamental_Utils
    NxFundamental/Utils/Stringifier.h
)

nexium_configure_target(Nexium_Fundamental_Utils)
add_dependencies(Nexium_Fundamental_Utils CodeGenerator)
