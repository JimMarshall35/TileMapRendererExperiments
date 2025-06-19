#version 330 core

#define NUM_PRIMITIVES_PER_INSTANCE 1

layout (location = 0) in vec3 pos;
out int TileId;

uniform ivec2 mapSize;
uniform mat4 projection;
uniform mat4 model;


uniform sampler1D tileIdSampler;


void main()
{
    int i = gl_InstanceID / NUM_PRIMITIVES_PER_INSTANCE;
    float y = float(i / mapSize.y); //float(i & 15);
    float x = float(i % mapSize.y); //float((i >> 4) & 15);
    gl_Position = projection * model * vec4(pos.x + x, pos.y + y, 0, 1);

    vec4 tileId = texelFetch(uvsSampler, i, 0);
    // use other channels for something else - animation?
    TileId = int(tileId[0]); 
}