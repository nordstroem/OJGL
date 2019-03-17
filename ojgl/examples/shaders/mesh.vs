R""(
#version 430
layout (location = 0) in vec3 position;
out vec3 fragPos;

uniform mat4 M;

void main()
{
	gl_Position = M * vec4(position, 1.0);
	fragPos = (M*vec4(position, 1.0)).xyz;
}
)""
