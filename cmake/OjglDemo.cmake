# ojgl_add_demo — register the single selected production with the `ojgl` target.
#
# Exactly one demo is compiled per build (OJGL_DEMO). The demo declares its own music
# backend, so selecting the demo drives all the synth wiring; there is no separate global
# synth switch. This function wires the demo sources + backend. The demo itself is
# instantiated through ojgl::createSelectedDemo() (declared in demo/Demo.h), which each
# production defines in its .cpp — so no per-production header is needed and no factory is
# generated.
#
# ojgl_add_demo(
#   NAME     <Edison2026>            # logical name; also the demo's own dir/shader defaults
#   SYNTH    <V2|CLINKSTER|None>     # music backend (exclusive per binary). None links no synth
#                                    #   library and runs silently (e.g. Template).
#   SOURCES  <a.cpp> [b.cpp ...]     # demo sources (relative to the caller's dir)
#   [INCLUDE_DIR <dir>]              # dir put on the include path so SOURCES' own includes
#                                    #   (e.g. music/*.inc) resolve,
#                                    #   default the caller's CMAKE_CURRENT_SOURCE_DIR
#   [MUSIC   <path>]                 # the song resource; required by V2 and CLINKSTER, rejected by
#                                    #   None:
#                                    #   V2:        the *_song.inc, embedded as resources::song
#                                    #              in the generated EmbeddedSong.h
#                                    #   CLINKSTER: the baked-song .asm (its dir becomes -I)
#   [SHADERS <a.fs> [b.fs ...]]      # the demo's own shader files to embed (filenames relative
#                                    #   to SHADER_DIR). Only listed shaders are embedded — there
#                                    #   is no globbing, so unused files are never shipped.
#   [COMMON_SHADERS <a.fs> ...]      # shared shaders (filenames relative to
#                                    #   productions/common/shaders) this demo needs, embedded
#                                    #   under the "common/" prefix. List only what is used.
#                                    #   common/quad.vs and common/passThrough.fs are always
#                                    #   embedded automatically; do not list them here.
#   [SHADER_DIR <dir>]               # folder the SHADERS filenames resolve against,
#                                    #   default "<caller>/shaders"
#   [SHADER_PREFIX <prefix>]         # virtual-path prefix for the demo's shaders (e.g. a shader
#                                    #   "cube.fs" -> "<prefix>/cube.fs"), default lowercased NAME
# )
#
# Shaders are embedded from two explicit lists (no globbing): SHADERS (the demo's own, under
# SHADER_PREFIX) and COMMON_SHADERS (from productions/common/shaders, under "common/"). Each
# listed file must exist; a typo is a configure-time error. To add a shader, drop it in the
# folder and add its name to the relevant list.
#
# Expects the caller to have set OJGL_ROOT (dir containing src/) and OJGL_GENERATED_DIR.

# Directory of this module (captured at include time; CMAKE_CURRENT_LIST_DIR is not reliable
# inside a function called from another listfile).
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

    # --- music synth backend (exclusive; the demo declares which one) ---
    # Which backend is compiled is decided purely by which player .cpp we add below; each such
    # .cpp defines ojgl::createSelectedPlayer(). No preprocessor synth switch is needed.
    if(DEMO_SYNTH STREQUAL "V2")
        if(NOT DEMO_MUSIC)
            message(FATAL_ERROR "ojgl_add_demo(${DEMO_NAME}): SYNTH V2 requires MUSIC (the song .inc); use SYNTH None for a silent demo")
        endif()
        # libv2.lib provides the synth and DirectSound helpers, so it links in every config;
        # DSound is only needed where libv2 doesn't already pull it in.
        target_sources(ojgl PRIVATE "${OJGL_ROOT}/src/music/V2MPlayer.cpp")
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
        # This function runs in the demo's subdirectory, but the ojgl target lives in the root.
        # A bare OUTPUT custom command only gets a build rule via a target in its own directory,
        # so wrap it in a custom target ojgl depends on, and mark the object EXTERNAL_OBJECT in
        # ojgl's directory scope (TARGET_DIRECTORY) so ojgl links it instead of compiling it.
        add_custom_target(${DEMO_NAME}_clinkster DEPENDS ${_clinkster_obj})
        add_dependencies(ojgl ${DEMO_NAME}_clinkster)
        set_source_files_properties(${_clinkster_obj}
            TARGET_DIRECTORY ojgl
            PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)

        target_sources(ojgl PRIVATE
            "${OJGL_ROOT}/src/music/ClinksterPlayer.cpp"
            "${OJGL_ROOT}/src/music/AudioOutput.cpp"
            ${_clinkster_obj})
        # AudioOutput needs DirectSound in every config (no libv2 to provide it); winmm resolves
        # the waveOut* imports from Clinkster's unused sections that Debug doesn't strip.
        target_link_libraries(ojgl PRIVATE dsound winmm)
    elseif(DEMO_SYNTH STREQUAL "None")
        # No music backend: link no synth library. NoMusicPlayer.cpp supplies a
        # createSelectedPlayer() that returns nullptr, so the demo runs silently on the system clock.
        if(DEMO_MUSIC)
            message(FATAL_ERROR "ojgl_add_demo(${DEMO_NAME}): SYNTH None takes no MUSIC")
        endif()
        target_sources(ojgl PRIVATE "${OJGL_ROOT}/src/music/NoMusicPlayer.cpp")
    else()
        message(FATAL_ERROR "ojgl_add_demo(${DEMO_NAME}): SYNTH must be V2, CLINKSTER or None (got '${DEMO_SYNTH}')")
    endif()

    # --- generate the embedded song (resources::song, read by createSelectedPlayer) ---
    # V2 embeds the demo's *_song.inc bytes as an array. Clinkster (song baked into the linked
    # object) and any demo without MUSIC get a null pointer, which createSelectedPlayer() treats
    # as "this demo has no music".
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

    # --- generate the embedded-shader list (replaces the hand-maintained EmbeddedResources.h) ---
    # Embed exactly the shaders the demo declares: its own (SHADERS, resolved against
    # DEMO_SHADER_DIR, under prefix SHADER_PREFIX) plus the shared ones it uses (COMMON_SHADERS,
    # from productions/common/shaders, under "common/"). No globbing — a demo ships only the
    # shaders it lists. Release embeds the source via #include of the R""()"" literal; debug also
    # records the disk path for hot reload. Order is irrelevant (ShaderReader resolves #includes
    # lazily).
    set(OJGL_SHADER_EMBED_ENTRIES "")
    set(OJGL_SHADER_DISKPATH_ENTRIES "")
    # Each entry: "<dir>|<prefix>|<filename>".
    # quad.vs and passThrough.fs are used by virtually every demo, so they're always embedded
    # here rather than requiring every production to list them in COMMON_SHADERS.
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
