R""(
in vec2 fragCoord;
out vec4 fragColor;

uniform float iTime;
uniform sampler2D inTexture0;

uniform float startFadeTime;
uniform float endFadeTime;
uniform bool fadeIn; 

void main()
{
	float f = smoothstep(startFadeTime, endFadeTime, iTime);
	if (!fadeIn)
		f = 1.0 - f;
	fragColor = f * texture(inTexture0, fragCoord.xy);
}

)""
