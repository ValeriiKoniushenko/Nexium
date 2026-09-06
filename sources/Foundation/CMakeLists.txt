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
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)
target_sources(Nexium_Foundation PRIVATE ${SOURCES} PUBLIC ${HEADERS})

target_include_directories(Nexium_Foundation PUBLIC ../)
target_link_libraries(Nexium_Foundation PUBLIC
    Utils::Core
    Utils::Utils
    spdlog::spdlog
)

CoreAddCompileOptionsTo(Nexium_Foundation)
