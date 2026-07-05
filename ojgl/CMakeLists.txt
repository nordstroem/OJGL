# ojgl - the framework + demos executable.

set(OJGL_SOURCES
    # examples/demos/DodensTriumf.cpp
    # examples/demos/Edison2021.cpp
    # examples/demos/Edison2022.cpp
    examples/demos/Edison2025.cpp
    # examples/demos/Edison2026.cpp
    # examples/demos/Eldur.cpp
    # examples/demos/InnerSystemLab.cpp
    # examples/demos/QED.cpp
    # examples/demos/Template.cpp
    examples/FreeCameraController.cpp
    examples/Main.cpp
    examples/TextRenderer.cpp
    src/thirdparty/StbImage.cpp
    src/music/Music.cpp
    src/music/SyncChannel.cpp
    src/music/V2MPlayer.cpp
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

# Non-compiled files (headers, GLSL shaders, embedded songs). Included in the target purely so
# they appear in the generated Visual Studio solution, mirroring the old <None>/<ClInclude> items.
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

target_include_directories(ojgl PRIVATE src examples)

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
    "${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/libv2.lib"
    opengl32 kernel32 user32 gdi32 winspool comdlg32 advapi32 shell32
    ole32 oleaut32 uuid odbc32 odbccp32
    # Release/CrinklerRelease replace the standard CRT with tlibc and pull in the DirectX libs.
    $<$<CONFIG:Release,CrinklerRelease>:tlibc>
    $<$<CONFIG:Release,CrinklerRelease>:DDraw>
    $<$<CONFIG:Release,CrinklerRelease>:DSound>
    $<$<CONFIG:Release,CrinklerRelease>:Dinput8>
)

target_link_options(ojgl PRIVATE
    # libv2.lib modules are not SAFESEH-compatible. Crinkler builds its own imports and does not
    # understand /SAFESEH, so it is applied to every config except CrinklerRelease.
    $<$<NOT:$<CONFIG:CrinklerRelease>>:/SAFESEH:NO>
    $<$<CONFIG:Debug>:/DEBUG:FASTLINK>
    $<$<CONFIG:OptimizedDebug>:/DEBUG:FASTLINK;/OPT:REF;/OPT:ICF>
    # Release: ignore default libs (tlibc is the CRT), enter through mainCRTStartup, fold/trim,
    # emit a .map, and run link-time codegen (tlibc objects are compiled with /GL).
    $<$<CONFIG:Release>:/DEBUG;/NODEFAULTLIB;/ENTRY:mainCRTStartup;/OPT:REF;/OPT:ICF;/MAP;/LTCG>
    # CrinklerRelease: Crinkler links + compresses. /NODEFAULTLIB keeps it from auto-importing
    # msvcrt/inserting its own startup (tlibc is the CRT); /MANIFEST:NO so CMake's vs_link_exe
    # wrapper skips mt.exe (Crinkler does not support manifests). No /GL/LTCG objects reach it.
    # /REUSE lets Crinkler read/write a reuse file that records the part layout. It is required
    # to split an oversized part: /REUSEMODE:PARTS (the default) honors the parts defined in the
    # file (so a hand-added "Code2" part is respected) while recalculating section ordering,
    # models, and hash size. The file is written before the 64k check, so even a failing build
    # produces a complete file to edit.
    $<$<CONFIG:CrinklerRelease>:/NODEFAULTLIB;/ENTRY:mainCRTStartup;/MANIFEST:NO;/CRINKLER;/RANGE:opengl32;/COMPMODE:FAST;/TRANSFORM:CALLS;/REUSE:ojgl_crinkler_reuse.txt;/REUSEMODE:PARTS;/REPORT:ojgl_crinkler_report.html;/PRINT:IMPORTS;/HASHTRIES:10;/HASHSIZE:1000>
)
