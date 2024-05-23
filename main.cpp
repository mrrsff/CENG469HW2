#define _USE_MATH_DEFINES
#define GLEW_STATIC
#define CALLBACK

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <math.h> 
#include <filesystem>

#include <GL/glew.h>
// #include <GL/gl.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header

#include <glm/glm.hpp> // GL Math library header

#include "typedefs.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Light.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "ShaderProgram.h"
#include "utils.h"
#include "constTypes.h"
#include "printExtensions.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "EnvironmentRenderer.h"
#include "IBLSampler.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Paths
const std::string hdriTestPath = "hdr_equirectengular_maps/Test.hdr";
const std::string hdriPath = "hdr_equirectengular_maps/Thumersbach.hdr";

const std::string cubeObjectPath = "obj/cube24.obj";
const std::string sphereObjectPath = "obj/sphere.obj";

const std::string vertexShaderPath = "shaders/lit.vert";
const std::string fragmentShaderPath = "shaders/lit.frag";

const std::string lightProbeFragmentShaderPath = "shaders/light_probe.frag";
const std::string mirrorFragmentShaderPath = "shaders/mirror.frag";
const std::string glassFragmentShaderPath = "shaders/glass.frag";
const std::string glossyFragmentShaderPath = "shaders/glossy.frag";
const std::string specularDiscoFragmentShaderPath = "shaders/specular_disco.frag";

enum DrawMode
{
	LIGHT_PROBE = 1,
	MIRROR,
	GLASS,
	GLOSSY,
	SPECULAR_DISCO,
};
DrawMode drawMode = LIGHT_PROBE;
bool specularEnabled = true;
std::map<DrawMode, ShaderProgram*> shaderPrograms;

// Light count
int minDirectLightCount = 1;
int maxDirectLightCount = 7;
int directionalLightPow = 2; // 2^n	// 1, 2, 4, 8, 16, 32, 64

// Window dimensions
GLuint WIDTH = 1280, HEIGHT = 720;
Vector3 backgroundColor = Vector3(0.2f, 0.2f, 0.4f);
std::string windowTitle = "CENG469 HW2 OPENGL";

// Window
GLFWwindow* window;

// Scene components
Camera* mainCamera;
std::vector<Light*> lights;
MeshRenderer* meshRenderer;
EnvironmentRenderer* environmentRenderer;
IBLSampler* iblSampler;
Framebuffer* hdriToCubemapFramebuffer;
Mesh *cubeMesh, *sphereMesh;
Texture* hdriTexture;
Texture* skyboxTexture;

Material* shinyMaterial;

int rotationDirection = 0; // 0: no rotation, 1: right, -1: left


// Game objects (all necessary components for rendering is in here)
GameObject* sphere;

void CreateWindow();
void reshape(GLFWwindow* window, int w, int h);
void inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mainLoop(GLFWwindow* window);
void init();
void drawObjects();
void update();
void rotateCamera(float yaw, float pitch);

void init()
{
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glEnable(GL_CULL_FACE);

	// Create skybox texture
	hdriTexture = new Texture(hdriPath, true);
	hdriTexture->setTextureUnit(0);
	assert(glGetError() == GL_NO_ERROR);
	std::cout << "HDRI texture loaded" << std::endl;

	// Create skybox mesh
	cubeMesh = ParseObjFile(cubeObjectPath.c_str(), true, true);
	assert(cubeMesh != nullptr);
	std::cout << "Cube mesh loaded" << std::endl;

	// Create framebuffer for cubemap creation
	hdriToCubemapFramebuffer = Framebuffer::CreateFramebuffer(1024, 1024, hdriTexture);
	assert(glGetError() == GL_NO_ERROR);
	std::cout << "Framebuffer created" << std::endl;

	// Create environment renderer
	environmentRenderer = new EnvironmentRenderer(hdriToCubemapFramebuffer, cubeMesh);
	assert(glGetError() == GL_NO_ERROR);
	std::cout << "Environment renderer created" << std::endl;

	// Create skybox texture
	skyboxTexture = environmentRenderer->getCubemapTexture();

	// Load sphere mesh
	sphereMesh = ParseObjFile(sphereObjectPath.c_str(), true, true);
	assert(sphereMesh != nullptr);

	// Create materials
	shinyMaterial = new Material();
	shinyMaterial->ambient = Vector3(0);
	shinyMaterial->diffuse = Vector3(1);
	shinyMaterial->specular = Vector3(1);
	shinyMaterial->shininess = 1;

	// Create shader programs
	shaderPrograms[LIGHT_PROBE] = new ShaderProgram(vertexShaderPath.c_str(), lightProbeFragmentShaderPath.c_str());
	shaderPrograms[MIRROR] = new ShaderProgram(vertexShaderPath.c_str(), mirrorFragmentShaderPath.c_str());
	shaderPrograms[GLASS] = new ShaderProgram(vertexShaderPath.c_str(), glassFragmentShaderPath.c_str());
	shaderPrograms[GLOSSY] = new ShaderProgram(vertexShaderPath.c_str(), glossyFragmentShaderPath.c_str());
	shaderPrograms[SPECULAR_DISCO] = new ShaderProgram(vertexShaderPath.c_str(), specularDiscoFragmentShaderPath.c_str());

	// Create game objects
	sphere = new GameObject();
	sphere->SetShader(shaderPrograms[drawMode]);
	sphere->SetMesh(sphereMesh);
	// Rotate upside down
	sphere->SetRotation(utilsFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 180.0f));
	sphere->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	sphere->SetScale(Vector3(1.0f, 1.0f, 1.0f));
	sphere->SetMaterial(shinyMaterial);

	// Create camera
	mainCamera = new Camera();
	mainCamera->setPosition(Vector3(0.0f, 0.0f, 5.0f));
	// Look at center
	mainCamera->setTarget(Vector3(0.0f, 0.0f, 0.0f));
	// Set clipping planes
	mainCamera->setNearPlane(0.01f);
	mainCamera->setFarPlane(100.0f);

	// Create IBL sampler for lighting
	iblSampler = new IBLSampler(hdriTexture, (int) pow(2, directionalLightPow));
	std::cout << "IBL sampler created" << std::endl;

	// Create mesh renderer
	meshRenderer = new MeshRenderer();
	meshRenderer->SetCamera(mainCamera);
	meshRenderer->SetCubemap(skyboxTexture);
	meshRenderer->SetExposure(environmentRenderer->getExposure());
	meshRenderer->SetSpecularEnabled(specularEnabled);
	meshRenderer->SetLights(iblSampler->getLights());
}
void update()
{
	// Rotate sphere
	if (rotationDirection != 0)
	{
		float angle = 0.01f * rotationDirection;
		rotateCamera(angle, 0.0f);
		// rotate game object too
		Quaternion rotation = sphere->GetRotation();
		Quaternion newRotation = glm::rotate(rotation, angle, Vector3(0.0f, 1.0f, 0.0f));
		sphere->SetRotation(newRotation);
	}
}

void drawObjects()
{
	// Draw environment
	environmentRenderer->render(*mainCamera);

	// Draw game objects
	meshRenderer->Draw(sphere);
	assert(glGetError() == GL_NO_ERROR);
}
void rotateCamera(float yaw, float pitch)
{
	// Move and rotate camera on a sphere centered around 0,0,0 with radius 10
	// Calculate spherical coordinates
	Vector3 position = mainCamera->getPosition();
	float radius = glm::length(position);
	float theta = atan2(position.z, position.x);
	float phi = acos(position.y / radius);

	theta += yaw;
	phi += pitch;

	// Clamp phi
	phi = glm::clamp(phi, 0.1f, glm::pi<float>() - 0.1f);

	// Convert back to cartesian
	position.x = radius * sin(phi) * cos(theta);
	position.y = radius * cos(phi);
	position.z = radius * sin(phi) * sin(theta);

	mainCamera->setPosition(position);
	mainCamera->setTarget(Vector3(0.0f, 0.0f, 0.0f));

	// Update camera UBO
	meshRenderer->UpdateCameraUBO();
}

int main(int argc, char** argv)
{
	CreateWindow();
	return 0;
}
void reshape(GLFWwindow* window, int w, int h)
{
	w = w < 1 ? 1 : w;
	h = h < 1 ? 1 : h;

	WIDTH = w;
	HEIGHT = h;
	mainCamera->setAspectRatio((float)w / (float)h);

	glViewport(0, 0, w, h);
}
void inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GL_TRUE);

		// W to double exposure
		if (key == GLFW_KEY_W)
		{
			float exposure = environmentRenderer->getExposure();
			if (exposure == 0.0f) exposure = 0.01f;
			environmentRenderer->setExposure(exposure * 2.0f);
			meshRenderer->SetExposure(exposure * 2.0f);
			std::cout << "Exposure: " << environmentRenderer->getExposure() << std::endl;
		}
		// S to half exposure
		if (key == GLFW_KEY_S)
		{
			float exposure = environmentRenderer->getExposure();
			environmentRenderer->setExposure(exposure * 0.5f);
			meshRenderer->SetExposure(exposure * 0.5f);
			std::cout << "Exposure: " << environmentRenderer->getExposure() << std::endl;
		}
		// A to rotate left
		if (key == GLFW_KEY_A)
			rotationDirection += -1;
		// D to rotate right
		if (key == GLFW_KEY_D)
			rotationDirection += 1;
		// F to toggle specular lightning effect
		if (key == GLFW_KEY_F)
		{
			specularEnabled = !specularEnabled;
			std::cout << "Specular enabled: " << specularEnabled << std::endl;
			meshRenderer->SetSpecularEnabled(specularEnabled);
		}
		// R to multiply direct light count by 2
		if (key == GLFW_KEY_R)
		{
			if (directionalLightPow == 7) return;
			directionalLightPow++;
			iblSampler->changeNumLights((int) pow(2, directionalLightPow));
			std::cout << "Direct light count: " << (int) pow(2, directionalLightPow) << std::endl;
			meshRenderer->SetLights(iblSampler->getLights());
		}
		// E to multiply direct light count by 0.5
		if (key == GLFW_KEY_E)
		{
			if (directionalLightPow == 0) return;
			directionalLightPow--;
			iblSampler->changeNumLights((int) pow(2, directionalLightPow));
			std::cout << "Direct light count: " << (int) pow(2, directionalLightPow) << std::endl;
			meshRenderer->SetLights(iblSampler->getLights());
		}

		// 1 -> LIGHT_PROBE
		// 2 -> MIRROR
		// 3 -> GLASS
		// 4 -> GLOSSY
		// 5 -> SPECULAR_DISCO
		int mode = key - GLFW_KEY_0;
		if (mode >= 1 && mode <= 5)
		{
			if (drawMode == (DrawMode)mode) return;

			drawMode = (DrawMode)mode;
			sphere->SetShader(shaderPrograms[drawMode]);
			std::cout << "Draw mode: ";
			switch (drawMode)
			{
				case LIGHT_PROBE: std::cout << "LIGHT_PROBE"; break;
				case MIRROR: std::cout << "MIRROR"; break;
				case GLASS: std::cout << "GLASS"; break;
				case GLOSSY: std::cout << "GLOSSY"; break;
				case SPECULAR_DISCO: std::cout << "SPECULAR_DISCO";break;
			}
			std::cout << std::endl;
		}
	}
	else if(action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_A)
			rotationDirection += 1;
		if (key == GLFW_KEY_D)
			rotationDirection += -1;
	}
	
}
void mousePosInputCallback(GLFWwindow* window, double xpos, double ypos)
{
	// Move camera with mouse
	static bool doRotateCamera = false;
	static double lastX = 0;
	static double lastY = 0;
	if (doRotateCamera)
	{
		double xoffset = lastX - xpos;
		double yoffset = ypos - lastY;
	
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.003f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		rotateCamera(xoffset, yoffset);

	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		doRotateCamera = true;
		lastX = xpos;
		lastY = ypos;		
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		doRotateCamera = false;
	}
}
void mainLoop(GLFWwindow* window)
{
	while (!glfwWindowShouldClose(window))
	{
		// Clear the colorbuffer
		// glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Print avg fps
		static double previousSeconds = glfwGetTime();
		static int frameCount;
		double elapsedSeconds = glfwGetTime() - previousSeconds;
		if (elapsedSeconds > 0.25)
		{
			previousSeconds = glfwGetTime();
			double fps = (double)frameCount / elapsedSeconds;
			double msPerFrame = 1000.0 / fps;

			std::ostringstream outs;
			outs.precision(3); // decimal places
			outs << std::fixed
				<< "FPS: " << fps << " Frame Time: " << msPerFrame << "(ms)";
			// Append fps to window title
			glfwSetWindowTitle(window, (windowTitle + " - " + outs.str()).c_str());

			frameCount = 0;
		}
		frameCount++;

		// Update
		update();

		// Draw game objects
		drawObjects();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *msg, const void *data)
{
    std::string source_;
    std::string type_;
    std::string severity_;

    switch (source)
	{
		case GL_DEBUG_SOURCE_API:             source_ = "API";             break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_ = "WINDOW_SYSTEM";   break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: source_ = "SHADER_COMPILER"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     source_ = "THIRD_PARTY";     break;
		case GL_DEBUG_SOURCE_APPLICATION:     source_ = "APPLICATION";     break;
		case GL_DEBUG_SOURCE_OTHER:           source_ = "OTHER";           break;
		default:                              source_ = "<SOURCE>";        break;
	}

	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:               type_ = "ERROR";               break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_ = "DEPRECATED_BEHAVIOR"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_ = "UDEFINED_BEHAVIOR";   break;
		case GL_DEBUG_TYPE_PORTABILITY:         type_ = "PORTABILITY";         break;
		case GL_DEBUG_TYPE_PERFORMANCE:         type_ = "PERFORMANCE";         break;
		case GL_DEBUG_TYPE_OTHER:               type_ = "OTHER";               break;
		case GL_DEBUG_TYPE_MARKER:              type_ = "MARKER";              break;
		default:                                type_ = "<TYPE>";              break;
	}

	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         severity_ = "HIGH";         break;
		case GL_DEBUG_SEVERITY_MEDIUM:       severity_ = "MEDIUM";       break;
		case GL_DEBUG_SEVERITY_LOW:          severity_ = "LOW";          break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: severity_ = "NOTIFICATION"; break;
		default:                             severity_ = "<SEVERITY>";   break;
	}


	std::cout << id << ": " << type_ << " of " << severity_ << " severity, raised from " << source_ << ": " << msg << std::endl;
}
void EnableGLDebugging()
{
	glDebugMessageCallback(GLDebugMessageCallback, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}
void CreateWindow()
{
	if(!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "CENG469 HW2 OPENGL", NULL, NULL);

	if(!window)
	{
		glfwTerminate();
		std::cout << "Failed to create window" << std::endl;
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	std::cout << "OpenGL version supported by this platform " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLFW version: " << glfwGetVersionString() << std::endl;
	std::cout << "GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;

	// Initialize GLEW to setup the OpenGL Function pointers
	int initState = glewInit();
	if (initState != GLEW_OK)
	{
		std::cerr << "Error initializing GLEW: " << glewGetErrorString(initState) << ". Enum:" << initState << std::endl;
		exit(EXIT_FAILURE);
	}
	
	glfwSetWindowTitle(window, windowTitle.c_str());
	glfwSetKeyCallback(window, inputCallback);
	glfwSetCursorPosCallback(window, mousePosInputCallback);
	glfwSetWindowSizeCallback(window, reshape);
	// Set timeout for polling events
	glfwWaitEventsTimeout(0.01); // Every 10ms (not fps-limited)
	
	EnableGLDebugging();

	init();

	reshape(window, WIDTH, HEIGHT); // need to call this once ourselves
	mainLoop(window); // this does not return unless the window is closed

	glfwDestroyWindow(window);
	glfwTerminate();
}
