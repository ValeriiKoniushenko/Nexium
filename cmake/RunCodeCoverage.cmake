cmake_minimum_required(VERSION 3.30)

foreach(variable IN ITEMS
    GCOVR_EXECUTABLE
    GCOV_EXECUTABLE
    PROJECT_ROOT
    BINARY_DIR
    REPORT_DIR
    TEST_EXECUTABLE
)
    if(NOT DEFINED ${variable} OR "${${variable}}" STREQUAL "")
        message(FATAL_ERROR "${variable} is required")
    endif()
endforeach()

file(GLOB_RECURSE coverage_data "${BINARY_DIR}/*.gcda")
if(coverage_data)
    file(REMOVE ${coverage_data})
endif()
file(MAKE_DIRECTORY "${REPORT_DIR}")
file(REMOVE
    "${REPORT_DIR}/summary.json"
    "${REPORT_DIR}/summary.txt"
    "${REPORT_DIR}/index.html"
    "${REPORT_DIR}/code-coverage-report.zip"
)

execute_process(
    COMMAND "${TEST_EXECUTABLE}"
    WORKING_DIRECTORY "${PROJECT_ROOT}"
    RESULT_VARIABLE test_result
    COMMAND_ECHO STDOUT
)
if(NOT test_result EQUAL 0)
    message(FATAL_ERROR "Nexium_Tests failed with exit code ${test_result}")
endif()

execute_process(
    COMMAND "${GCOVR_EXECUTABLE}"
        --root "${PROJECT_ROOT}"
        --object-directory "${BINARY_DIR}"
        --gcov-executable "${GCOV_EXECUTABLE}"
        --filter "${PROJECT_ROOT}/sources/"
        --exclude ".*\\.generated\\.(h|cpp)$"
        --exclude-unreachable-branches
        --exclude-throw-branches
        --json-summary "${REPORT_DIR}/summary.json"
        --json-summary-pretty
        --html-details "${REPORT_DIR}/index.html"
        --html-single-page
        --html-self-contained
        --txt "${REPORT_DIR}/summary.txt"
        --print-summary
        -j 4
    WORKING_DIRECTORY "${PROJECT_ROOT}"
    RESULT_VARIABLE gcovr_result
    COMMAND_ECHO STDOUT
)
if(NOT gcovr_result EQUAL 0)
    message(FATAL_ERROR "gcovr failed with exit code ${gcovr_result}")
endif()

message(STATUS "Coverage report: ${REPORT_DIR}/index.html")
