include_guard()

function(CoreAddCompileOptionsTo Target)
    if (MSVC)
        target_compile_options(${Target} PRIVATE
            "/wd4005"            # disable: macro redefinition (closest to -Wno-comment, MSVC warns differently)
            "/wd4101"            # disable: unused variable (-Wno-unused-variable)
            "/wd4100"            # disable: unused parameter (-Wno-unused-parameter)
            "/we4715"            # treat "not all control paths return a value" as error (-Werror=return-type)

            # Debug config
            "$<$<CONFIG:Debug>:/Od>"
            "$<$<CONFIG:Debug>:/Ob0>"
            "$<$<CONFIG:Debug>:/Oy->"

            # Release config
            "$<$<CONFIG:Release>:/O2>"
            # "$<$<CONFIG:Release>:/fp:fast>"
        )

        target_compile_definitions(${Target} PRIVATE -DNOMINMAX)
    elseif (MINGW)
        target_link_options(${Target} PRIVATE -Wl,--allow-multiple-definition)
    else ()
        target_compile_options(${Target} PRIVATE
            "-Wall"
            "-Wextra"
            "-Wno-comment"
            # "-Werror"
            # "-Wno-error=unused-variable"
            # "-Wno-error=unused-but-set-variable"
            "-Wno-unused-variable"
            "-Wno-unused-parameter"
            "-Wno-unused-but-set-variable"
            "-Werror=return-type"

            # Debug config
            "$<$<CONFIG:Debug>:-g>"
            "$<$<CONFIG:Debug>:-gdwarf-5>"
            "$<$<CONFIG:Debug>:-O0>"
            "$<$<CONFIG:Debug>:-gsplit-dwarf>"
            "$<$<CONFIG:Debug>:-fno-omit-frame-pointer>"

            "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:Clang>>:-fdebug-info-for-profiling>"

            "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GCC>>:-fno-inline-small-functions>"
            "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GCC>>:-fno-inline-functions-called-once>"
            "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GCC>>:-fno-default-inline>"
            "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GCC>>:-fno-early-inlining>"
            "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GCC>>:-fno-inline-atomics>"

            # Release config
            "$<$<CONFIG:Release>:-O3>"
            # "$<$<CONFIG:Release>:-ffast-math>"
        )
    endif ()
endfunction()