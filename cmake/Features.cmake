include_guard()

function(InstallCXXStandardVersion)
    if(${REQUIRED_CXX_STANDARD} IN_LIST CMAKE_CXX_COMPILE_FEATURES)
        set(CXX_STANDARD ${REQUIRED_CXX_STANDARD} PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Impossible to set '${REQUIRED_CXX_STANDARD}' (C++ Standard). Can not find this feature in your: 'CMAKE_CXX_COMPILE_FEATURES'")
    endif()
endfunction()