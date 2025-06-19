#version 460 core
in vec2 TexCoords;
out vec4 FragColour;

layout(binding=0) uniform sampler2D myTexture;

void main(){
	vec4 texColor = texture(myTexture, TexCoords);

    FragColour = texColor;
	
}