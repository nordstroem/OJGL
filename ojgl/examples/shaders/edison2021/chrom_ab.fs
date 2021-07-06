R""(

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;
uniform float drumSince;

void main()
{
	vec2 uv = fragCoord.xy;
    fragColor.r = texture(inTexture0, uv).r;
    float a = 0.1 + 1.0 * exp(-10.0 * drumSince);
    vec2 dir = vec2(1.0, 0.0);//normalize(uv - vec2(0.5));
    float l = 1.0; //length(vec2(0.5) - uv);
    vec2 uv2 = clamp(uv + dir * a * 0.01*l, vec2(0.001), vec2(0.999));
    vec2 uv3 = clamp(uv + dir * a * 0.02*l, vec2(0.001), vec2(0.999));
    fragColor.g = texture(inTexture0, uv2).g;
    fragColor.b = texture(inTexture0, uv3).b;
}

)""
