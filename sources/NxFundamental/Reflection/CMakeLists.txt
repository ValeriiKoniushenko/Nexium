add_library(Nexium_Fundamental_Reflection STATIC)
add_library(Nexium::Fundamental::Reflection ALIAS Nexium_Fundamental_Reflection)
target_sources(Nexium_Fundamental_Reflection PRIVATE
    TypeDescriptor.cpp
)
nexium_add_public_headers(Nexium_Fundamental_Reflection
    NxFundamental/Reflection/TypeDescriptor.h
)
target_link_libraries(Nexium_Fundamental_Reflection
    PUBLIC
    JustReflectMe::Adapter
    Utils::Core
)
nexium_configure_target(Nexium_Fundamental_Reflection)
add_dependencies(Nexium_Fundamental_Reflection CodeGenerator)
