// ShaderProgram.cpp
#include "ShaderProgram.h"

ShaderProgram* ShaderProgram::getDefaultShader() {
    const std::string DEFAULT_VERTEX_SHADER = "shaders/lit.vert";
    const std::string DEFAULT_FRAGMENT_SHADER = "shaders/lit.frag";
    static ShaderProgram* defaultShader = new ShaderProgram(DEFAULT_VERTEX_SHADER.c_str(), DEFAULT_FRAGMENT_SHADER.c_str());
    return defaultShader;
}

ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    // vShaderFile.open(vertexPath);
    // fShaderFile.open(fragmentPath);
    // std::stringstream vShaderStream, fShaderStream;
    // vShaderStream << vShaderFile.rdbuf();
    // fShaderStream << fShaderFile.rdbuf();
    // vShaderFile.close();
    // fShaderFile.close();
    // vertexCode = vShaderStream.str();
    // fragmentCode = fShaderStream.str();	

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();	
    } catch(std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << vertexPath << " " << fragmentPath << std::endl;
        std::cout << e.what() << std::endl;
        assert(false);
    }
    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // Clear memory
    vertexCode.clear();
    fragmentCode.clear();

    assert(glGetError() == GL_NO_ERROR);
}

void ShaderProgram::AddGeometryShader(const char* geometryPath) {
    std::string geometryCode;
    std::ifstream gShaderFile;
    gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        gShaderFile.open(geometryPath);
        std::stringstream gShaderStream;
        gShaderStream << gShaderFile.rdbuf();
        gShaderFile.close();
        geometryCode = gShaderStream.str();
    } catch(std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        assert(false);
    }
    const char* gShaderCode = geometryCode.c_str();
    unsigned int geometry;
    int success;
    char infoLog[512];
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    checkCompileErrors(geometry, "GEOMETRY");
    glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    glDeleteShader(geometry);
}

void ShaderProgram::use() {
    glUseProgram(ID);
}
void ShaderProgram::unuse() { 
    glUseProgram(0);
}
void ShaderProgram::setBool(const std::string &name, bool value) const { 
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform1i(loc, (int)value); 
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setInt(const std::string &name, int value) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform1i(loc, value); 
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setFloat(const std::string &name, float value) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform1f(loc, value); 
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setVec2(const std::string &name, const Vector2 &value) const { 
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform2fv(loc, 1, glm::value_ptr(value)); 
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setVec3(const std::string &name, const Vector3 &value) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(value)); 
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setVec4(const std::string &name, const Vector4 &value) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(value)); 
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setMat3(const std::string &name, const Matrix3 &value) const { 
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setMat4(const std::string &name, const Matrix4 &value) const { 
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setSampler2D(const std::string &name, int value) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform1i(loc, value);
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setSamplerCube(const std::string &name, int value) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform1i(loc, value);
    assert(glGetError() == GL_NO_ERROR);
}

bool ShaderProgram::getBool(const std::string &name) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    int value;
    glGetUniformiv(ID, loc, &value);
    return (bool)value;
}
int ShaderProgram::getInt(const std::string &name) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    int value;
    glGetUniformiv(ID, loc, &value);
    return value;
}
float ShaderProgram::getFloat(const std::string &name) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    float value;
    glGetUniformfv(ID, loc, &value);
    return value;
}
Vector2 ShaderProgram::getVec2(const std::string &name) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    Vector2 value;
    glGetUniformfv(ID, loc, glm::value_ptr(value));
    return value;
}
Vector3 ShaderProgram::getVec3(const std::string &name) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    Vector3 value;
    glGetUniformfv(ID, loc, glm::value_ptr(value));
    return value;
}
Vector4 ShaderProgram::getVec4(const std::string &name) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    Vector4 value;
    glGetUniformfv(ID, loc, glm::value_ptr(value));
    return value;
}
Matrix3 ShaderProgram::getMat3(const std::string &name) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    Matrix3 value;
    glGetUniformfv(ID, loc, glm::value_ptr(value));
    return value;
}
Matrix4 ShaderProgram::getMat4(const std::string &name) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    Matrix4 value;
    glGetUniformfv(ID, loc, glm::value_ptr(value));
    return value;
}

void ShaderProgram::checkCompileErrors(GLuint shader, std::string type) {
    int success;
    char infoLog[1024];
    if(type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    assert(glGetError() == GL_NO_ERROR);
}

int ShaderProgram::getUniformLocation(const std::string &name) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cout << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        assert(false);
    }
    return loc;
}