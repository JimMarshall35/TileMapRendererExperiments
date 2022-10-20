// frag

#version 440 core

uniform sampler2DArray atlasSampler;
in vec3 TexCoords;
out vec4 FragColor;

void main()
{
    FragColor = texture(atlasSampler, TexCoords);
}