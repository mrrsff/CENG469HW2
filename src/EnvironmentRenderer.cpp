#include "EnvironmentRenderer.h"

EnvironmentRenderer::EnvironmentRenderer(Framebuffer* cubemapCreationFramebuffer, Mesh* cube)
    : cubemapCreationFramebuffer(cubemapCreationFramebuffer), cube(cube)
{
    assert(cubemapCreationFramebuffer != nullptr);
    assert(cube != nullptr);

    // Create the cubemap.
    CreateCubemap();

}

EnvironmentRenderer::~EnvironmentRenderer()
{
    delete equirectengularToCubemapShader;
    delete cube;
    delete skyboxShader;
    delete cubemapCreationFramebuffer;
    delete outputFramebuffer;
    delete cubemapTexture;
    glDeleteSamplers(1, &sampler);
}

void EnvironmentRenderer::CreateCubemap()
{
    // Load shader
    equirectengularToCubemapShader = new ShaderProgram("shaders/fullscreen.vert", "shaders/panoramicToCubemap.frag");
    assert(glGetError() == GL_NO_ERROR);

    // Link the shader
    // Create the cubemap texture
    cubemapTexture = Texture::CreateCubemap(cubemapCreationFramebuffer->getWidth(), cubemapCreationFramebuffer->getHeight());

    // Create the cubemap texture from the framebuffer
    // In each iteration, another face of the cube is rendered to the cubemap texture.
    // After all faces are rendered, the cubemap texture is complete.
    for (int i = 0; i < 6; i++) {

        cubemapCreationFramebuffer->bind();
        assert(glGetError() == GL_NO_ERROR);
        int face = i;
        
        glFramebufferTexture2D( GL_FRAMEBUFFER, 
                                GL_COLOR_ATTACHMENT0, 
                                GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 
                                cubemapTexture->getID(), 
                                0);
        assert(glGetError() == GL_NO_ERROR);

        // Bind cube map texture
        cubemapTexture->bind();

        glViewport(0, 0, cubemapCreationFramebuffer->getWidth(), cubemapCreationFramebuffer->getHeight());
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        assert(glGetError() == GL_NO_ERROR);


        equirectengularToCubemapShader->use();
        glActiveTexture(GL_TEXTURE0 + cubemapCreationFramebuffer->getColorTexture()->getTextureUnit());
        glBindTexture(cubemapCreationFramebuffer->getColorTexture()->getTarget(), cubemapCreationFramebuffer->getColorTexture()->getID());
        assert(glGetError() == GL_NO_ERROR);

        equirectengularToCubemapShader->setInt("face", face);
        equirectengularToCubemapShader->setSampler2D("panoramicTexture", cubemapCreationFramebuffer->getColorTexture()->getTextureUnit());
        assert(glGetError() == GL_NO_ERROR);

        cube->Draw();
        assert(glGetError() == GL_NO_ERROR);
    }
    assert(glGetError() == GL_NO_ERROR);

    // Unbind the shader
    equirectengularToCubemapShader->unuse();

    // Create mipmaps for the cubemap texture
    cubemapTexture->bind();
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    assert(glGetError() == GL_NO_ERROR);
    
    // Unbind the cubemap texture
    cubemapTexture->unbind();

    // Unbind the framebuffer
    cubemapCreationFramebuffer->unbind();

    // Create the cubemap sampler
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Set the cubemap texture to the sampler
    glBindSampler(0, sampler);
    glActiveTexture(GL_TEXTURE0 + cubemapTexture->getTextureUnit());
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture->getID());
    assert(glGetError() == GL_NO_ERROR);

    // Unbind everything
    glBindSampler(0, sampler);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


    // Enable seamless cubemap sampling
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Create the output framebuffer
    outputFramebuffer = Framebuffer::CreateCubemapFramebuffer(cubemapTexture);
    assert(glGetError() == GL_NO_ERROR);

    skyboxShader = new ShaderProgram("shaders/skybox.vert", "shaders/skybox.frag");
    assert(glGetError() == GL_NO_ERROR);
    skyboxShader->use();

    skyboxShader->setSamplerCube("skybox", cubemapTexture->getTextureUnit());
    skyboxShader->unuse();
    assert(glGetError() == GL_NO_ERROR);
}

void EnvironmentRenderer::bind()
{
    // Bind the cubemap texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture->getID());
    glBindSampler(0, sampler);
}

void EnvironmentRenderer::unbind()
{
    // Unbind the cubemap texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindSampler(0, 0);
}

void EnvironmentRenderer::render(Camera& cam)
{
    // Disable culling, depth testing and face culling (if enabled)
    bool cullingEnabled = glIsEnabled(GL_CULL_FACE);
    bool depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    bool faceCullingEnabled = glIsEnabled(GL_FRAMEBUFFER_SRGB);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FRAMEBUFFER_SRGB);

    // Render the skybox
    skyboxShader->use();

    // Set the cubemap texture
    glActiveTexture(GL_TEXTURE0 + cubemapTexture->getTextureUnit());
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture->getID());

    // Set the view, projection and model matrices
    skyboxShader->setMat4("projection", *cam.getProjectionMatrix());
    skyboxShader->setMat4("view", *cam.getViewMatrix());
    skyboxShader->setVec3("eyePos", cam.getPosition());
    skyboxShader->setFloat("exposure", exposure);

    // Draw the cube
    cube->Draw();

    // Unbind the cubemap texture and the sampler
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindSampler(cubemapTexture->getTextureUnit(), 0);

    // Unbind the shader
    skyboxShader->unuse();
    
    // Re-enable culling, depth testing and face culling (if enabled)
    if (cullingEnabled) {
        glEnable(GL_CULL_FACE);
    }
    if (depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
    }
    if (faceCullingEnabled) {
        glEnable(GL_FRAMEBUFFER_SRGB);
    }
}

Texture* EnvironmentRenderer::getCubemapTexture()
{
    return cubemapTexture;
}

void EnvironmentRenderer::setExposure(float exposure)
{
    this->exposure = exposure;
}