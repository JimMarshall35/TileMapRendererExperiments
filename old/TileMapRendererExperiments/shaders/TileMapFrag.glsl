#version 330 core

uniform sampler2D atlasSampler;
in vec2 texCoord;
out vec4 FragColor;

void main()
{
    FragColor = texture(atlasSampler, texCoord);
}