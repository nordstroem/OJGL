cmake_minimum_required(VERSION 3.21)

# OJGL is a Windows / OpenGL demoscene framework. It only builds with MSVC targeting
# Win32 (x86). Configure with the Visual Studio generator, e.g.:
#   cmake -S . -B build -G "Visual Studio 17 2022" -A Win32
project(OJGL CXX)

if(NOT MSVC)
    message(FATAL_ERROR "OJGL only builds with MSVC (cl.exe). Use the Visual Studio generator.")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    # message(FATAL_ERROR "OJGL is a 32-bit (Win32/x86) build. Configure with: -A Win32")
endif()

# Four build configurations:
#   Debug          - standard CRT, console, no optimization, _DEBUG paths on
#   OptimizedDebug - standard CRT, console, size-optimized, _DEBUG paths on
#   Release        - tlibc CRT replacement, windows subsystem, min-size shippable binary
#   CrinklerRelease- like Release, but linked+compressed with Crinkler (Ninja generator only)
set(CMAKE_CONFIGURATION_TYPES "Debug;OptimizedDebug;Release;CrinklerRelease" CACHE STRING "" FORCE)

# Music synth backend, selected at configure time (the switch is exclusive: a build is either
# V2 or Clinkster). V2 is the default runtime-streamed .v2m player; CLINKSTER bakes the song
# into build-time assembly and renders it up front. (Leave room for a future 4KLANG value.)
set(OJGL_SYNTH "V2" CACHE STRING "Music synth backend: V2 or CLINKSTER")
set_property(CACHE OJGL_SYNTH PROPERTY STRINGS "V2" "CLINKSTER")

# All real compiler/linker flags are applied per-target via generator expressions so the
# behavior is explicit and diffable. Clear CMake's built-in per-config defaults (which would
# otherwise inject things like /RTC1, /Ob0, /INCREMENTAL) so they don't fight our flags.
foreach(_cfg DEBUG OPTIMIZEDDEBUG RELEASE CRINKLERRELEASE)
    set(CMAKE_CXX_FLAGS_${_cfg} "" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_${_cfg} "" CACHE STRING "" FORCE)
    set(CMAKE_STATIC_LINKER_FLAGS_${_cfg} "" CACHE STRING "" FORCE)
endforeach()

# C runtime: dynamic debug CRT for the debug configs, static release CRT for Release.
# (Requires CMP0091 NEW, the default at cmake_minimum_required 3.21+.)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug,OptimizedDebug>:DebugDLL>")

# CMake injects /EHsc into the base MSVC flags. Exceptions must be OFF for tlibc and for the
# Release ojgl build, so strip it here and re-add /EHsc per-target only where wanted.
string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Crinkler is a drop-in link.exe replacement that also compresses the executable. Only the
# Ninja/Makefile generators honor CMAKE_LINKER (the VS generator lets MSBuild pick the linker),
# so wire it there only; a VS-generator configure is left completely untouched.
#
# CMAKE_LINKER can NOT be made config-dependent via a generator expression - it is substituted
# into the link rule as a literal string (verified empirically: a "$<IF:$<CONFIG:...>,...>"
# placed here is passed through to the linker invocation unevaluated), and a single Ninja
# Multi-Config build tree only ever has one toolchain/linker path shared by all its configs. So
# CMAKE_LINKER has to point at one binary that itself dispatches per-config.
#
# The naive way to do that is to copy Crinkler.exe to a build-local link.exe: since it isn't
# named crinkler.exe, /CRINKLER defaults off, and Crinkler is documented to transparently
# forward its arguments to the real link.exe on PATH in that case. But that passthrough mode
# has a stack-buffer-overrun bug in this Crinkler build (3.0a) - it crashes with
# STATUS_STACK_BUFFER_OVERRUN once the forwarded command line has more than ~5 arguments,
# which made it unusable for Debug/OptimizedDebug/Release (whose real link.exe invocations have
# far more than 5 args). So instead CMAKE_LINKER points at our own tiny dispatcher batch file,
# which inspects its arguments for the literal "/CRINKLER" flag - added via generator
# expression only for CrinklerRelease, see ojgl/CMakeLists.txt - and routes to Crinkler.exe (its
# own native compressing linker, not the buggy passthrough) or straight to the real link.exe
# accordingly, forwarding all arguments unmodified either way.
# Optional escape hatch for offline / air-gapped builds (or to pin a different Crinkler build):
# point this at a directory that directly contains Crinkler.exe and the download below is
# skipped entirely, using that binary as-is. Empty (the default) means auto-download v3.0a.
set(OJGL_CRINKLER_DIR "" CACHE PATH "Directory containing a local Crinkler.exe (skips the download)")

if(CMAKE_GENERATOR MATCHES "Ninja")
    # Crinkler is a prebuilt third-party binary, not checked into the repo. Fetch the official
    # v3.0a release into the build tree (which is gitignored) on first configure, so the source
    # tree stays free of unversioned binaries. Subsequent configures reuse the extracted copy.
    set(_crinkler_dir "${CMAKE_BINARY_DIR}/crinkler")
    set(_crinkler_sha256 "61de655f9ae3e3df98f452a2454e51829f49d41ef15bed27a65803dc5160c5c0")
    if(OJGL_CRINKLER_DIR)
        # User-supplied copy: trust it as-is (skip the download and the v3.0a hash pin, so a
        # locally-cached or alternate Crinkler build can be used without network access).
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
            # The archive unpacks to a top-level crinkler30a/ folder (giving crinkler30a/Win32/...).
            file(ARCHIVE_EXTRACT INPUT "${_crinkler_zip}" DESTINATION "${_crinkler_dir}")
            if(NOT EXISTS "${_crinkler_exe}")
                message(FATAL_ERROR "Crinkler archive did not contain the expected ${_crinkler_exe}")
            endif()
        endif()
        # Verify the binary we're about to run as our linker matches the known-good v3.0a build.
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
    # The real link.exe lives next to cl.exe; resolve it directly instead of relying on PATH
    # order (which, on a machine with Git for Windows installed, can otherwise resolve
    # "link.exe" to Git's own coreutils hard-link tool instead of MSVC's linker).
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
    # CMake defaults to the "-LIBPATH:" spelling for MSVC library directories. link.exe accepts
    # both dash and slash, but Crinkler only understands "/LIBPATH:", so emit the slash form.
    set(CMAKE_LIBRARY_PATH_FLAG "/LIBPATH:")
endif()

add_subdirectory(tlibc)
add_subdirectory(ojgl)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ojgl)
