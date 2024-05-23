#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Texture.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include <vector>

class Framebuffer {
private:
    GLuint id;
    int width;
    int height;
    Texture* inputTexture; // Input color texture
    Framebuffer(Texture* inputTexture, bool isCubemap);
    Framebuffer(int width, int height, Texture* inputTexture);

public:
    static Framebuffer* CreateCubemapFramebuffer(Texture* inputTexture);
    static Framebuffer* CreateFramebuffer(int width, int height, Texture* inputTexture);
    ~Framebuffer();

    void bind();
    void unbind();   
    bool hasColorTexture() const { return inputTexture != nullptr; }

    Texture* getColorTexture() const { return inputTexture; }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    GLuint getID() const { return id; }
};

#endif
// Path: src/Framebuffer.cpp