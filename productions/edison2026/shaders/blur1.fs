R""(
uniform vec2 blurDir;

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;

void main()
{
    vec2 uv = fragCoord.xy;
    vec3 col = vec3(0.);
    float samples = 20.;
    float totalWeight = 0.;
    float focus = texture(inTexture0, uv).a;
    float dist = focus * 0.01;
    for (float i = 0.; i < samples; i++) {
        float f = (i - samples / 2.) / (samples / 2.);
        float weight = 1. - pow(abs(f), 3.);
        vec2 uv2 = clamp(uv + blurDir * f * dist, vec2(0.001), vec2(0.999));
        float sampleFocus = texture(inTexture0, uv2).a;
        // A sample may only contribute if its own blur circle reaches this pixel,
        // so in-focus geometry cannot bleed onto its surroundings.
        float offset = abs(f) * dist;
        weight *= clamp(1. - (offset - sampleFocus * 0.02) * 500., 0., 1.);

        totalWeight += weight;

        col += texture(inTexture0, uv2).rgb * weight;
    }
    fragColor.rgb = col / totalWeight;
    fragColor.a = focus;
}

)""
