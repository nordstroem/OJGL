# Edison2025 — V2-scored production. Selected with -DOJGL_DEMO=edison2025.
ojgl_add_demo(
    NAME Edison2025
    SYNTH V2
    MUSIC "${CMAKE_CURRENT_SOURCE_DIR}/music/edison_2025_song.inc"
    SOURCES Edison2025.cpp
    SHADERS
        blur1.fs
        borgila.fs
        borgila_hyperspace.fs
        chrom_ab.fs
        chrom_ab_hyperspace.fs
        indoor.fs
        lissajous.fs
        noise.fs
        oj_text.fs
        radar.fs
        stars.fs
        ufo_hyperspace.fs
        ufo_landing.fs
        ufo_landing_utils.fs
        ufo_raymarch_utils.fs
        ufo_scene_1.fs
        ufo_scene_2.fs
        ufo_scene_3.fs
        ufo_scene_4.fs
        ufo_scenes.fs
    COMMON_SHADERS
        noise.fs
        primitives.fs
        quad.vs
        radial_blur.fs
        raymarch_utils.fs
        utils.fs)
