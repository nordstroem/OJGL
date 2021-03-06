R""(

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;
uniform float iGlobalTime;


void main()
{
	vec2 uv = fragCoord.xy;

	fragColor = texture(inTexture0, uv);
	

	//fragColor.rgb = mix(vec3(1,0,0), fragColor.rgb, min(red, 1.0));

	float a = 0.5 + 0.5 * sin(iGlobalTime* 5.0);
	//a = a * a * a;
	a = 0.5;
	vec2 dir = normalize(uv - vec2(0.5));
	float l = length(vec2(0.5) - uv);
	fragColor.g = texture(inTexture0, uv + dir * a * 0.01*l).g;
	fragColor.b = texture(inTexture0,  uv + dir * a * 0.02*l).b;

	fragColor.rgb *=  0.95 + 0.05*clamp(sin(uv.y*1000) + 0.8, 0.0, 1.0);
}
)""