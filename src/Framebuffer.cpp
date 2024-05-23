#include "Framebuffer.h"

Framebuffer* Framebuffer::CreateCubemapFramebuffer(Texture* inputTexture)
{
    return new Framebuffer(inputTexture, true);
}

Framebuffer* Framebuffer::CreateFramebuffer(int width, int height, Texture* inputTexture)
{
    return new Framebuffer(width, height, inputTexture);
}

Framebuffer::Framebuffer(Texture* inputTexture, bool isCubemap)
{
    if (inputTexture == nullptr) {
        return;
    }
    this->inputTexture = inputTexture;
    this->width = inputTexture->getWidth();
    this->height = inputTexture->getHeight();
    
    glGenFramebuffers(1, &id);
    bind();

    if (isCubemap) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, inputTexture->getID(), 0);
    }
    else {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, inputTexture->getID(), 0);
    }

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    assert(glGetError() == GL_NO_ERROR);
    
    unbind();
}

Framebuffer::Framebuffer(int width, int height, Texture* inputTexture)
{
    if (inputTexture == nullptr) {
        return;
    }
    this->inputTexture = inputTexture;
    this->width = width;
    this->height = height;
    
    glGenFramebuffers(1, &id);
    bind();
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, inputTexture->getID(), 0);
    
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    assert(glGetError() == GL_NO_ERROR);
    
    unbind();
}


Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &id);
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void Framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}