add_library(Nexium_Subsystem_Input STATIC)
add_library(Nexium::Subsystem::Input ALIAS Nexium_Subsystem_Input)
target_sources(Nexium_Subsystem_Input PRIVATE
    InputAction.cpp
    InputManager.cpp
    InputTypes.cpp
)
nexium_add_public_headers(Nexium_Subsystem_Input
    NxSubsystems/Input/InputAction.h
    NxSubsystems/Input/InputManager.h
    NxSubsystems/Input/InputTypes.h
    NxSubsystems/Input/InputTypes.generated.h
    NxSubsystems/PrivateModuleInfo.h
)
target_link_libraries(Nexium_Subsystem_Input
    PUBLIC
    Nexium::Foundation::Base
    Nexium::Foundation::Interfaces
    Nexium::Platform::Input
    Utils::Core
    glm
    PRIVATE
    ${MODULE_PRIVATE_TARGET}
)

nexium_configure_target(Nexium_Subsystem_Input)
add_dependencies(Nexium_Subsystem_Input CodeGenerator)
