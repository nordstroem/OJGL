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
	outPos.w = 1.0;
	outNormal.xyz = fragNormal;
	outNormal.w = 0.5;
}

)""
