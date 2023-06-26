// frag

#version 440 core
uniform float opacity;
uniform sampler2DArray atlasSampler;
in vec3 TexCoords;
out vec4 FragColor;

void main()
{
    FragColor = vec4(texture(atlasSampler, TexCoords).rgb,texture(atlasSampler, TexCoords).a * opacity );
}