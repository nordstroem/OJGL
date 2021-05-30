R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;
	
uniform sampler2D inTexture0;
uniform sampler2D feedbackTexture0;
uniform float iTime;

void main()
{
	vec2 uv = fragCoord.xy;
	vec4 white = vec4(1.0);
	
	vec4 prevCol = texture(feedbackTexture0, uv);
	vec4 red = texture(inTexture0, uv);

	if (iTime < 1.0)
		fragColor = vec4(0.0);
	else {
		fragColor = mix(prevCol, red, 0.001);
	}
}
)""