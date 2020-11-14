R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;

uniform float CHANNEL_11_SINCE;
uniform float CHANNEL_4_TO;
uniform float CHANNEL_4_TOTAL;

void main()
{
	vec2 uv = fragCoord.xy;
	fragColor = texture(inTexture0, uv);
	fragColor.rgb *=  0.9 + 0.1*clamp(sin(uv.y*1000) + 0.8, 0.0, 1.0);
}
)""