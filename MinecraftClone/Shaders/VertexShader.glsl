#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
layout (location = 3) in float aType;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;

out vec3 modelPos;
out vec3 norm;
out vec3 fragPos;
out vec2 texCoords;
out float type;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);

	modelPos = aPos;
	norm = vec3(normalMatrix * vec4(aNorm, 1.0f));
	fragPos = vec3(model * vec4(aPos, 1.0f));
	texCoords = aTex;
	type = aType;
}