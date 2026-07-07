# InnerSystemLab — V2-scored production. Selected with -DOJGL_DEMO=innersystemlab.
ojgl_add_demo(
    NAME InnerSystemLab
    SYNTH V2
    MUSIC "${CMAKE_CURRENT_SOURCE_DIR}/music/innersystemlab_song.inc"
    SOURCES InnerSystemLab.cpp
    SHADERS
        edison_1.fs
        edison_1_post.fs
        edison_2.fs
        edison_planets.fs
        hospital.fs
    COMMON_SHADERS
        fxaa.fs
        fxaa.vs)
