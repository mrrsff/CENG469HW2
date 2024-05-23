// Fragment Shader for skybox with cubemap, glsl\

#version 330 core

uniform samplerCube skybox;
uniform float exposure;

in vec3 texCoord;
out vec4 fragColor;
/*
    The tone mapping is done per fragment as follows:
        – exp() of the HDR RGB texture color is first calculated after getting it from the texture.
        – The scene’s mean HDR RGB value is found by calling textureLod() function
            with a high number as the argument so that a 1x1 mipmap (mean HDR RGB)
            of the image is looked-up by this call automatically in shader (of course, we take
            exp() of this looked-up value, too). We should generate mipmaps of the rendered scene’s texture by calling glGenerateMipmap(GL TEXTURE 2D) and
            glTextureBarrier() before rendering the quad.
        – A scaled luminance is calculated by dividing the luminance of the exp()-applied
            HDR RGB texture value by the mean luminance and then multiplying with
            the exposure uniform. Luminances are found by interpolating the RGB’s by
            (0.2126, 0.7152, 0.0722).
        – The rest of the tone map code lines are the same as the code in the slides.
*/
float luminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}
vec3 tonemap(vec3 hdrColor, float exposure)
{
    vec3 meanColor = textureLod(skybox, vec3(0.5), 100.0).rgb;
	// Calculate the luminance
    float luminanceHdr = luminance(hdrColor);
    float luminanceMean = luminance(meanColor);
    float scaledLuminance = (luminanceHdr / luminanceMean) * exposure;

    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * scaledLuminance);

    // Gamma correction
    mapped = pow(mapped, vec3(1.0/2.2));

    return mapped;
}
void main()
{    
    // Sample the hdr environment map
    vec3 hdrColor = texture(skybox, texCoord).rgb;
    vec3 mapped = tonemap(hdrColor, exposure);

    fragColor = vec4(mapped, 1.0);

    // // Exposure tone mapping
    // vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

    // // Gamma correction
    // mapped = pow(mapped, vec3(1.0/2.2));

    // fragColor = vec4(mapped, 1.0);

}

// Path: shaders/skybox.vert