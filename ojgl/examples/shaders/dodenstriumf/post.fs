R""(
#version 430

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D inTexture0;
uniform float r;

void main() {
	vec3 col = texture2D(inTexture0, fragCoord).xyz;
	col = mix(col, vec3(1.0, 1.0, 0.0), 0.5);
	fragColor = vec4(1.0, 0.0, 1.0, 1.0);

}
)""