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
uniform float exposure;
uniform bool specularEnabled;

in vec3 fragEyePos;
in vec4 fragWorldPos;
in vec3 fragWorldNor;

out vec4 fragColor;

float kd = 0.1; // Diffuse coefficient
float ks = 1; // Specular coefficient
int shininess = 200; // Shininess

vec3 calculateLighting(vec3 normal, vec3 viewDir)
{
	vec3 result = vec3(0.0);
	for (int i = 0; i < numLights; i++)
	{
		// Calculate the light direction
		vec3 lightDir = normalize(fragWorldPos.xyz - lights[i].position);
		vec3 halfDir = normalize(lightDir + viewDir);

		// Calculate the lambertian factor
		float lambertian = max(dot(lightDir, normal), 0.0);
		
		// Calculate the specular factor, if enabled
		float specular = 0.0;
		if (specularEnabled)
		{
			specular = pow(max(dot(halfDir, normal), 0.0), shininess);
		}

		// Calculate the light intensity
		vec3 lightIntensity = lights[i].color * lights[i].intensity;

		// Calculate the final color
		result += lambertian * lightIntensity * kd;
		result += specular * lightIntensity * ks;
	}
	
	return result;
}

float luminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}
vec3 tonemap(vec3 hdrColor, float exposure)
{
    vec3 meanColor = textureLod(skybox, vec3(0.5), 100.0).rgb;
	// Calculate the luminance
    float luminanceHdr = luminance(hdrColor);
    float luminanceMean = luminance(meanColor);
    float scaledLuminance = (luminanceHdr / luminanceMean) * exposure;

    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * scaledLuminance);

    // Gamma correction
    mapped = pow(mapped, vec3(1.0/2.2));

    return mapped;
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
	vec3 color = normalize(lightning) * exposure;

	// Apply tone mapping
	color = tonemap(color, exposure);

	// Set the final color
	fragColor = vec4(color, 1.0);

}
