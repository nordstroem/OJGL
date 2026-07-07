# Edison2021 — V2-scored production. Selected with -DOJGL_DEMO=edison2021.
ojgl_add_demo(
    NAME Edison2021
    SYNTH V2
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
        quad.vs
        radial_blur.fs
        raymarch_utils.fs
        utils.fs)
