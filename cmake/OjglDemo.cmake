set(_OJGL_CMAKE_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(ojgl_add_demo)
    set(oneValueArgs NAME SYNTH INCLUDE_DIR MUSIC SHADER_DIR SHADER_PREFIX)
    set(multiValueArgs SOURCES SHADERS COMMON_SHADERS)
    cmake_parse_arguments(DEMO "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT DEMO_NAME)
        message(FATAL_ERROR "ojgl_add_demo: NAME is required")
    endif()
    if(NOT DEMO_SOURCES)
        message(FATAL_ERROR "ojgl_add_demo(${DEMO_NAME}): SOURCES is required")
    endif()
    if(NOT DEMO_INCLUDE_DIR)
        set(DEMO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    if(NOT DEMO_SHADER_DIR)
        set(DEMO_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
    endif()
    if(NOT DEMO_SHADER_PREFIX)
        string(TOLOWER "${DEMO_NAME}" DEMO_SHADER_PREFIX)
    endif()

    target_sources(ojgl PRIVATE ${DEMO_SOURCES})
    target_include_directories(ojgl PRIVATE "${DEMO_INCLUDE_DIR}")

    if(DEMO_SYNTH STREQUAL "V2")
        if(NOT DEMO_MUSIC)
            message(FATAL_ERROR "ojgl_add_demo(${DEMO_NAME}): SYNTH V2 requires MUSIC (the song .inc); use SYNTH None for a silent demo")
        endif()
        target_sources(ojgl PRIVATE "${OJGL_ROOT}/src/music/V2MPlayer.cpp")
        target_link_libraries(ojgl PRIVATE
            "${OJGL_ROOT}/src/thirdparty/libv2.lib"
            $<$<CONFIG:Release,CrinklerRelease>:DSound>)
    elseif(DEMO_SYNTH STREQUAL "Clinkster")
        if(NOT DEMO_MUSIC)
            message(FATAL_ERROR "ojgl_add_demo(${DEMO_NAME}): SYNTH Clinkster requires MUSIC (the song .asm)")
        endif()
        find_program(OJGL_ASM_EXECUTABLE NAMES vsyasm yasm nasm nasmw
            DOC "Assembler for Clinkster's NASM-syntax .asm (yasm/vsyasm/nasm)")
        if(NOT OJGL_ASM_EXECUTABLE)
            message(FATAL_ERROR
                "SYNTH Clinkster needs an assembler (yasm, vsyasm or nasm) on PATH. "
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
        add_custom_target(${DEMO_NAME}_clinkster DEPENDS ${_clinkster_obj})
        add_dependencies(ojgl ${DEMO_NAME}_clinkster)
        set_source_files_properties(${_clinkster_obj}
            TARGET_DIRECTORY ojgl
            PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)

        target_sources(ojgl PRIVATE
            "${OJGL_ROOT}/src/music/ClinksterPlayer.cpp"
            $<$<CONFIG:Debug,OptimizedDebug>:${OJGL_ROOT}/src/music/AudioOutput.cpp>
            ${_clinkster_obj})
        target_link_libraries(ojgl PRIVATE
            winmm
            $<$<CONFIG:Debug,OptimizedDebug>:dsound>)
    elseif(DEMO_SYNTH STREQUAL "None")
        if(DEMO_MUSIC)
            message(FATAL_ERROR "ojgl_add_demo(${DEMO_NAME}): SYNTH None takes no MUSIC")
        endif()
        target_sources(ojgl PRIVATE "${OJGL_ROOT}/src/music/NoMusicPlayer.cpp")
    else()
        message(FATAL_ERROR "ojgl_add_demo(${DEMO_NAME}): SYNTH must be V2, Clinkster or None (got '${DEMO_SYNTH}')")
    endif()

    if(DEMO_SYNTH STREQUAL "V2" AND DEMO_MUSIC)
        if(NOT EXISTS "${DEMO_MUSIC}")
            message(FATAL_ERROR "ojgl_add_demo(${DEMO_NAME}): MUSIC file not found at ${DEMO_MUSIC}")
        endif()
        set(OJGL_SONG_DEFINITION "inline const unsigned char song[] = {\n#include \"${DEMO_MUSIC}\"\n};")
    else()
        set(OJGL_SONG_DEFINITION "inline const unsigned char* const song = nullptr;")
    endif()
    configure_file(
        "${_OJGL_CMAKE_MODULE_DIR}/EmbeddedSong.h.in"
        "${OJGL_GENERATED_DIR}/EmbeddedSong.h"
        @ONLY)

    set(OJGL_SHADER_EMBED_ENTRIES "")
    set(OJGL_SHADER_DISKPATH_ENTRIES "")
    set(_shader_entries "${OJGL_ROOT}/productions/common/shaders|common|quad.vs"
                        "${OJGL_ROOT}/productions/common/shaders|common|passThrough.fs")
    foreach(_fn IN LISTS DEMO_SHADERS)
        list(APPEND _shader_entries "${DEMO_SHADER_DIR}|${DEMO_SHADER_PREFIX}|${_fn}")
    endforeach()
    foreach(_fn IN LISTS DEMO_COMMON_SHADERS)
        list(APPEND _shader_entries "${OJGL_ROOT}/productions/common/shaders|common|${_fn}")
    endforeach()
    foreach(_entry IN LISTS _shader_entries)
        string(REPLACE "|" ";" _parts "${_entry}")
        list(GET _parts 0 _dir)
        list(GET _parts 1 _prefix)
        list(GET _parts 2 _fn)
        set(_f "${_dir}/${_fn}")
        if(NOT EXISTS "${_f}")
            message(FATAL_ERROR
                "ojgl_add_demo(${DEMO_NAME}): declared shader '${_fn}' not found at ${_f}")
        endif()
        set(_vp "${_prefix}/${_fn}")
        string(APPEND OJGL_SHADER_EMBED_ENTRIES "    {\n#include \"${_f}\"\n    , \"${_vp}\" },\n")
        string(APPEND OJGL_SHADER_DISKPATH_ENTRIES "    { \"${_vp}\", \"${_f}\" },\n")
    endforeach()
    configure_file(
        "${_OJGL_CMAKE_MODULE_DIR}/EmbeddedShaders.h.in"
        "${OJGL_GENERATED_DIR}/EmbeddedShaders.h"
        @ONLY)
endfunction()
