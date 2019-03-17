R""(
#version 430

layout (location = 0) in vec3 position;
out vec2 fragCoord;

uniform vec2 iResolution;

void main()
{
	fragCoord.x = (position.x + 1)/2 * iResolution.x;
	fragCoord.y = (position.y + 1)/2 * iResolution.y;
    gl_Position = vec4(position, 1);
}
)""