ojgl_add_demo(
    NAME QED
    SYNTH V2
    MUSIC "${CMAKE_CURRENT_SOURCE_DIR}/music/qed_song.inc"
    SOURCES QED.cpp
    SHADER_PREFIX QED
    SHADERS
        blur1.fs
        post.fs
        rayMarch.fs
        rayMarch.vs
        raymarch_utils.fs
        raymarch_utils_tower.fs
        sphere.fs
        sphere.vs
        tower.fs
    COMMON_SHADERS
        noise.fs
        primitives.fs
        utils.fs)
