# tlibc - a tiny custom C runtime that replaces the standard CRT in the size-optimized
# Release build. Produces libct.lib. Only the ojgl Release config links against it.
add_library(tlibc STATIC
    alloc.cpp
    argcargv.cpp
    atol.cpp
    crt0tcon.cpp
    crt0tdll.cpp
    crt0twin.cpp
    file.cpp
    initterm.cpp
    isctype.cpp
    math.cpp
    memory.cpp
    newdel.cpp
    printf.cpp
    puts.cpp
    qsort.cpp
    sprintf.cpp
    string.cpp
    struplwr.cpp
    wstring.cpp
    libct.h
)

# Output name libct -> libct.lib (matches the old <OutputFile>libct.lib</OutputFile>).
set_target_properties(tlibc PROPERTIES OUTPUT_NAME libct)

# tlibc always uses the static, non-debug CRT regardless of build config.
set_target_properties(tlibc PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")

target_compile_definitions(tlibc PRIVATE
    WIN32 _LIB
    _CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_DEPRECATE
    VC_EXTRALEAN WIN32_LEAN_AND_MEAN WIN32_EXTRA_LEAN NOMINMAX
    $<$<CONFIG:Debug,OptimizedDebug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(tlibc PRIVATE
    /W4
    /GR-                                       # no RTTI
    /GS-                                       # no buffer security checks
    /U_MBCS                                    # old project used CharacterSet=NotSet, not MBCS
    # size-optimize + whole-program optimization in the optimized configs
    $<$<CONFIG:OptimizedDebug,Release>:/O1;/Os;/GL>
    # tlibc re-implements CRT functions (strcat, wcscmp, ...) that MSVC would otherwise treat
    # as intrinsics; disable intrinsic generation so they can be defined here. Must come after
    # any /O1 (which implies /Oi).
    /Oi-
)

# memory.cpp defines memcpy/memset/memmove as hand-written byte loops. Under optimization the
# MSVC "loop idiom" pass rewrites those loops into calls to memcpy/memset -- i.e. the function
# calls itself, giving infinite recursion and a STATUS_STACK_OVERFLOW at startup. /Oi- does not
# suppress this pass; only turning optimization (and whole-program opt) off for this TU does.
# The size cost is negligible (a few small byte-copy loops).
set_source_files_properties(memory.cpp PROPERTIES COMPILE_OPTIONS "/Od;/GL-")
