# Template — minimal starting-point production (no song; uses only common/ shaders).
# Selected with -DOJGL_DEMO=template.
ojgl_add_demo(
    NAME Template
    SYNTH V2
    SOURCES Template.cpp
    COMMON_SHADERS
        noise.fs
        primitives.fs
        raymarch_settings.fs
        raymarch_template.fs
        raymarch_utils.fs
        utils.fs)
