#ifndef TEXTURE_H
#define TEXTURE_H

#include "typedefs.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include <array>
#include <vector>
#include <string>

#include "stb_image.h"

class Texture {
public:
    static Texture* CreateCubemap(GLuint width, GLuint height);

    Texture();
    Texture(const std::string& path, bool isHDR);
    ~Texture();

    void bind();
    void unbind();
    void setTextureUnit(GLuint unit);
    void setWrap(GLenum wrap);

    Vector3 getPixel(GLuint x, GLuint y);
    Vector3 getMaximumPixel();

    GLuint getID() const { return id; }
    GLuint getWidth() const { return width; }
    GLuint getHeight() const { return height; }
    GLuint getChannels() const { return channels; }
    GLuint getFormat() const { return format; }
    GLuint getTarget() const { return target; }
    GLuint getTextureUnit() const { return current_unit; }

private:
    GLenum target; // GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP
    GLenum format; // GL_RED, GL_RG, GL_RGB, GL_RGBA
    GLuint id; // texture id
    GLuint width; // texture width
    GLuint height; // texture height
    GLuint channels; // number of channels
    GLuint current_unit; // current texture unit

    float* hdriData; // HDR data
    unsigned char* data; // LDR data

    void load(const std::string& path);
    void loadHDR(const std::string& path);
};


#endif