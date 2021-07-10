R""(

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;
uniform float iTime;

void main()
{
	vec2 uv = fragCoord.xy;
	fragColor = texture(inTexture0, uv);
}
)""