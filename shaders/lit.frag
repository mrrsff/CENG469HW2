#version 330 core

#define MAX_LIGHTS 128

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct LightSource {
	vec3 position;
	vec3 color;
	float intensity;
};

layout (std140) uniform Lights
{
	int numLights;
	LightSource lights[MAX_LIGHTS];
};

uniform mat4 model;
uniform samplerCube skybox;
uniform Material material;

in vec3 fragEyePos;
in vec4 fragWorldPos;
in vec3 fragWorldNor;

out vec4 fragColor;

vec3 samleFromCubeMap(vec3 direction)
{
	// irradiance map
	vec3 irradiance = texture(skybox, direction).rgb;

	// scale to keep the colors consistent
	irradiance = irradiance * 0.1;

	return irradiance;
}

vec3 calculateLighting(vec3 normal, vec3 viewDir)
{
	vec3 result = vec3(0.0);
	for (int i = 0; i < numLights; i++)
	{
		// Calculate the light direction
		vec3 lightDir = normalize(lights[i].position - fragWorldPos.xyz);
		vec3 halfDir = normalize(lightDir + viewDir);

		// Calculate the lambertian factor
		float lambertian = max(dot(lightDir, normal), 0.0);
		float specular = pow(max(dot(halfDir, normal), 0.0), material.shininess);

		// Calculate the light intensity
		vec3 lightIntensity = lights[i].color * lights[i].intensity;

		// Calculate the final color
		result += (material.ambient + material.diffuse * lambertian + material.specular * specular) * lightIntensity;
	}
	
	return result;
}

void main(void)
{
	// Calculate the normal
	vec3 normal = normalize(fragWorldNor);

	// Calculate the view direction
	vec3 viewDir = normalize(fragEyePos - fragWorldPos.xyz);

	// Calculate the lightning
	vec3 lightning = calculateLighting(normal, viewDir);

	// Calculate the final color
	vec3 color = normalize(lightning);

	// Set the final color
	fragColor = vec4(color, 1.0);

}
