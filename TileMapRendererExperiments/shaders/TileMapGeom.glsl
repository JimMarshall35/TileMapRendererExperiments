#version 460 core

#define MAX_NUM_TILES 1060
uniform mat4 projection;
uniform mat4 model;
uniform sampler1D uvsSampler;

layout(binding = 0) uniform TilesDataBlock
{
    vec4 uvCoordsTopLeftBottomRight[MAX_NUM_TILES];
};

in VS_OUT {
    uint tileId;
} gs_in[];

out vec2 texCoord;

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;


void main() {
    uint tileId = gs_in[0].tileId;
    //const vec4 uvs = uvCoordsTopLeftBottomRight[tileId];
    vec4 uvs = texelFetch(uvsSampler, int(tileId), 0);
    float left = uvs[0];
    float top = uvs[1];
    float right =  uvs[2];
    float bottom = uvs[3];


    gl_Position = projection * model * gl_in[0].gl_Position; // top left
    texCoord = vec2(left,top);
    EmitVertex();

    gl_Position = projection * model * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0)); // top right
    texCoord = vec2(right,top);
    EmitVertex();

    gl_Position = projection * model * (gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0)); // bottom left
    texCoord = vec2(left,bottom);

    EmitVertex();

    gl_Position = projection * model * (gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0)); // bottom right
    texCoord = vec2(right,bottom);


    EmitVertex();

    EndPrimitive();
}  