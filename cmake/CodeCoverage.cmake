include_guard(GLOBAL)

if(NEXIUM_DISABLE_TESTS)
    message(FATAL_ERROR "NEXIUM_ENABLE_CODE_COVERAGE requires NEXIUM_DISABLE_TESTS=OFF")
endif()
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "Nexium code coverage currently requires GCC")
endif()
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(FATAL_ERROR "Nexium code coverage requires a Debug build")
endif()

find_program(GCOVR_EXECUTABLE gcovr REQUIRED)
find_program(GCOV_EXECUTABLE gcov REQUIRED)

set(NEXIUM_COVERAGE_REPORT_DIR "${PROJECT_BINARY_DIR}/coverage-report"
    CACHE PATH "Directory for Nexium coverage reports"
)

add_custom_target(Nexium_Coverage
    COMMAND "${CMAKE_COMMAND}"
        "-DGCOVR_EXECUTABLE=${GCOVR_EXECUTABLE}"
        "-DGCOV_EXECUTABLE=${GCOV_EXECUTABLE}"
        "-DPROJECT_ROOT=${PROJECT_SOURCE_DIR}"
        "-DBINARY_DIR=${PROJECT_BINARY_DIR}"
        "-DREPORT_DIR=${NEXIUM_COVERAGE_REPORT_DIR}"
        "-DTEST_EXECUTABLE=$<TARGET_FILE:Nexium_Tests>"
        -P "${CMAKE_CURRENT_LIST_DIR}/RunCodeCoverage.cmake"
    DEPENDS Nexium_Tests
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Running unit tests and generating the Nexium coverage report"
    VERBATIM
)
