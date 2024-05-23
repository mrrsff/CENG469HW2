#version 330 core

struct Light
{
	vec3 position;
	float quadratic;
	vec3 ambient;
	float linear;
	vec3 diffuse;
	float constant;
	vec3 specular;
	vec3 colorIntensity;
};

const int MAX_LIGHTS = 32;
layout (std140) uniform Lights
{
	int numLights;
	Light lights[MAX_LIGHTS];
};

layout (std140) uniform CameraMatrices
{
	mat4 view;
	mat4 projection;
	vec3 eyePos;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

uniform mat4 model;
uniform bool useTexture;
// uniform sampler2D tex;

layout(location=0) in vec3 inVertex; // the position of the fragment in world space
layout(location=1) in vec3 inNormal; // normal in world space
layout(location=2) in vec2 inTexCoords; // texture coordinates

out vec4 color;

vec3 calculateLightFactor(Light light, vec3 normal, vec3 eyePos, Material material, vec3 inVertex);
vec3 calculateLightFactor(Light light, vec3 normal, vec3 eyePos, Material material, vec3 inVertex)
{
	// Calculate the light direction
	vec3 lightDir = normalize(light.position - inVertex);

	// Calculate the ambient component
	vec3 ambient = light.ambient * material.ambient;

	// Calculate the diffuse component
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// Calculate the specular component
	vec3 viewDir = normalize(eyePos - inVertex);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	// Calculate the attenuation
	float distance = length(light.position - inVertex);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// Calculate the final light factor
	vec3 lightFactor = (ambient + diffuse + specular) * attenuation * light.colorIntensity;

	return lightFactor;
}

void main(void)
{
	// Uncomment to debug normals
	// color = vec4(normalize(inNormal), 1.0);

	// White color
	color = vec4(1.0, 1.0, 1.0, 1.0);

	// Calculate the normal matrix
	mat3 normalMatrix = transpose(inverse(mat3(model)));

	// Calculate the normal in eye space
	vec3 normal = normalize(normalMatrix * inNormal);

	// Calculate the light
	vec3 light = vec3(0.0);
	for (int i = 0; i < numLights; i++)
	{
		light += calculateLightFactor(lights[i], normal, eyePos, material, inVertex);
	}

	// Calculate the final color
	if (useTexture)
	{
		// color *= texture(tex, inTexCoords) * vec4(light, 1.0);
	}
	else
	{
		color *= vec4(light, 1.0);
	}

	// Calculate the final position
	gl_Position = projection * view * model * vec4(inVertex, 1.0);
}