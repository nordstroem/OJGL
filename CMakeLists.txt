cmake_minimum_required(VERSION 3.21)

# OJGL is a Windows / OpenGL demoscene framework. It only builds with MSVC targeting
# Win32 (x86). Configure with the Visual Studio generator, e.g.:
#   cmake -S . -B build -G "Visual Studio 17 2022" -A Win32
project(OJGL CXX)

if(NOT MSVC)
    message(FATAL_ERROR "OJGL only builds with MSVC (cl.exe). Use the Visual Studio generator.")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "OJGL is a 32-bit (Win32/x86) build. Configure with: -A Win32")
endif()

# Three build configurations survive the migration from the old .sln:
#   Debug          - standard CRT, console, no optimization, _DEBUG paths on
#   OptimizedDebug - standard CRT, console, size-optimized, _DEBUG paths on
#   Release        - tlibc CRT replacement, windows subsystem, min-size shippable binary
set(CMAKE_CONFIGURATION_TYPES "Debug;OptimizedDebug;Release" CACHE STRING "" FORCE)

# All real compiler/linker flags are applied per-target via generator expressions so the
# behavior is explicit and diffable. Clear CMake's built-in per-config defaults (which would
# otherwise inject things like /RTC1, /Ob0, /INCREMENTAL) so they don't fight our flags.
foreach(_cfg DEBUG OPTIMIZEDDEBUG RELEASE)
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

add_subdirectory(tlibc)
add_subdirectory(ojgl)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ojgl)
