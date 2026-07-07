# Edison2026 — Clinkster-scored production. Selected with -DOJGL_DEMO=edison2026.
ojgl_add_demo(
    NAME Edison2026
    SYNTH CLINKSTER
    SOURCES Edison2026.cpp
    MUSIC "${CMAKE_CURRENT_SOURCE_DIR}/music/song.asm"
    SHADERS
        cube.fs
    COMMON_SHADERS
        quad.vs)
