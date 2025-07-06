R""(


uniform sampler2D ojTexture;
uniform sampler2D feedbackTexture0;
uniform float iTime;
uniform vec2 iResolution;
in vec2 fragCoord;
out vec4 fragColor;

float circle(vec2 uv)
{
    return abs(length(uv) - 0.5);
}

void main()
{
    float u = (fragCoord.x - 0.5);
    float v = (fragCoord.y - 0.5);
    vec2 uv = vec2(u, v);

    float lineWidth = 0.005;
    float dc = circle(uv);
    float alpha = smoothstep(lineWidth, 0.0, dc);
    vec3 color = mix(vec3(0.0), vec3(1.0, 0.0, 0.0), alpha);
    fragColor = vec4(color, 1.0);
}

)""
