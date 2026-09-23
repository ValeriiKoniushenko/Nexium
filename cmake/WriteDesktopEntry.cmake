cmake_minimum_required(VERSION 3.30)

foreach(variable IN ITEMS
    NEXIUM_DESKTOP_FILE
    NEXIUM_DESKTOP_EXECUTABLE
    NEXIUM_DESKTOP_ICON
    NEXIUM_DESKTOP_NAME
    NEXIUM_DESKTOP_WM_CLASS
    NEXIUM_DESKTOP_WORKING_DIRECTORY
)
    if(NOT DEFINED ${variable} OR "${${variable}}" STREQUAL "")
        message(FATAL_ERROR "${variable} is required")
    endif()
endforeach()

set(executable "${NEXIUM_DESKTOP_EXECUTABLE}")
string(REPLACE "\\" "\\\\" executable "${executable}")
string(REPLACE "\"" "\\\"" executable "${executable}")
string(REPLACE "`" "\\`" executable "${executable}")
string(REPLACE "$" "\\$" executable "${executable}")

file(WRITE "${NEXIUM_DESKTOP_FILE}" "[Desktop Entry]\n"
    "Type=Application\n"
    "Version=1.0\n"
    "Name=${NEXIUM_DESKTOP_NAME}\n"
    "Exec=\"${executable}\"\n"
    "Icon=${NEXIUM_DESKTOP_ICON}\n"
    "Path=${NEXIUM_DESKTOP_WORKING_DIRECTORY}\n"
    "Terminal=false\n"
    "Categories=Development;\n"
    "StartupWMClass=${NEXIUM_DESKTOP_WM_CLASS}\n"
)
file(CHMOD "${NEXIUM_DESKTOP_FILE}"
    PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE
)
