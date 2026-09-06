set(SOURCES
    Configs.cpp
    BaseLog.cpp

    Interfaces/DataStream.cpp
)

set(HEADERS
    Configs.h
    BaseLog.h

    Interfaces/DataStream.h
)

add_library(Nexium_Foundation STATIC)
add_library(Nexium::Foundation ALIAS Nexium_Foundation)
target_sources(Nexium_Foundation PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(Nexium_Foundation PUBLIC
    Utils::Core
    Utils::Utils
    spdlog::spdlog
)

nexium_configure_target(Nexium_Foundation)
