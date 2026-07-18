ojgl_add_demo(
    NAME Edison2021
    SYNTH V2
    MUSIC "${CMAKE_CURRENT_SOURCE_DIR}/music/edison_2021_song.inc"
    SOURCES Edison2021.cpp
    SHADERS
        blob.fs
        chrom_ab.fs
        introText.fs
        pipes.fs
        plane.fs
        raymarch_settings.fs
        tunnel_base.fs
        tunnel_transform.fs
    COMMON_SHADERS
        fade.fs
        fxaa.fs
        fxaa.vs
        noise.fs
        primitives.fs
        radial_blur.fs
        raymarch_utils.fs
        utils.fs)
