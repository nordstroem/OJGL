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
if(CMAKE_GENERATOR MATCHES "Ninja")
    configure_file(
        "${CMAKE_SOURCE_DIR}/crinkler30a/crinkler30a/Win32/Crinkler.exe"
        "${CMAKE_BINARY_DIR}/crinkler/crinkler.exe" COPYONLY)
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
