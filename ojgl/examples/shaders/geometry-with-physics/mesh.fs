R""(
#version 430
#include "shaders/common/noise.fs"

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 outPos;
layout(location = 1) out vec4 outNormal;


uniform sampler2D inTexture0;
uniform vec2 iResolution;
uniform float iGlobalTime;
uniform float iTime;

void main()
{
	outPos.xyz = fragPos;
	outPos.w = 0.5;
	outNormal.xyz = fragNormal;
	outNormal.w = 0.5;

	vec2 uv = fragTexCoord - 0.5;

	vec3 uv3 = vec3(uv, 0.);
	float d = length(uv);
	float alpha = fbm3_high(uv3 * 2.5 + vec3(iTime), 0.16, 3.7);
	
	alpha *= 1.f - smoothstep(0.0, 0.4 + 0.1*noise_2(9*uv + vec2(cos(sin(iTime)) + iTime, sin(sin(iTime)))), d);
	outNormal.w = alpha;

}

)""
