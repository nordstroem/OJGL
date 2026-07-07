# ojgl_add_demo — register the single selected production with the `ojgl` target.
#
# Exactly one demo is compiled per build (OJGL_DEMO). The demo declares its own music
# backend, so selecting the demo drives all the synth wiring; there is no separate global
# synth switch. This function wires the demo sources + backend and generates the
# SelectedDemo.h factory that Main.cpp uses to instantiate the demo.
#
# ojgl_add_demo(
#   NAME     <Edison2026>            # logical + C++ class name (namespace ojgl)
#   SYNTH    <V2|CLINKSTER>          # music backend (exclusive per binary)
#   SOURCES  <a.cpp> [b.cpp ...]     # demo sources (relative to the caller's dir)
#   [HEADER  <Edison2026.h>]         # demo header include, default "<NAME>.h"
#   [INCLUDE_DIR <dir>]              # dir put on the include path so HEADER/SOURCES resolve,
#                                    #   default the caller's CMAKE_CURRENT_SOURCE_DIR
#   [MUSIC   <path>]                 # V2:       the *_song.inc (embedded by the demo .cpp)
#                                    #   CLINKSTER: the baked-song .asm (its dir becomes -I)
# )
#
# Expects the caller to have set OJGL_ROOT (dir containing src/) and OJGL_GENERATED_DIR.

# Directory of this module (captured at include time; CMAKE_CURRENT_LIST_DIR is not reliable
# inside a function called from another listfile).
set(_OJGL_CMAKE_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(ojgl_add_demo)
    set(oneValueArgs NAME SYNTH HEADER INCLUDE_DIR MUSIC)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(DEMO "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT DEMO_NAME)
        message(FATAL_ERROR "ojgl_add_demo: NAME is required")
    endif()
    if(NOT DEMO_SOURCES)
        message(FATAL_ERROR "ojgl_add_demo(${DEMO_NAME}): SOURCES is required")
    endif()
    if(NOT DEMO_HEADER)
        set(DEMO_HEADER "${DEMO_NAME}.h")
    endif()
    if(NOT DEMO_INCLUDE_DIR)
        set(DEMO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    target_sources(ojgl PRIVATE ${DEMO_SOURCES})
    target_include_directories(ojgl PRIVATE "${DEMO_INCLUDE_DIR}")

    # --- music synth backend (exclusive; the demo declares which one) ---
    if(DEMO_SYNTH STREQUAL "V2")
        # libv2.lib provides the synth and DirectSound helpers, so it links in every config;
        # DSound is only needed where libv2 doesn't already pull it in.
        target_sources(ojgl PRIVATE "${OJGL_ROOT}/src/music/V2MPlayer.cpp")
        target_compile_definitions(ojgl PRIVATE OJGL_SYNTH_V2)
        target_link_libraries(ojgl PRIVATE
            "${OJGL_ROOT}/src/thirdparty/libv2.lib"
            $<$<CONFIG:Release,CrinklerRelease>:DSound>)
    elseif(DEMO_SYNTH STREQUAL "CLINKSTER")
        if(NOT DEMO_MUSIC)
            message(FATAL_ERROR "ojgl_add_demo(${DEMO_NAME}): SYNTH CLINKSTER requires MUSIC (the song .asm)")
        endif()
        # clinkster.asm %includes the baked song; assemble both into one object. The song's
        # directory is the assembler include path, so each demo supplies its own song.
        find_program(OJGL_ASM_EXECUTABLE NAMES vsyasm yasm nasm nasmw
            DOC "Assembler for Clinkster's NASM-syntax .asm (yasm/vsyasm/nasm)")
        if(NOT OJGL_ASM_EXECUTABLE)
            message(FATAL_ERROR
                "SYNTH CLINKSTER needs an assembler (yasm, vsyasm or nasm) on PATH. "
                "Install one or pass -DOJGL_ASM_EXECUTABLE=<path-to-yasm.exe>.")
        endif()

        set(_clinkster_dir "${OJGL_ROOT}/src/thirdparty/clinkster")
        get_filename_component(_song_dir "${DEMO_MUSIC}" DIRECTORY)
        set(_clinkster_obj "${CMAKE_CURRENT_BINARY_DIR}/clinkster.obj")
        add_custom_command(
            OUTPUT ${_clinkster_obj}
            COMMAND ${OJGL_ASM_EXECUTABLE} -f win32
                    -I ${_song_dir}/
                    -o ${_clinkster_obj}
                    ${_clinkster_dir}/clinkster.asm
            DEPENDS ${_clinkster_dir}/clinkster.asm ${DEMO_MUSIC}
            COMMENT "Assembling Clinkster engine + baked song -> clinkster.obj")
        set_source_files_properties(${_clinkster_obj} PROPERTIES GENERATED TRUE EXTERNAL_OBJECT TRUE)

        target_sources(ojgl PRIVATE
            "${OJGL_ROOT}/src/music/ClinksterPlayer.cpp"
            "${OJGL_ROOT}/src/music/AudioOutput.cpp"
            ${_clinkster_obj})
        target_compile_definitions(ojgl PRIVATE OJGL_SYNTH_CLINKSTER)
        # AudioOutput needs DirectSound in every config (no libv2 to provide it); winmm resolves
        # the waveOut* imports from Clinkster's unused sections that Debug doesn't strip.
        target_link_libraries(ojgl PRIVATE dsound winmm)
    else()
        message(FATAL_ERROR "ojgl_add_demo(${DEMO_NAME}): SYNTH must be V2 or CLINKSTER (got '${DEMO_SYNTH}')")
    endif()

    # --- generate the demo factory consumed by Main.cpp (replaces the old enum/switch) ---
    set(OJGL_DEMO_HEADER "${DEMO_HEADER}")
    set(OJGL_DEMO_CLASS "${DEMO_NAME}")
    configure_file(
        "${_OJGL_CMAKE_MODULE_DIR}/SelectedDemo.h.in"
        "${OJGL_GENERATED_DIR}/SelectedDemo.h"
        @ONLY)
endfunction()
