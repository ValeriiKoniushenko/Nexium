add_library(Nexium_Runtime_GameUtils STATIC)
add_library(Nexium::Runtime::GameUtils ALIAS Nexium_Runtime_GameUtils)
target_sources(Nexium_Runtime_GameUtils PRIVATE
    FPSCounter.cpp
)
nexium_add_public_headers(Nexium_Runtime_GameUtils
    NxRuntime/GameUtils/FPSCounter.h
)
target_link_libraries(Nexium_Runtime_GameUtils
    PRIVATE
    ${MODULE_PRIVATE_TARGET}
)

nexium_configure_target(Nexium_Runtime_GameUtils)
