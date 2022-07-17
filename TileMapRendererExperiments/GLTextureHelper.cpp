#include "GLTextureHelper.h"
#include <glad/glad.h>


bool OpenGlGPULoadTexture(const unsigned char* data, unsigned int width, unsigned int height, unsigned int* id)
{
	glGenTextures(1, id);
	glBindTexture(GL_TEXTURE_2D, *id);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST is the better filtering option for this game
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // as gives better more "pixelated" (less "smoothed out") textures
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width,height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	return true;
}