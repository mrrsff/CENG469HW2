#version 330 core  

in vec2 TexCoords;

uniform int face;
uniform sampler2D panoramicTexture;

out vec4 fragColor;

const float PI = 3.14159265359;

vec3 uvToDir(vec2 uv, int face) {
    vec3 dir;
    float flippedY = uv.y; // Flip the y coordinate
    float flippedX = -uv.x; // Flip the x coordinate
    if (face == 0) { // Positive x (right)
        dir = vec3(-1.0, flippedY, flippedX);
    } else if (face == 1) { // Negative x (left)
        dir = vec3(1.0, flippedY, -flippedX);
    } else if (face == 2) { // Positive y (up)
        dir = vec3(flippedX, -1.0, flippedY);
    } else if (face == 3) { // Negative y (down)
        dir = vec3(flippedX, 1.0, -flippedY);
    } else if (face == 4) { // Positive z (front)
        dir = vec3(flippedX, flippedY, 1.0);
    } else { // Negative z (back)
        dir = vec3(-flippedX, flippedY, -1.0);
    }
    return normalize(dir);
}

vec2 dirToUV(vec3 dir) {
    float u = (atan(dir.z, dir.x) / PI * 0.5) + 0.5;
    float v = (acos(dir.y) / PI);
    return vec2(u, v);
}

vec3 panoramaToCubemap(vec2 uv, int face) {
    vec2 _texCoord = TexCoords*2.0 - 1.0;
    vec3 dir = uvToDir(_texCoord, face);
    vec2 st = dirToUV(dir);
    return texture(panoramicTexture, st).rgb;
}

void main() {
    fragColor = vec4(panoramaToCubemap(TexCoords, face), 1.0);
}