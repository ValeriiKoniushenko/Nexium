
set(TARGET_NAME __SW_BetterEnums)

set(SOURCES enum.h)
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)

add_library(${TARGET_NAME} INTERFACE ${SOURCES})
add_library(SW::BetterEnums ALIAS ${TARGET_NAME})

target_include_directories(${TARGET_NAME} INTERFACE ./)