#version 440 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv;
out vec3 TexCoords;

uniform mat4 vpMatrix;
uniform mat4 modelMatrix;
uniform ivec2 chunkOffset;
uniform ivec2 chunkSize;
uniform usampler2D masterTileTexture;

#define TILE_NUM_VERTS 4
#define NUM_TILE_INDICES 6
void main()
{

	// vertices and indices that make up two triangles (a quad)
	// ie one tile in the map
	vec4 vertices[TILE_NUM_VERTS] = vec4[TILE_NUM_VERTS](
		vec4(0.5f,  0.5f,   1.0f, 1.0f),
		vec4(0.5f, -0.5f,   1.0f, 0.0f),
		vec4(-0.5f, -0.5f,  0.0f, 0.0f),
		vec4(-0.5f,  0.5f,  0.0f, 1.0f)
	);
	int indices[NUM_TILE_INDICES] = int[NUM_TILE_INDICES](
	    0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
	);

	// cycle through indicies
	int index = indices[int(gl_VertexID % NUM_TILE_INDICES)];

	// get base vertex
	vec4 baseVertex = vertices[index];

	// which tile in the map is being drawn?
	int whichTile = gl_VertexID / NUM_TILE_INDICES;

	// transfrom into x y coords of tile in the chunk
	ivec2 tilexy = ivec2(int(whichTile / chunkSize.y), int(whichTile % chunkSize.y));

	// translate base vertex by tilexy
    baseVertex.xy += vec2(tilexy);

    // set the z coord of the tex coords passed based on what tile is here
    // in the master tile map.
    TexCoords = vec3(
    	baseVertex.zw,
    	float(texelFetch(masterTileTexture, tilexy + chunkOffset, 0).r)); 

    gl_Position = vpMatrix * modelMatrix * vec4(baseVertex.xy, 0.0, 1.0);

}