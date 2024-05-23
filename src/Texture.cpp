#include "Texture.h"    

Texture* Texture::CreateCubemap(GLuint width, GLuint height) {
    Texture* texture = new Texture();
    texture->width = width;
    texture->height = height;
    texture->target = GL_TEXTURE_CUBE_MAP;
    texture->format = GL_RGB;

    glGenTextures(1, &texture->id);
    texture->setTextureUnit(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, GL_RGB32F, width, height, 
                     0, GL_RGBA, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}

Texture::Texture() 
    : id(0), width(0), height(0), channels(0) 
{
}

Texture::Texture(const std::string& path, bool isHDR)
    : id(0), width(0), height(0), channels(0)
{
    if (isHDR) {
        loadHDR(path);
    }
    else {
        load(path);
    }
    assert(id != 0);
    assert(glGetError() == GL_NO_ERROR);

    target = GL_TEXTURE_2D;
    current_unit = 0;
    setWrap(GL_REPEAT);
    // Set filtering parameters
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::cout << "Loaded texture: " << path << std::endl;

    unbind();
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::load(const std::string& path) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    
    this->width = width;
    this->height = height;
    this->channels = channels;

    if (data) {
        if (channels == 1) {
            format = GL_RED;
        }
        else if (channels == 2) {
            format = GL_RG;
        }
        else if (channels == 3) {
            format = GL_RGB;
        }
        else if (channels == 4) {
            format = GL_RGBA;
        }
        else {
            std::cout << "Unsupported number of channels: " << channels << std::endl;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }

    // Copy data to this->data
    this->data = new unsigned char[width * height * channels];
    for (int i = 0; i < width * height * channels; i++) {
        this->data[i] = data[i];
    }
    stbi_image_free(data);
}

void Texture::loadHDR(const std::string& path) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    int width, height, channels;
    float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);

    this->width = width;
    this->height = height;
    this->channels = channels;

    if (data) {
        if (channels == 1) {
            format = GL_RED;
        }
        else if (channels == 2) {
            format = GL_RG;
        }
        else if (channels == 3) {
            format = GL_RGB;
        }
        else if (channels == 4) {
            format = GL_RGBA;
        }
        else {
            std::cout << "Unsupported number of channels: " << channels << std::endl;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, format, GL_FLOAT, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        assert(glGetError() == GL_NO_ERROR);
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }

    // Copy data to hdriData
    hdriData = new float[width * height * channels];
    for (int i = 0; i < width * height * channels; i++) {
        hdriData[i] = data[i];
    }
    stbi_image_free(data);
}

void Texture::bind() {
    glBindTexture(target, id);
}

void Texture::unbind() {
    glBindTexture(target, 0);
}

void Texture::setTextureUnit(GLuint unit) {
    current_unit = unit;
    glActiveTexture(GL_TEXTURE0 + current_unit);
    glBindTexture(target, id);
}

void Texture::setWrap(GLenum wrap) {
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);
}

Vector3 Texture::getPixel(GLuint x, GLuint y) {
    if (hdriData) {
        GLuint index = (y * width + x) * channels;
        return Vector3(hdriData[index], hdriData[index + 1], hdriData[index + 2]);
    }
    else if (data) {
        GLuint index = (y * width + x) * channels;
        return Vector3(data[index], data[index + 1], data[index + 2]);
    }
    else {
        std::cerr << "Texture is not HDR" << std::endl;
        return Vector3(0.0f);
    }
}

Vector3 Texture::getMaximumPixel() {
    Vector3 maxPixel(0.0f);
    if (hdriData) {
        for (int i = 0; i < width * height * channels; i += channels) {
            Vector3 pixel(hdriData[i], hdriData[i + 1], hdriData[i + 2]);
            maxPixel = glm::max(maxPixel, pixel);
        }
    }
    else if (data) {
        for (int i = 0; i < width * height * channels; i += channels) {
            Vector3 pixel(data[i], data[i + 1], data[i + 2]);
            maxPixel = glm::max(maxPixel, pixel);
        }
    }
    else {
        std::cerr << "Texture is not HDR" << std::endl;
    }
    return maxPixel;
}
