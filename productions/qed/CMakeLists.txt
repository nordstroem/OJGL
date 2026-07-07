# QED — V2-scored production. Selected with -DOJGL_DEMO=qed.
# Shaders use the "QED/" virtual prefix (uppercase), so override the default (lowercased NAME).
ojgl_add_demo(
    NAME QED
    SYNTH V2
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
        quad.vs
        utils.fs)
