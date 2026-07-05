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

# Which software synthesizer produces the music. This is an exclusive, configure-time choice:
#   V2     - the V2 synth (libv2.lib); song is a runtime V2M byte array from Demo::getSong().
#   4KLANG - the 4klang synth; the song is baked into build-time assembly (assembled by YASM,
#            see ojgl/CMakeLists.txt) and rendered up front. Selected via -DOJGL_SYNTH=4KLANG.
set(OJGL_SYNTH "V2" CACHE STRING "Music synth backend: V2 or 4KLANG")
set_property(CACHE OJGL_SYNTH PROPERTY STRINGS "V2" "4KLANG")
if(NOT OJGL_SYNTH STREQUAL "V2" AND NOT OJGL_SYNTH STREQUAL "4KLANG")
    message(FATAL_ERROR "OJGL_SYNTH must be 'V2' or '4KLANG', got '${OJGL_SYNTH}'")
endif()

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
# so wire it there only; a VS-generator configure is left completely untouched. We copy
# Crinkler.exe to a build-local link.exe: because it is NOT named crinkler.exe, /CRINKLER
# defaults off, so for every config except CrinklerRelease (which passes /CRINKLER) it simply
# forwards its arguments to the real link.exe on PATH.
if(CMAKE_GENERATOR MATCHES "Ninja")
    configure_file(
        "${CMAKE_SOURCE_DIR}/crinkler30a/crinkler30a/Win32/Crinkler.exe"
        "${CMAKE_BINARY_DIR}/crinkler/link.exe" COPYONLY)
    set(CMAKE_LINKER "${CMAKE_BINARY_DIR}/crinkler/link.exe")
    # CMake defaults to the "-LIBPATH:" spelling for MSVC library directories. link.exe accepts
    # both dash and slash, but Crinkler only understands "/LIBPATH:", so emit the slash form.
    set(CMAKE_LIBRARY_PATH_FLAG "/LIBPATH:")
endif()

add_subdirectory(tlibc)
add_subdirectory(ojgl)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ojgl)
