# ojgl - the framework + demos executable.

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
    # ShaderReader resolves shader paths relative to examples/shaders, so the debugger must run
    # from the ojgl/ directory (see CLAUDE.md).
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
