cmake_minimum_required(VERSION 3.21)

# Windows/OpenGL demoscene framework. MSVC + Win32 (x86) only, e.g.:
#   cmake -S . -B build -G "Visual Studio 17 2022" -A Win32
project(OJGL CXX)

if(NOT MSVC)
    message(FATAL_ERROR "OJGL only builds with MSVC (cl.exe). Use the Visual Studio generator.")
endif()

# Debug/OptimizedDebug: standard CRT, console, _DEBUG paths. Release: tlibc CRT, GUI, min-size.
# CrinklerRelease: Release linked+compressed with Crinkler (Ninja generator only).
set(CMAKE_CONFIGURATION_TYPES "Debug;OptimizedDebug;Release;CrinklerRelease" CACHE STRING "" FORCE)

# Which production to build is selected per-build via the OJGL_DEMO cache variable (see
# ojgl/CMakeLists.txt); the demo declares its own music backend, so there is no global synth
# switch.

# Flags are set per-target via generator expressions; clear CMake's per-config defaults
# (/RTC1, /Ob0, /INCREMENTAL, ...) so they don't fight ours.
foreach(_cfg DEBUG OPTIMIZEDDEBUG RELEASE CRINKLERRELEASE)
    set(CMAKE_CXX_FLAGS_${_cfg} "" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_${_cfg} "" CACHE STRING "" FORCE)
    set(CMAKE_STATIC_LINKER_FLAGS_${_cfg} "" CACHE STRING "" FORCE)
endforeach()

# Dynamic debug CRT for debug configs, static release CRT otherwise.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug,OptimizedDebug>:DebugDLL>")

# Exceptions must be off for tlibc and Release ojgl; strip CMake's injected /EHsc and re-add
# it per-target where wanted.
string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Point this at a directory containing a local Crinkler.exe to skip the download below (offline
# builds, or to pin a different Crinkler build). Empty = auto-download v3.0a.
set(OJGL_CRINKLER_DIR "" CACHE PATH "Directory containing a local Crinkler.exe (skips the download)")

# Crinkler is a compressing link.exe replacement, only wired for the Ninja generator (the VS
# generator lets MSBuild pick the linker). CMAKE_LINKER can't be a generator expression, so it
# points at a batch dispatcher that routes to Crinkler.exe on /CRINKLER (CrinklerRelease only,
# see ojgl/CMakeLists.txt) and the real link.exe otherwise. (Crinkler's own link.exe-passthrough
# mode is unusable: 3.0a crashes with STATUS_STACK_BUFFER_OVERRUN past ~5 forwarded args.)
if(CMAKE_GENERATOR MATCHES "Ninja")
    # Crinkler isn't checked in; download the official v3.0a release into the (gitignored) build
    # tree on first configure and reuse it after.
    set(_crinkler_dir "${CMAKE_BINARY_DIR}/crinkler")
    set(_crinkler_sha256 "61de655f9ae3e3df98f452a2454e51829f49d41ef15bed27a65803dc5160c5c0")
    if(OJGL_CRINKLER_DIR)
        # User-supplied copy: trust it as-is (skip the download and the v3.0a hash pin).
        set(_crinkler_exe "${OJGL_CRINKLER_DIR}/Crinkler.exe")
        if(NOT EXISTS "${_crinkler_exe}")
            message(FATAL_ERROR "OJGL_CRINKLER_DIR is '${OJGL_CRINKLER_DIR}' but no Crinkler.exe was found there.")
        endif()
        message(STATUS "Using Crinkler from OJGL_CRINKLER_DIR: ${_crinkler_exe}")
    else()
        set(_crinkler_exe "${_crinkler_dir}/crinkler30a/Win32/Crinkler.exe")
        if(NOT EXISTS "${_crinkler_exe}")
            set(_crinkler_url "https://github.com/runestubbe/Crinkler/releases/download/v3.0a/crinkler30a.zip")
            set(_crinkler_zip "${_crinkler_dir}/crinkler30a.zip")
            message(STATUS "Crinkler not found; downloading ${_crinkler_url}")
            file(DOWNLOAD "${_crinkler_url}" "${_crinkler_zip}"
                SHOW_PROGRESS
                STATUS _crinkler_dl_status)
            list(GET _crinkler_dl_status 0 _crinkler_dl_code)
            if(NOT _crinkler_dl_code EQUAL 0)
                list(GET _crinkler_dl_status 1 _crinkler_dl_msg)
                file(REMOVE "${_crinkler_zip}")
                message(FATAL_ERROR "Failed to download Crinkler from ${_crinkler_url}: ${_crinkler_dl_msg}\n"
                    "For an offline build, extract it manually and set -DOJGL_CRINKLER_DIR=<dir containing Crinkler.exe>.")
            endif()
            file(ARCHIVE_EXTRACT INPUT "${_crinkler_zip}" DESTINATION "${_crinkler_dir}")
            if(NOT EXISTS "${_crinkler_exe}")
                message(FATAL_ERROR "Crinkler archive did not contain the expected ${_crinkler_exe}")
            endif()
        endif()
        file(SHA256 "${_crinkler_exe}" _crinkler_actual_sha256)
        if(NOT _crinkler_actual_sha256 STREQUAL _crinkler_sha256)
            message(FATAL_ERROR
                "Crinkler.exe SHA256 mismatch at ${_crinkler_exe}\n"
                "  expected ${_crinkler_sha256}\n"
                "  actual   ${_crinkler_actual_sha256}\n"
                "Delete ${_crinkler_dir} and reconfigure to re-download.")
        endif()
    endif()
    configure_file("${_crinkler_exe}" "${_crinkler_dir}/crinkler.exe" COPYONLY)
    # Resolve link.exe next to cl.exe, not via PATH (Git for Windows ships its own link.exe).
    get_filename_component(_msvc_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
    find_program(OJGL_REAL_LINKER NAMES link.exe PATHS "${_msvc_bin_dir}" NO_DEFAULT_PATH REQUIRED)
    file(WRITE "${CMAKE_BINARY_DIR}/crinkler/link.bat" "@echo off\r
echo %*| findstr /C:\"/CRINKLER\" >nul\r
if %errorlevel%==0 (\r
    \"${CMAKE_BINARY_DIR}/crinkler/crinkler.exe\" %*\r
) else (\r
    \"${OJGL_REAL_LINKER}\" %*\r
)\r
")
    set(CMAKE_LINKER "${CMAKE_BINARY_DIR}/crinkler/link.bat")
    # Crinkler only understands "/LIBPATH:", not CMake's default "-LIBPATH:".
    set(CMAKE_LIBRARY_PATH_FLAG "/LIBPATH:")
endif()

add_subdirectory(src/thirdparty/tlibc)

# =====================================================================================
# ojgl - the framework + demos executable.
# =====================================================================================

# Which production to build. Exactly one demo is compiled per build; the demo declares its
# own music backend (see ojgl_add_demo), so this selection drives all the synth wiring.
set(OJGL_DEMO "Edison2026" CACHE STRING "Which production/demo to build")

set(OJGL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(OJGL_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/OjglDemo.cmake")

# Framework/app sources. The selected demo's own sources and its music backend are added by
# ojgl_add_demo below.
set(OJGL_SOURCES
    examples/FreeCameraController.cpp
    examples/Main.cpp
    examples/TextRenderer.cpp
    src/thirdparty/StbImage.cpp
    src/music/Music.cpp
    src/music/SyncChannel.cpp
    src/render/Buffer.cpp
    src/render/GLState.cpp
    src/render/Mesh.cpp
    src/render/Popup.cpp
    src/render/Scene.cpp
    src/render/Texture.cpp
    src/render/Uniform.cpp
    src/render/Window.cpp
    src/utility/Matrix.cpp
    src/utility/Duration.cpp
    src/utility/OJstd.cpp
    src/utility/ShaderReader.cpp
    src/utility/Timepoint.cpp
    src/winapi/gl_loader.cpp
)

# Non-compiled files (headers, shaders, songs), included only so they show in the IDE.
file(GLOB_RECURSE OJGL_DISPLAY_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/shaders/*.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/shaders/*.vs"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/render/shaders/*.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/render/shaders/*.vs"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/songs/*.inc"
)
set_source_files_properties(${OJGL_DISPLAY_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(ojgl ${OJGL_SOURCES} ${OJGL_DISPLAY_FILES})

# Group files in the IDE by their on-disk folder structure.
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${OJGL_SOURCES} ${OJGL_DISPLAY_FILES})

# Only the Release builds are Windows-subsystem (GUI) apps; the debug configs stay Console.
set_target_properties(ojgl PROPERTIES
    WIN32_EXECUTABLE $<CONFIG:Release,CrinklerRelease>
    # Shaders resolve via absolute paths (debug) or are embedded (release), so the working
    # directory no longer matters; point the debugger at the repo root for convenience.
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(ojgl PRIVATE src examples "${OJGL_GENERATED_DIR}")

target_compile_definitions(ojgl PRIVATE
    WIN32 _CONSOLE _CRT_SECURE_NO_WARNINGS
    VC_EXTRALEAN WIN32_LEAN_AND_MEAN WIN32_EXTRA_LEAN NOMINMAX
    $<$<CONFIG:Debug,OptimizedDebug>:_DEBUG>
    $<$<CONFIG:Release,CrinklerRelease>:NDEBUG>
)

target_compile_options(ojgl PRIVATE
    /W4 /WX
    /GR-                 # no RTTI
    /GS-                 # no buffer security checks
    /U_MBCS              # old project used CharacterSet=NotSet, not MBCS
    /Zc:sizedDealloc-
    $<$<CONFIG:Debug>:/Od;/EHsc;/ZI;/w44062>
    $<$<CONFIG:OptimizedDebug>:/O1;/Os;/Oy;/EHsc;/ZI>
    $<$<CONFIG:Release,CrinklerRelease>:/O1;/Os;/Oy;/arch:IA32;/Zi>
)

# C++17 (matches /std:c++17 in the old project).
target_compile_features(ojgl PRIVATE cxx_std_17)

# Where to find the prebuilt libv2.lib.
target_link_directories(ojgl PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty")

target_link_libraries(ojgl PRIVATE
    # linked in every config
    opengl32 kernel32 user32 gdi32 winspool comdlg32 advapi32 shell32
    ole32 oleaut32 uuid odbc32 odbccp32
    # Release/CrinklerRelease swap in tlibc and the DirectX libs (DSound is added per-backend).
    $<$<CONFIG:Release,CrinklerRelease>:tlibc>
    $<$<CONFIG:Release,CrinklerRelease>:DDraw>
    $<$<CONFIG:Release,CrinklerRelease>:Dinput8>
)

# Register the selected production (adds its sources + music backend, generates the
# SelectedDemo.h factory). Until the demos move into productions/<name>/ dirs, they are
# registered here. Only Edison2026 (the active demo) is wired so far; others are added as
# they migrate.
if(OJGL_DEMO STREQUAL "Edison2026")
    ojgl_add_demo(
        NAME Edison2026
        SYNTH CLINKSTER
        SOURCES examples/demos/Edison2026.cpp
        INCLUDE_DIR examples/demos
        MUSIC "${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/clinkster/music2.asm"
        SHADER_DIRS
            "${CMAKE_CURRENT_SOURCE_DIR}/examples/shaders/common"
            "${CMAKE_CURRENT_SOURCE_DIR}/examples/shaders/edison2026")
else()
    message(FATAL_ERROR "Unknown OJGL_DEMO '${OJGL_DEMO}'")
endif()

target_link_options(ojgl PRIVATE
    # libv2.lib and clinkster.obj lack SAFESEH metadata; Crinkler doesn't understand /SAFESEH, so
    # apply it to every config except CrinklerRelease.
    $<$<NOT:$<CONFIG:CrinklerRelease>>:/SAFESEH:NO>
    $<$<CONFIG:Debug>:/DEBUG:FASTLINK>
    $<$<CONFIG:OptimizedDebug>:/DEBUG:FASTLINK;/OPT:REF;/OPT:ICF>
    # Release: tlibc is the CRT (/NODEFAULTLIB), enter through mainCRTStartup, LTCG (tlibc is /GL).
    $<$<CONFIG:Release>:/DEBUG;/NODEFAULTLIB;/ENTRY:mainCRTStartup;/OPT:REF;/OPT:ICF;/MAP;/LTCG>
    # CrinklerRelease: Crinkler links + compresses. /MANIFEST:NO so CMake's wrapper skips mt.exe
    # (Crinkler has no manifest support). /REUSE was removed: it crashed on Clinkster's large BSS.
    $<$<CONFIG:CrinklerRelease>:/NODEFAULTLIB;/ENTRY:mainCRTStartup;/MANIFEST:NO;/CRINKLER;/RANGE:opengl32;/COMPMODE:FAST;/TRANSFORM:CALLS;/REPORT:ojgl_crinkler_report.html;/PRINT:IMPORTS;/HASHTRIES:1;/HASHSIZE:1000>
)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ojgl)
