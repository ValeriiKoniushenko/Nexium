include_guard(GLOBAL)

function(nexium_configure_target target)
    target_compile_features(${target} PUBLIC cxx_std_26)
    target_include_directories(${target} PUBLIC "${PROJECT_SOURCE_DIR}/sources")
    target_compile_definitions(${target} PRIVATE
        NEXIUM_PROJECT_DIR="${PROJECT_SOURCE_DIR}"
        "$<$<CONFIG:Debug>:DEBUG;GRAPHICS_DEBUG>"
    )
    CoreAddCompileOptionsTo(${target})
endfunction()
