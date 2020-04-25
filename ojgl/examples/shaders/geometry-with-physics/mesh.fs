R""(
#version 430

in vec3 fragPos;
in vec3 fragNormal;

layout(location = 0) out vec4 outPos;
layout(location = 1) out vec4 outNormal;


uniform sampler2D inTexture0;
uniform float iGlobalTime;

void main()
{
	outPos.xyz = fragPos;
	outPos.w = 0.5;
	outNormal.xyz = fragNormal;
}

)""
