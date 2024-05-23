// Vertex Shader for skybox with cubemap, glsl

#version 330 core

layout(location = 0) in vec3 inVertex;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 eyePos;

out vec3 texCoord;

void main()
{
    // calculate model matrix from eyePos
    mat4 modeling = mat4(1.0);
    modeling[3] = vec4(eyePos, 1.0);
    
    texCoord = inVertex;
    gl_Position = projection * view * modeling * vec4(inVertex, 1.0);
}

// Path: /shaders/skybox.vert