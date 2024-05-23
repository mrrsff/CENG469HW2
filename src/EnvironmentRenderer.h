#ifndef ENVIROMENT_RENDERER_H
#define ENVIROMENT_RENDERER_H

#define DEFAULT_ENVIRONMENT_RENDERER_WIDTH 1024
#define DEFAULT_ENVIRONMENT_RENDERER_HEIGHT 1024

#include "typedefs.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include <vector>

#include "Texture.h"
#include "Framebuffer.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Camera.h"

class EnvironmentRenderer {
private:
    ShaderProgram* equirectengularToCubemapShader;
    ShaderProgram* skyboxShader;
    Framebuffer* cubemapCreationFramebuffer;
    float exposure = 0.18;
    Mesh* cube;
    Texture* cubemapTexture;
    GLuint sampler;

    Framebuffer* outputFramebuffer;

    void CreateCubemap();

public:
    EnvironmentRenderer(Framebuffer* cubemapCreationFramebuffer, Mesh* cube);
    ~EnvironmentRenderer();
    
    void bind();
    void unbind();

    void setExposure(float exposure);
    float getExposure() const { return exposure; }

    void render(Camera& cam);

    Texture* getCubemapTexture();
};

#endif