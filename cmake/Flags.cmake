include_guard()

function(CoreAddCompileOptionsTo Target)
    if (MSVC)
        target_compile_options(${Target} PRIVATE
            "/W4"
            "/wd4005"            # disable: macro redefinition (closest to -Wno-comment, MSVC warns differently)
            "/wd4101"            # disable: unused variable (-Wno-unused-variable)
            "/wd4100"            # disable: unused parameter (-Wno-unused-parameter)
            "/we4715"            # treat "not all control paths return a value" as error (-Werror=return-type)

            # Debug config
            "$<$<CONFIG:DEBUG>:/Zi>"
            "$<$<CONFIG:DEBUG>:/Od>"
            "$<$<CONFIG:DEBUG>:/Ob0>"
            "$<$<CONFIG:DEBUG>:/Oy->"

            # Release config
            "$<$<CONFIG:RELEASE>:/O2>"
            # "$<$<CONFIG:RELEASE>:/fp:fast>"
        )

        target_compile_definitions(${Target} -DNOMINMAX)
    else ()
        target_compile_options(${Target} PRIVATE
            "-Wall"
            "-Wextra"
            "-Wno-comment"
            # "-Werror"
            "-Wno-error=unused-variable"
            "-Wno-unused-variable"
            "-Wno-unused-parameter"
            "-Werror=return-type"

            # Debug config
            "$<$<CONFIG:DEBUG>:-g3>"
            "$<$<CONFIG:DEBUG>:-O0>"
            "$<$<CONFIG:DEBUG>:-fno-inline>"
            "$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer>"

            # Release config
            "$<$<CONFIG:RELEASE>:-O3>"
            # "$<$<CONFIG:RELEASE>:-ffast-math>"
        )
    endif ()
endfunction()