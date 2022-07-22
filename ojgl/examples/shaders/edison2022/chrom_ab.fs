R""(

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;

void main()
{
	vec2 uv = fragCoord.xy;
    fragColor.r = texture(inTexture0, uv).r;
    vec2 dir = vec2(1.0, 0.0);
    float l = 1.0;
    vec2 uv2 = clamp(uv + dir * 0.004*l, vec2(0.001), vec2(0.999));
    vec2 uv3 = clamp(uv + dir * 0.008*l, vec2(0.001), vec2(0.999));
    fragColor.g = texture(inTexture0, uv2).g;
    fragColor.b = texture(inTexture0, uv3).b;
}

)""
