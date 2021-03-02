R""(
#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 fragCoord;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main()
{
	fragCoord.x = (position.x + 1) * 1 / 2.0;
	fragCoord.y = (position.y + 1) * 1 / 2.0;
	gl_Position = vec4(position, 1);

	fragNormal = normal;
	fragTexCoord = texCoord;
}
)""
