# DodensTriumf — V2-scored production. Selected with -DOJGL_DEMO=dodenstriumf.
ojgl_add_demo(
    NAME DodensTriumf
    SYNTH V2
    MUSIC "${CMAKE_CURRENT_SOURCE_DIR}/music/dodenstriumf_song.inc"
    SOURCES DodensTriumf.cpp
    SHADERS
        graveScene.fs
        graveScenePost.fs
        introScene.fs
        roomScene.fs
        roomScenePost.fs
    COMMON_SHADERS
        fxaa.fs
        fxaa.vs)
