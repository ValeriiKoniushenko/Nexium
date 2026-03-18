
file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE HEADERS "*.h")

add_library(__GameplaySystem ${CPP_SOURCES} ${HEADERS})


target_link_libraries(EngineTests PUBLIC
    gtest
    Nexium::Core
)

CoreAddCompileOptionsTo(EngineTests)