#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "typedefs.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "GameObject.h"
#include "Light.h"
#include "Camera.h"
#include "utils.h"
#include "Texture.h"
#include "printExtensions.h"

#include <vector>
#include <string>
#include <iostream>

// Lights: binding = 0
// Camera: binding = 1
const int MAX_LIGHTS = 256;
struct __light {
	Vector3 position;
	int padding;
	Vector3 color;
	float intensity;
};
struct __lights {
	int numLights;
	int padding[3];
    __light lightSources[MAX_LIGHTS]; // Assuming MAX_LIGHTS is defined
};
struct __camera {
	Matrix4 view;
	Matrix4 projection;
	Vector3 eyePos;
};
struct __material{
	Vector3 ambient;
	float shininess;
	Vector3 diffuse;
	float padding;
	Vector3 specular;
	float padding2;
};

class MeshRenderer {
public:
	MeshRenderer();
	~MeshRenderer();

	void SetCubemap(Texture* cubemapTexture);
	void SetExposure(float exposure);
	void SetSpecularEnabled(bool enabled);
	void SetLights(std::vector<Light*>* lights);
	void SetCamera(Camera* camera);
	void Draw(GameObject* gameObject);

	void UpdateCameraUBO();
	void UpdateLightsUBO();

private:
	GLvoid* cameraDataPtr;
	GLuint cameraUBO;
	__camera cameraData;
	Camera* camera;
	
	Texture* cubemapTexture;
	GLuint sampler;

	GLvoid* lightsDataPtr;
	GLuint lightsUBO;
	__lights lightsData;
	std::vector<Light*>* lights;

	float exposure;
	bool specularEnabled;
	
	void setupCameraUBO();
	void setupLightsUBO();
};

#endif