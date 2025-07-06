R""(
#include "common/utils.fs"

uniform sampler2D ojTexture;
uniform sampler2D feedbackTexture0;
uniform float iTime;
uniform vec2 iResolution;
in vec2 fragCoord;
out vec4 fragColor;

float circles(vec2 uv)
{
    float q = length(uv);
    if (q > 0.5) {
        return 0.5;
    }
    q = mod(q, 0.1);
    return abs(q - 0.05);
}

float lines(vec2 uv) {
    pModPolar(uv, 12);
    uv = uv.yx;
    float q = length(uv);
    if (q > 0.45 || q < 0.05) {
        return 0.5;
    }
    return abs(uv.x);
}

float radar(vec2 uv) {
    uv *= rot(-0.5*iTime);
    float q = length(uv);
    if (q > 0.45 || uv.y < 0.01) {
        return 0.5;
    }

    return abs(uv.x);
}

void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5);
    vec2 uv = vec2(u, v);

    float lineWidth = 0.005;
    float dc = circles(uv);
    float dl = lines(uv);
    float dr = radar(uv);
    float d = min(dc, dl);
    d = min(d, dr);
    float alpha = smoothstep(lineWidth, 0.0, d);
    vec3 color = mix(vec3(0.0), vec3(1.0, 0.0, 0.0), alpha);


    vec3 prevColor = texture(feedbackTexture0, fragCoord).xyz;
    color = min(color + prevColor, vec3(1.0));
    color = mix(color, vec3(0.0), 0.2);
    fragColor = vec4(color, 1.0);
}

)""
