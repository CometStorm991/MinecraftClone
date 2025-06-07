#version 330 core

in vec3 modelPos;
in vec3 norm;
in vec3 fragPos;
in vec2 texCoords;
in float type;

out vec4 fragColor;

uniform vec3 viewPos;

struct MaterialAtlas
{
	sampler2D diffuse;
	sampler2D specular;

	float shininess;
};
uniform MaterialAtlas materialAtlas;

struct DirectionalLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirectionalLight directionalLight;

vec3 calculateDirectionalLight(vec3 normalizedNorm, vec3 normalizedViewDir)
{
	vec3 diffuseColor = vec3(texture(materialAtlas.diffuse, texCoords));
	vec3 specularColor = vec3(texture(materialAtlas.specular, texCoords));
	float shininess = materialAtlas.shininess;

	vec3 normalizedLightDir = normalize(-directionalLight.direction);

	vec3 ambient = diffuseColor * directionalLight.ambient;

	float diffuseAmount = max(dot(normalizedNorm, normalizedLightDir), 0.0f);
	vec3 diffuse = diffuseAmount * diffuseColor * directionalLight.diffuse;

	vec3 reflectDir = reflect(-normalizedLightDir, normalizedNorm);
	float specularAmount = pow(max(dot(normalizedViewDir, reflectDir), 0.0f), shininess);
	vec3 specular = specularAmount * specularColor * directionalLight.specular;

	vec3 result = ambient + diffuse + specular;
	return result;
}

void main()
{
	vec3 normalizedNorm = normalize(norm);
	vec3 normalizedViewDir = normalize(viewPos - fragPos);

	vec3 result = vec3(0.0f);
	result += calculateDirectionalLight(normalizedNorm, normalizedViewDir);

	fragColor = vec4(result, 1.0f);
}