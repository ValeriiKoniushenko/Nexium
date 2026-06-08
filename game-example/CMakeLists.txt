set(TARGET_NAME TemplateGame)

file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE HEADERS "*.h")

add_executable(${TARGET_NAME} ${CPP_SOURCES} ${HEADERS})

target_link_libraries(${TARGET_NAME} PUBLIC
    Nexium::Core
)

if(MINGW)
    target_link_options(TemplateGame PRIVATE -Wl,--allow-multiple-definition)
endif()


CoreAddCompileOptionsTo(${TARGET_NAME})
