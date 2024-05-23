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

in vec3 fragEyePos; // Eye position
in vec4 fragWorldPos; // World position
in vec3 fragWorldNor; // World normal

out vec4 fragColor;

float kt = 0.5; // Transmission coefficient

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
	// // DEBUGGING
	// // Directly map the skybox texture to the fragment color
	// fragColor = texture(skybox, fragWorldNor);

    // Sample the refraction color
    vec3 viewDir = normalize(fragWorldPos.xyz - fragEyePos);
    vec3 refractionVector = refract(viewDir, fragWorldNor, 1.0 / 1.5);
    vec4 refractedColor = texture(skybox, refractionVector) * kt;

    vec3 finalColor = tonemap(refractedColor.rgb, exposure);
    fragColor = vec4(finalColor, 1.0);
}
