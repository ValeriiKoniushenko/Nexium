set(TARGET_NAME TemplateGame)

file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE HEADERS "*.h")

add_executable(${TARGET_NAME} ${CPP_SOURCES} ${HEADERS})

target_link_libraries(${TARGET_NAME} PUBLIC
    NexiumEngine
)

target_compile_options(${TARGET_NAME} PRIVATE
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-g3 -O0 -fno-inline>
)
