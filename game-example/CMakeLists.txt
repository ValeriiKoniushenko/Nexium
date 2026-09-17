set(TARGET_NAME TemplateGame)

file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE HEADERS "*.h")

if (WIN32)
    set(APP_ICON_RESOURCE ${CMAKE_SOURCE_DIR}/sources/Resources/app.rc)
endif ()

add_executable(${TARGET_NAME} ${CPP_SOURCES} ${HEADERS} ${APP_ICON_RESOURCE})
target_link_libraries(${TARGET_NAME} PRIVATE
    "$<LINK_LIBRARY:WHOLE_ARCHIVE,Nexium::Editor>"
)


nexium_configure_target(${TARGET_NAME})
