#version 330 core

layout (std140) uniform CameraMatrices
{
	mat4 view;
	mat4 projection;
	vec3 eyePos;
}; 

uniform mat4 model;

layout(location=0) in vec3 inVertex; // the position of the fragment in world space
layout(location=1) in vec3 inNormal; // normal in world space
layout(location=2) in vec2 inTexCoords; // texture coordinates

out vec3 fragEyePos; // the position of the eye in world space
out vec4 fragWorldPos; // the position of the fragment in world space
out vec3 fragWorldNor; // the normal of the fragment in world space

void main(void)
{
	// Pass the eye position
	fragEyePos = eyePos;

	// Pass the world position
	fragWorldPos = model * vec4(inVertex, 1.0);

	// Pass the vertex position to the fragment shader
	gl_Position = projection * view * fragWorldPos;

	// Pass the world normal
	fragWorldNor = normalize(mat3(transpose(inverse(model))) * inNormal);
}