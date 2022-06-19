R""(

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;

void main()
{
	vec2 uv = fragCoord.xy;
    fragColor.r = texture(inTexture0, uv).r;
    float a = 0;
    vec2 dir = normalize(uv - vec2(0.5));
    float l = length(vec2(0.5) - uv);
    vec2 uv2 = clamp(uv + dir * a * 0.01*l, vec2(0.001), vec2(0.999));
    vec2 uv3 = clamp(uv + dir * a * 0.02*l, vec2(0.001), vec2(0.999));
    fragColor.g = texture(inTexture0, uv2).g;
    fragColor.b = texture(inTexture0, uv3).b;
}

)""
