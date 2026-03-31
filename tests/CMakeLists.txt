
# file(GLOB_RECURSE CPP_SOURCES "*.cpp")
# set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)
#
# file(GLOB_RECURSE HEADERS "*.h")
#
# add_executable(EngineTests ${CPP_SOURCES} ${HEADERS})
#
# target_link_libraries(EngineTests PUBLIC
#     gtest
#     Nexium::ECS
# )
#
# CoreAddCompileOptionsTo(EngineTests)

add_executable(ECSTests ECSTests.cpp)
target_link_libraries(ECSTests PUBLIC Nexium::ECS)
CoreAddCompileOptionsTo(ECSTests)