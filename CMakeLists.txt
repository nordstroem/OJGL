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

# Music synth backend, exclusive at configure time. V2: runtime-streamed .v2m player.
# CLINKSTER: song baked into build-time assembly, rendered up front.
set(OJGL_SYNTH "V2" CACHE STRING "Music synth backend: V2 or CLINKSTER")
set_property(CACHE OJGL_SYNTH PROPERTY STRINGS "V2" "CLINKSTER")

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

add_subdirectory(tlibc)
add_subdirectory(ojgl)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ojgl)
