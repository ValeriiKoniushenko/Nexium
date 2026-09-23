set(TARGET_NAME TemplateGame)

file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE HEADERS "*.h")

add_executable(${TARGET_NAME} ${CPP_SOURCES} ${HEADERS})
target_link_libraries(${TARGET_NAME} PRIVATE
    "$<LINK_LIBRARY:WHOLE_ARCHIVE,Nexium::Editor>"
)

nexium_configure_target(${TARGET_NAME})
nexium_configure_application(${TARGET_NAME}
    APPLICATION_ID nexium
    DISPLAY_NAME "Nexium Engine"
)
