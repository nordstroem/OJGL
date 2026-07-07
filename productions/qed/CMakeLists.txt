# QED — V2-scored production. Selected with -DOJGL_DEMO=qed.
# Shaders use the "QED/" virtual prefix (uppercase), so override the default (lowercased NAME).
ojgl_add_demo(
    NAME QED
    SYNTH V2
    SOURCES QED.cpp
    SHADER_PREFIX QED)
