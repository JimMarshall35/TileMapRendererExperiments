#version 460 core
layout (location = 0) in uint point;

uniform vec2 point1world;
uniform vec2 point2world;

uniform mat4 projection;

void main(){
	gl_Position = projection  * vec4((point == 0) ? point1world : point2world,0.0,1.0);
}