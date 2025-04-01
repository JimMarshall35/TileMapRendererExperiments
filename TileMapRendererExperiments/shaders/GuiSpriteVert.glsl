#version 460 core
layout (location = 0) in vec2 pos; // x,y == position, z,w == uv
layout (location = 1) in vec2 uv; // x,y == position, z,w == uv
out vec2 TexCoords;

uniform mat4 projection;
uniform vec2 pixelPos;
uniform vec2 pixelDims;

void main(){
	gl_Position = projection * vec4(pixelDims * (pixelPos + pos.xy), 0.0, 1.0);

}