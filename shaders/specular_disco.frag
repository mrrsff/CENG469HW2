#version 330 core

#define MAX_LIGHTS 128

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct LightSource {
	vec3 position; // Direction is -position
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

in vec3 fragEyePos;
in vec4 fragWorldPos;
in vec3 fragWorldNor;

out vec4 fragColor;

float ks = 1; // Specular coefficient
int shininess = 16000; // Shininess

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

vec3 calculateLighting(vec3 normal, vec3 viewDir)
{
	vec3 result = vec3(0.0);
	for (int i = 0; i < numLights; i++)
	{
		// Calculate the light direction
		vec3 lightDir = normalize(-lights[i].position);
		vec3 halfDir = normalize(lightDir + viewDir);

		float specular = pow(max(dot(halfDir, normal), 0.0), shininess);
		if(specular == 0.0) continue;

		// Calculate the light intensity
		vec3 lightIntensity = lights[i].color * lights[i].intensity;
		// lightIntensity /= (vec3(1) + lightIntensity); // Normalize the light intensity
		// lightIntensity = tonemap(lightIntensity, exposure);
		
		// Calculate the specular color
		vec3 specularColor = specular * lightIntensity;

		result += specularColor;
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
	vec3 color = normalize(lightning) * exposure;
	// color = tonemap(color, exposure);

	// vec3 reflectionVector = reflect(normalize(-viewDir), normalize(fragWorldNor));

	// vec4 reflectedColor = texture(skybox, reflectionVector);
	// reflectedColor.rgb = tonemap(reflectedColor.rgb, exposure);

	// color = color + ks * reflectedColor.rgb;
	// Calculate the final color
	fragColor = vec4(color, 1.0);

	// // Set the final color
	// fragColor = reflectedColor;

}
