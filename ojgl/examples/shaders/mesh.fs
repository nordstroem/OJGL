R""(
#version 430

in vec2 fragPos;
in vec3 fragNormal;

layout(location = 0) out vec4 outPos;
layout(location = 1) out vec4 outNormal;


uniform sampler2D inTexture0;
uniform float iGlobalTime;

void main()
{
	vec3 col = vec3(1.0, 0.0, 0.0);
	vec3 invLight = vec3(1.0, 0.0, 0.0);
	col = col * (0.5 + 0.5 * max(0.0, dot(fragNormal, invLight)));
	fragColor.rgb = col;
	fragColor.a = 1.0;
	//fragColor.rgb = vec3(0.5) + fragNormal * 0.5;
	outNormal = fragNormal;
}

)""
