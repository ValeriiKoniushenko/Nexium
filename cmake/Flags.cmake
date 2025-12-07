include_guard()

function(CoreAddCompileOptionsTo Target)
    if (MSVC)
        target_compile_options(${Target} PRIVATE
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

        target_compile_definitions(${Target} PRIVATE -DNOMINMAX)
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
            "$<$<CONFIG:DEBUG>:-g>"
            "$<$<CONFIG:DEBUG>:-gdwarf-5>"
            "$<$<CONFIG:DEBUG>:-O0>"
            "$<$<CONFIG:DEBUG>:-gsplit-dwarf>"
            "$<$<CONFIG:DEBUG>:-fno-inline>"
            "$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer>"

            "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:Clang>>:-fdebug-info-for-profiling>"

            "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GCC>>:-fno-inline-small-functions>"
            "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GCC>>:-fno-inline-functions-called-once>"
            "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GCC>>:-fno-default-inline>"
            "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GCC>>:-fno-early-inlining>"
            "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GCC>>:-fno-inline-atomics>"

            # Release config
            "$<$<CONFIG:RELEASE>:-O3>"
            # "$<$<CONFIG:RELEASE>:-ffast-math>"
        )
    endif ()
endfunction()