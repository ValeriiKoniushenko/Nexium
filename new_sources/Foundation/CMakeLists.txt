set(SOURCES
    Configs.cpp
    BaseLog.cpp
)

set(HEADERS
    Configs.h
    BaseLog.h
)

add_library(Nexium_Foundation STATIC)
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)
target_sources(Nexium_Foundation PRIVATE ${SOURCES} PUBLIC ${HEADERS})

target_include_directories(Nexium_Foundation PUBLIC ../)
target_link_libraries(Nexium_Foundation PUBLIC Utils::Core Utils::Utils)

CoreAddCompileOptionsTo(Nexium_Foundation)
