# Eldur — V2-scored production. Selected with -DOJGL_DEMO=eldur.
ojgl_add_demo(
    NAME Eldur
    SYNTH V2
    SOURCES Eldur.cpp
    SHADERS
        lavaIntro.fs
        lavaScene2.fs
        mountain.fs
        mountainNoise.fs
        mountainPost.fs
        outro.fs
        post.fs
    COMMON_SHADERS
        fxaa.fs
        fxaa.vs
        quad.vs)
