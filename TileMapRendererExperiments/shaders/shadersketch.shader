// vert
/*
cpp setup:
create a big index buffer
*/
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv;
out vec3 TexCoord;

uniform mat4 vpMatrix;
uniform mat4 modelMatrix;
uniform ivec2 tileTextureOffset;
uniform sampler2d masterTileTexture;

#define TILE_NUM_VERTS 4
#define NUM_TILE_INDICES 6
void main()
{
	vec4 vertices[TILE_NUM_VERTS] = vec4[TILE_NUM_VERTS](
		vec4(0.5f,  0.5f,   1.0f, 1.0f),
		vec4(0.5f, -0.5f,   1.0f, 0.0f),
		vec4(-0.5f, -0.5f,  0.0f, 0.0f),
		vec4(-0.5f,  0.5f,   0.0f, 1.0f)
	);
	int indices[NUM_TILE_INDICES] = int[NUM_TILE_INDICES](
	    0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
	);
	int index = indices[int(gl_VertexID % NUM_TILE_INDICES)];
	vec4 baseVertex = vertices[index];

	int i = gl_VertexID / NUM_TILE_INDICES;

	ivec2 tilexy = ivec2(int(i / mapSize.y), int(i % mapSize.y));

    baseVertex.xy += vec2(tilexy);

    TexCoords = vec3(
    	baseVertex.zw,
    	float(texelFetch(masterTileTexture, tilexy + tileTextureOffset));
    	
    gl_Position = vpMatrix * modelMatrix * vec4(baseVertex, 0.0, 1.0);

}


// frag