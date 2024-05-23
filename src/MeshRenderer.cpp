#include "MeshRenderer.h"

MeshRenderer::MeshRenderer() {}

MeshRenderer::~MeshRenderer() {}

void MeshRenderer::SetCubemap(Texture* cubemapTexture) {
	this->cubemapTexture = cubemapTexture;
	// Create a sampler
	glGenSamplers(1, &sampler);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void MeshRenderer::SetCamera(Camera* camera) {
	this->camera = camera;
	setupCameraUBO();
}

void MeshRenderer::SetExposure(float exposure) {
	this->exposure = exposure;
}

void MeshRenderer::SetSpecularEnabled(bool enabled) {
	this->specularEnabled = enabled;
}

void MeshRenderer::SetLights(std::vector<Light*>* lights) {
	this->lights = lights;
	setupLightsUBO();
}

void MeshRenderer::Draw(GameObject* gameObject) {
	ShaderProgram* shader = gameObject->shader;
	if (shader == nullptr)
	{
		std::cerr << "No shader attached to the game object " << gameObject->name << std::endl;
		return;
	}
	shader->use();

	// Bind cube map texture
	glActiveTexture(GL_TEXTURE0 + cubemapTexture->getTextureUnit());
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture->getID());

    // Set the sampler
    glBindSampler(cubemapTexture->getTextureUnit(), sampler);

	// Set the exposure
	shader->setFloat("exposure", this->exposure);
	shader->setBool("specularEnabled", this->specularEnabled);
	
	// Bind the camera UBO
	GLuint cameraUBOIndex = glGetUniformBlockIndex(shader->getID(), "CameraMatrices");
	glUniformBlockBinding(shader->getID(), cameraUBOIndex, 1);
	assert(glGetError() == GL_NO_ERROR);

	// Bind the lights UBO
	GLuint lightsUBOIndex = glGetUniformBlockIndex(shader->getID(), "Lights");
	glUniformBlockBinding(shader->getID(), lightsUBOIndex, 0);
	assert(glGetError() == GL_NO_ERROR);
	
	// Set the model matrix
	shader->setMat4("model", gameObject->getModelingMatrix());

	gameObject->mesh->Draw();

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindSampler(cubemapTexture->getTextureUnit(), 0);

	shader->unuse();
}

void MeshRenderer::setupCameraUBO() {
	/*
		layout (std140, binding = 1) uniform Matrices
		{
			mat4 view;
			mat4 projection;
			vec3 eyePos;
		};
	*/
	glGenBuffers(1, &cameraUBO);
	UpdateCameraUBO(); 
}

void MeshRenderer::UpdateCameraUBO() {
	cameraData.view = *camera->getViewMatrix();
	cameraData.projection = *camera->getProjectionMatrix();
	cameraData.eyePos = camera->getPosition();

	glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO); // Bind buffer
	glBufferData(GL_UNIFORM_BUFFER, sizeof(__camera), &cameraData, GL_STATIC_DRAW); // Set buffer data
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, cameraUBO); // Bind buffer to binding point 1

	glBindBuffer(GL_UNIFORM_BUFFER, 0); // Unbind buffer
	assert(glGetError() == GL_NO_ERROR);
}

void MeshRenderer::setupLightsUBO()
{
	/*
		layout (std140, binding = 0) uniform Lights
		{
			Light lightSources[MAX_LIGHTS];
			int numLights;
		};
	*/
	glGenBuffers(1, &lightsUBO);
	UpdateLightsUBO();
}

void MeshRenderer::UpdateLightsUBO() {
	/*
		layout (std140, binding = 0) uniform Lights
		{
			Light lightSources[MAX_LIGHTS];
			int numLights;
		};
	*/
	lightsData = __lights(); 
	lightsData.numLights = lights->size();
	for (int i = 0; i < lights->size(); i++) {
		Light* light = lights->at(i);
		lightsData.lightSources[i].position = light->position;
		lightsData.lightSources[i].color = light->color;
		lightsData.lightSources[i].intensity = light->intensity;
	}

	int size = sizeof(__lights);
	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO); // Bind buffer
	glBufferData(GL_UNIFORM_BUFFER, size, &lightsData, GL_STATIC_DRAW); // Set buffer data
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightsUBO); // Bind buffer to binding point 0

	glBindBuffer(GL_UNIFORM_BUFFER, 0); // Unbind buffer
	assert(glGetError() == GL_NO_ERROR);
}