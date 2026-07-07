# Edison2022 — V2-scored production. Selected with -DOJGL_DEMO=edison2022.
ojgl_add_demo(
    NAME Edison2022
    SYNTH V2
    MUSIC "${CMAKE_CURRENT_SOURCE_DIR}/music/edison_2022_song.inc"
    SOURCES Edison2022.cpp
    SHADERS
        ascent.fs
        blur1.fs
        grass.fs
        intro.fs
        jellyfish.fs
        raymarch_settings_refract.fs
        raymarch_utils.fs
        ship.fs
        ship_chrom_ab.fs
    COMMON_SHADERS
        fxaa.fs
        fxaa.vs
        noise.fs
        primitives.fs
        quad.vs
        utils.fs)
