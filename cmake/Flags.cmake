include_guard()

function(CoreAddCompileOptionsTo Target)
    if (MSVC)
        target_compile_options(${Target} PRIVATE "/W4" "$<$<CONFIG:RELEASE>:/O2>")
    else ()
        target_compile_options(${Target} PRIVATE
            "-Wall"
            "-Wextra"
            # "-Werror"
            "-Wno-error=unused-variable"
            "-Wno-unused-variable"
            "-Wno-unused-parameter"
            "-Werror=return-type"
            "$<$<CONFIG:DEBUG>:-g3>"
            "$<$<CONFIG:DEBUG>:-O0>"
            "$<$<CONFIG:DEBUG>:-fno-inline>"
            "$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer>"
            "$<$<CONFIG:RELEASE>:-O3>"
        )
    endif ()
endfunction()