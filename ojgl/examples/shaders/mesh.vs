R""(
#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 fragPos;
out vec3 fragNormal;

uniform mat4 M;
uniform mat4 P;

void main()
{
	gl_Position = P * M * vec4(position, 1.0);
	fragPos = (M*vec4(position, 1.0)).xyz;
	fragNormal = normalize(mat3(M)*normal);
}
)""
