# Template — minimal starting-point production (no music; uses only common/ shaders).
# Selected with -DOJGL_DEMO=template.
ojgl_add_demo(
    NAME Template
    SYNTH None
    SOURCES Template.cpp
    COMMON_SHADERS
        noise.fs
        primitives.fs
        raymarch_settings.fs
        raymarch_template.fs
        raymarch_utils.fs
        utils.fs)
