R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;
uniform float iGlobalTime;

void main()
{
	fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

)""