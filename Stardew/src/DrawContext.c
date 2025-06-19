#include "DrawContext.h"
#include <string.h>
#include <glad/glad.h>
#include <cglm/cglm.h>
#include "ObjectPool.h"
#include "Widget.h"
#include "DynArray.h"
#include "AssertLib.h"
#include "PlatformDefs.h"

const char* uiVert =
#if GAME_GL_API_TYPE == GAME_GL_API_TYPE_CORE
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec2 aUv;\n"
"layout (location = 2) in vec4 aColour;\n"
"out vec2 UV;\n"
"out vec4 Colour;\n"
"uniform mat4 screenToClipMatrix;\n"
"void main()\n"
"{\n"
	"gl_Position = screenToClipMatrix * vec4(aPos, 0.0, 1.0);\n"
	"UV = aUv;\n"
	"Colour = aColour;"
"}\n"
#elif GAME_GL_API_TYPE == GAME_GL_API_TYPE_ES
"#version 300 es\n"
"in vec2 aPos;\n"
"in vec2 aUv;\n"
"in vec4 aColour;\n"
"out vec2 UV;\n"
"out vec4 Colour;\n"
"uniform mat4 screenToClipMatrix;\n"
"void main()\n"
"{\n"
	"gl_Position = screenToClipMatrix * vec4(aPos, 0.0, 1.0);\n"
	"UV = aUv;\n"
	"Colour = aColour;"
"}\n"
#endif
;

const char* uiFrag =
#if GAME_GL_API_TYPE == GAME_GL_API_TYPE_CORE
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec4 Colour;\n"
"in vec2 UV;\n"

"uniform sampler2D ourTexture;\n"

"void main()\n"
"{\n"
	"FragColor = texture(ourTexture, UV) * Colour;\n"
"}\n"
#elif GAME_GL_API_TYPE == GAME_GL_API_TYPE_ES
"#version 300 es\n"
"precision mediump float;\n"
"out vec4 FragColor;\n"
"in vec4 Colour;\n"
"in vec2 UV;\n"

"uniform sampler2D ourTexture;\n"

"void main()\n"
"{\n"
	"FragColor = texture(ourTexture, UV) * Colour;\n"
"}\n"
#endif
;

//
//const char* tilemapVert = 
//"#version 330 core\n"


struct UIVertexBuffer
{
	GLuint vao;
	GLuint vbo;
	int capacity;
};

enum ShaderType
{
	ST_Vertex,
	ST_Fragment,
	ST_Program,
	ST_NUM
};

static const char* gShaderTypeNameLUT[ST_NUM] =
{
	"Vertex",
	"Fragment",
	"Program"
};

struct Shader
{
	GLuint program;
	GLuint frag;
	GLuint vert;
};

struct Shader gUIShader = {0,0,0};

mat4 gScreenspaceOrtho;

OBJECT_POOL(struct UIVertexBuffer) gVertexBuffersPool = NULL;


static bool OpenGlGPULoadTexture(const unsigned char* data, unsigned int width, unsigned int height, unsigned int* id)
{
	glGenTextures(1, id);
	glBindTexture(GL_TEXTURE_2D, *id);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST is the better filtering option for this game
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // as gives better more "pixelated" (less "smoothed out") textures
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	return true;
}

static void TestShaderStatus(GLuint shader, enum ShaderType type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != ST_Program)
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n -- --------------------------------------------------- -- \n", 
				gShaderTypeNameLUT[type],
				infoLog);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			printf("ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n -- --------------------------------------------------- -- \n",
				gShaderTypeNameLUT[type],
				infoLog);
		}
	}
}

static void UIVertexBufferData(HUIVertexBuffer hBuf, void* src, size_t size)
{
	struct UIVertexBuffer* pBuf = &gVertexBuffersPool[hBuf];

	glBindBuffer(GL_ARRAY_BUFFER, pBuf->vbo);
	if (size > pBuf->capacity)
	{
		glBufferData(GL_ARRAY_BUFFER, size, src, GL_DYNAMIC_DRAW);
		pBuf->capacity = size;
	}
	else
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, src);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void CreateUIShader()
{
	int success = GL_FALSE;

	gUIShader.vert = glCreateShader(GL_VERTEX_SHADER);
	char* v = uiVert;
	char* f = uiFrag;


	glShaderSource(gUIShader.vert, 1, (const char**)&v, 0);
	glCompileShader(gUIShader.vert);
	TestShaderStatus(gUIShader.vert, ST_Vertex);

	gUIShader.frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gUIShader.frag, 1, (const char**)&f, 0);
	glCompileShader(gUIShader.frag);
	TestShaderStatus(gUIShader.frag, ST_Fragment);

	gUIShader.program = glCreateProgram();
	glAttachShader(gUIShader.program, gUIShader.vert);
	glAttachShader(gUIShader.program, gUIShader.frag);
	glLinkProgram(gUIShader.program);

	TestShaderStatus(gUIShader.program, ST_Program);

}

static void CreateShaders()
{
	CreateUIShader();
};

static HUIVertexBuffer NewUIVertexBuffer(int size)
{
	HUIVertexBuffer buf = -1;
	GetObjectPoolIndex(gVertexBuffersPool, &buf);
	struct UIVertexBuffer* pBuf = &gVertexBuffersPool[buf];
	pBuf->capacity = 0;
	glGenVertexArrays(1, &pBuf->vao);
	glBindVertexArray(pBuf->vao);

	glGenBuffers(1, &pBuf->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, pBuf->vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct WidgetVertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct WidgetVertex), sizeof(float) * 2);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(struct WidgetVertex), sizeof(float) * 4);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return buf;
}

static void DrawUIVertexBuffer(HUIVertexBuffer hBuf, size_t vertexCount)
{
	const struct UIVertexBuffer* vertexBuffer = &gVertexBuffersPool[hBuf];

	glUseProgram(gUIShader.program);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->vbo);
	glBindVertexArray(vertexBuffer->vao);

	
	unsigned int projectionViewUniform = glGetUniformLocation(gUIShader.program, "screenToClipMatrix");
	glUniformMatrix4fv(projectionViewUniform, 1, false, &gScreenspaceOrtho[0][0]);


	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

static void DestroyUIVertexBuffer(HUIVertexBuffer hBuf)
{
	const struct UIVertexBuffer* vertexBuffer = &gVertexBuffersPool[hBuf];
	glDeleteBuffers(1, &vertexBuffer->vbo);
	glDeleteVertexArrays(1, &vertexBuffer->vao);
}

static hTexture UploadTexture(void* src, int channels, int pxWidth, int pxHeight)
{
	EASSERT(channels == 4);
	hTexture txture = 0;
	OpenGlGPULoadTexture(src, pxWidth, pxHeight, &txture);
	return txture;
}

static void SetCurrentAtlas(hTexture atlas)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atlas);
}

static void DestroyTexture(hTexture tex)
{
	glDeleteTextures(1, &tex);
}

DrawContext Dr_InitDrawContext()
{
	DrawContext d;
	memset(&d, 0, sizeof(DrawContext));
	d.DestroyVertexBuffer = &DestroyUIVertexBuffer;
	d.DrawUIVertexBuffer = &DrawUIVertexBuffer;
	d.NewUIVertexBuffer = &NewUIVertexBuffer;
	d.UIVertexBufferData = &UIVertexBufferData;
	d.SetCurrentAtlas = &SetCurrentAtlas;
	d.UploadTexture = &UploadTexture;
	gVertexBuffersPool = NEW_OBJECT_POOL(struct UIVertexBuffer, 256);
	glm_mat4_identity(gScreenspaceOrtho);
	CreateShaders();
	return d;
}

void Dr_OnScreenDimsChange(DrawContext* pCtx, int newW, int newH)
{
	pCtx->screenWidth = newW;
	pCtx->screenHeight = newH;
	glm_ortho(0.0f, newW, newH, 0.0f, -1.0f, 1.0f, gScreenspaceOrtho);
}
