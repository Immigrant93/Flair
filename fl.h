/* fl - v1.0.0 - MIT OpenGL v3.3 2D renderer

Do this:
	#define FL_IMPLEMENTATION
before you include this file in *one* C or C++ file to create the implementation.
	
// i.e. it should look like this:
#include ...
#include ...
#include ...
#define FL_IMPLEMENTATION
#include "fl.h"

LICENSE
	MIT 2017 Manidakis Marinos
	See at the end of the file for details
*/

#ifndef __FL_H__
#define __FL_H__

/*
DEPENDENCIES:
--- GLAD OpenGL Extensions loader https://github.com/Dav1dde/glad
--- SDL2 for window and OpenGL context creation	https://www.libsdl.org/index.php
--- stb_image.h Sean T. Barrett stb_image implementation for image loading https://github.com/nothings/stb

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
usage example:																									  //
																												  //
#define FL_IMPLEMENTATION																					  //
#include "fl.h"																							  //
																												  //
int main(int argc, char** argv) {																				  //
	// Create the Window																						  //
	flWindowCreate(640, 480, "Window Title");																	  //
																												  //
	// Initialize Renderer																						  //
	flRendererInit();																							  //
																												  //
	// Create the projection matrix and use for future drawing													  //
	struct flMat4 pr_matrix;																					  //
	flMat4Ortho(0.0f, 16.0f, 9.0f, 0.0f, -1.0f, 1.0f, &pr_matrix);												  //
	flRendererSetProjectionMatrix(&pr_matrix);																	  //
																												  //
	// Load a texture																							  //
	struct flTexture myTexture = flTextureLoadFromFile("res/textures/PNG Grass/slice03_03.png");				  //
																												  //
	struct flVec4 defaultUVCoordinates = (struct flVec4) { 0.0f, 0.0f, 1.0f, 1.0f };							  //
	struct flVec4 destinationRectangle = (struct flVec4) { 4.0f, 2.0f, 3.0f, 3.0f };							  //
	unsigned int colorTint = 0xFFFFFFFF; // 0xAABBGGRR Color Format												  //
																												  //
	// Main Game Loop																							  //
	while (!flWindowClose() && !flIsKeyDown(flKey_esc)) {														  //
																												  //
		// Clear back buffer																					  //
		flRendererClear(0.0f, 0.0f, 0.0f, 1.0f);																  //
																												  //
		// Begin the batching process																			  //
		flRendererBegin();																						  //
																												  //
		// Draw the texture																						  //
		flRendererDraw(&myTexture, &destinationRectangle, &defaultUVCoordinates, colorTint);					  //
																												  //
		// Construct batches and flush renderer																	  //
		flRendererEnd();																						  //
																												  //
		// Poll events and swap the window buffers																  //
		flWindowUpdate();																						  //
	}																											  //
																												  //
	// Clean up code																							  //
	flRendererDestroy();																						  //
																												  //
	// Delete the texture as well																				  //
	flTextureDestroy(&myTexture);																				  //
																												  //
	// Last destroy the window																					  //
	flWindowDestroy();																							  //
																												  //
	return 0;																									  //
}																												  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

#if defined(_WIN32) && !defined(__MINGW32__)
#	ifndef _CRT_SECURE_NO_WARNINGS
#		define _CRT_SECURE_NO_WARNINGS
#	endif
#	ifndef _CRT_NONSTDC_NO_DEPRECATE
#		define _CRT_NONSTDC_NO_DEPRECATE
#	endif
#	ifndef _CRT_NON_CONFORMING_SWPRINTFS
#		define _CRT_NON_CONFORMING_SWPRINTFS
#	endif
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef FL_STATIC
#	define FL_EXTERN static
#else
#	define FL_EXTERN extern
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef FL_GLAD_NO_INCLUDE
#include <glad/glad.h>
#endif

#ifndef FL_SDL2_NO_INCLUDE
#include <SDL2/SDL.h>
#endif

/*
 * Read file contents
 * @return char* buffer with the contents of the file
 * Remember to call flFreeBuffer(char *buffer) after you are done using the buffer
 */
FL_EXTERN char* flReadFileContents(const char *filepath);

/*
 * Free the buffer allocated from flReadFileContents
 */
FL_EXTERN void flFreeBuffer(char *buffer);

/*
 * Generic Implementation of vec2, vec3, vec4 and mat4 struct
 */
/* vec2 */
struct flVec2 {
	float x;
	float y;
};

/* vec3 */
struct flVec3 {
	float x;
	float y;
	float z;
};

/* vec4 */
struct flVec4 {
	float x;
	float y;
	float z;
	float w;
};

/* mat4 */
struct flMat4 {
	float data[16];
};

/*
 * Change the matrix passed to identity matrix
 * @param struct flMat4*
 */
FL_EXTERN void flMat4Identity(struct flMat4 *out);

/*
 * Create an orthographic projection matrix
 * Stores the matrix at {@param out}
 */
FL_EXTERN void flMat4Ortho(float left, float right, float bottom, float top, float zNear, float zFar, struct flMat4 *out);

#ifndef FL_SDL2_NO_INCLUDE
/*
 * Create the window and the OpenGL Context
 */
FL_EXTERN void flWindowCreate(int width, int height, const char *title);
FL_EXTERN int flWindowGetWidth();
FL_EXTERN int flWindowGetHeight();
/*
 * Poll Events, swap the window buffers
 */
FL_EXTERN void flWindowUpdate();
/*
 * Check if window should close
 */
FL_EXTERN bool flWindowClose();
/*
 * Destroy the window
 */
FL_EXTERN void flWindowDestroy();

/*
 * Wrap SDL keys for easier usage
 */
enum flKey {
	flKey_a = SDL_SCANCODE_A,
	flKey_b = SDL_SCANCODE_B,
	flKey_c = SDL_SCANCODE_C,
	flKey_d = SDL_SCANCODE_D,
	flKey_e = SDL_SCANCODE_E,
	flKey_f = SDL_SCANCODE_F,
	flKey_g = SDL_SCANCODE_G,
	flKey_h = SDL_SCANCODE_H,
	flKey_i = SDL_SCANCODE_I,
	flKey_j = SDL_SCANCODE_J,
	flKey_k = SDL_SCANCODE_K,
	flKey_l = SDL_SCANCODE_L,
	flKey_m = SDL_SCANCODE_M,
	flKey_n = SDL_SCANCODE_N,
	flKey_o = SDL_SCANCODE_O,
	flKey_p = SDL_SCANCODE_P,
	flKey_q = SDL_SCANCODE_Q,
	flKey_r = SDL_SCANCODE_R,
	flKey_s = SDL_SCANCODE_S,
	flKey_t = SDL_SCANCODE_T,
	flKey_u = SDL_SCANCODE_U,
	flKey_v = SDL_SCANCODE_V,
	flKey_w = SDL_SCANCODE_W,
	flKey_x = SDL_SCANCODE_X,
	flKey_y = SDL_SCANCODE_Y,
	flKey_z = SDL_SCANCODE_Z,
	flKey_0 = SDL_SCANCODE_0,
	flKey_1 = SDL_SCANCODE_1,
	flKey_2 = SDL_SCANCODE_2,
	flKey_3 = SDL_SCANCODE_3,
	flKey_4 = SDL_SCANCODE_4,
	flKey_5 = SDL_SCANCODE_5,
	flKey_6 = SDL_SCANCODE_6,
	flKey_7 = SDL_SCANCODE_7,
	flKey_8 = SDL_SCANCODE_8,
	flKey_9 = SDL_SCANCODE_9,
	flKey_up = SDL_SCANCODE_UP,
	flKey_down = SDL_SCANCODE_DOWN,
	flKey_left = SDL_SCANCODE_LEFT,
	flKey_right = SDL_SCANCODE_RIGHT,
	flKey_esc = SDL_SCANCODE_ESCAPE
};

/*
 * Check if {@param key} is pressed
 * Event fires multiple times per key press
 */
FL_EXTERN bool flIsKeyDown(enum flKey key);

/*
 * Check if {@param key} is pressed
 * Event fires only once per key press
 */
FL_EXTERN bool flIsKeyPress(enum flKey key);
#endif /* FL_SDL2_NO_INCLUDE */

typedef unsigned int flShader;

/*
 * Create the shader program
 */
FL_EXTERN flShader flShaderCreate();

/*
 * Attach a shader to the program
 * @param const char *src contains the source of the shader, not the path to the file containing the source
 * @param GLenum shaderType is the type of shader { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, ... }
 */
FL_EXTERN void flShaderAttach(flShader program, const char *src, GLenum shaderType);

/*
 * Link the shaders
 */
FL_EXTERN void flShaderLink(flShader program);

/*
 * Destroy the shader program
 */
FL_EXTERN void flShaderDestroy(flShader program);

/*
 * Use the shader program
 */
FL_EXTERN void flShaderBind(flShader program);

/*
 * Bind the attribute location in shader program
 */
FL_EXTERN void flShaderBindAttribLocation(flShader program, int index, const char *name);

/*
 * Get the uniform location
 */
FL_EXTERN int flShaderGetUniformLocation(flShader program, const char *name);

/*
 * Set values for different type of uniforms in shader program
 */
/* float uniform */
FL_EXTERN void flShaderUniform1f(int location, float value);
/* float array uniform */
FL_EXTERN void flShaderUniform1fv(int location, float value[], int size);
/* int uniform */
FL_EXTERN void flShaderUniform1i(int location, int value);
/* int array uniform */
FL_EXTERN void flShaderUniform1iv(int location, int value[], int size);
/* vec2 uniform */
FL_EXTERN void flShaderUniform2f(int location, struct flVec2 *vec);
/* vec3 uniform */
FL_EXTERN void flShaderUniform3f(int location, struct flVec3 *vec);
/* vec4 uniform */
FL_EXTERN void flShaderUniform4f(int location, struct flVec4 *vec);
/* mat4 uniform */
FL_EXTERN void flShaderUniformMatrix4fv(int location, struct flMat4 *matrix);

/*
 * Generic Texture struct
 */
struct flTexture {
	unsigned int id;
	float width;
	float height;
};

/*
 * Load a texture from file
 * @param const char *filepath
 */
FL_EXTERN struct flTexture flTextureLoadFromFile(const char *filepath);

/*
 * Destroy the Texture
 */
FL_EXTERN void flTextureDestroy(struct flTexture *texture);

/*
 * Vertex definition | Matches the default shader program attributes
 * Defines a point
 */
struct flVertex {
	struct flVec2 position;
	struct flVec2 uv;
	unsigned int color;
};

/*
 * Glyph definition | Defines a rectangle
 */
struct flGlyph {
	unsigned int textureId;
	struct flVertex topLeft;
	struct flVertex bottomLeft;
	struct flVertex bottomRight;
	struct flVertex topRight;
};

/*
 * Render batch definition | Batches vertices to draw them together
 * Vertices are sorted by {@param texutreId}
 */
struct flRenderBatch {
	int offset;
	int numVertices;
	unsigned int textureId;
};

/*
 * Initializes the renderer
 * Create the shader program
 * Bind attributes
 * Creates the Vertex Array and the Vertex Buffer
 * Enables vertex attrib arrays and set the vertex pointers
 */
FL_EXTERN void flRendererInit();

/**
 * Set the projection matrix to be used
 * Default is identity matrix
 */
FL_EXTERN void flRendererSetProjectionMatrix(struct flMat4 *pr_matrix);

/*
 * Clear the back buffer with rgba color
 */
FL_EXTERN void flRendererClear(float r, float g, float b, float a);

/*
 * Begin the process of batching vertices
 */
FL_EXTERN void flRendererBegin();

/*
 * Constructs the flGlyphs
 */
FL_EXTERN void flRendererDraw(struct flTexture *texture, struct flVec4 *destRectangle, struct flVec4 *srcRectangle, unsigned int color);

/*
 * Sort all glyphs by texture. Create the render batches and the vertices array
 * Push data to OpenGL and draw everything on the screen
 */
FL_EXTERN void flRendererEnd();

/*
 * Clean up method. Destroys the shader program as well
 */
FL_EXTERN void flRendererDestroy();

#ifdef __cplusplus
}
#endif /* __cplusplsu */

#endif /* __FL_H__ */
/*----------------------------------------------------------------------------*/
/*							 END OF HEADER FILE								  */
/*----------------------------------------------------------------------------*/

#ifdef FL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#ifndef FL_SDL2_NO_INCLUDE
static SDL_Window *__fl_window;
static int __fl_window_width;
static int __fl_window_height;
static bool __fl_window_should_close = false;

#define FL_MAX_KEYS 1024
static bool __fl_keysDown[FL_MAX_KEYS];
static bool __fl_keysDownLast[FL_MAX_KEYS];
static bool __fl_keysPressed[FL_MAX_KEYS];
#endif /* FL_SDL2_NO_INCLUDE */

static GLuint __fl_vao;
static GLuint __fl_vbo;
static flShader __fl_shader;

static const char *__fl_vertex_shader =
"#version 330 core											\n"
"															\n"
"layout(location = 0) in vec2 position;						\n"
"layout(location = 1) in vec2 uv;							\n"
"layout(location = 2) in vec4 color;						\n"
"															\n"
"uniform mat4 pr_matrix = mat4(1.0);						\n"
"															\n"
"out vec2 vsUV;												\n"
"out vec4 vsColor;											\n"
"															\n"
"void main() {												\n"
"	gl_Position = pr_matrix * vec4(position, 0.0, 1.0);		\n"
"	vsUV = uv;												\n"
"	vsColor = color;										\n"
"}															\n";

static const char *__fl_fragment_shader =
"#version 330 core											\n"
"															\n"
"layout(location = 0) out vec4 outColor;					\n"
"uniform sampler2D textureSampler;							\n"
"															\n"
"in vec2 vsUV;												\n"
"in vec4 vsColor;											\n"
"															\n"
"void main() {												\n"
"	outColor = texture(textureSampler, vsUV) * vsColor;		\n"
"}															\n";

#define FL_VERTEX_SIZE sizeof(struct flVertex)
#define FL_GLYPH_SIZE sizeof(struct flGlyph)
#define FL_RENDER_BATCH_SIZE sizeof(struct flRenderBatch)
#define FL_RENDERER_MAX_GLYPHS 1000
#define FL_RENDERER_MAX_VERTICES FL_RENDERER_MAX_GLYPHS * 6
#define FL_RENDERER_MAX_RENDER_BATCHES FL_RENDERER_MAX_GLYPHS
static int __fl_glyphs_size = 0;
static struct flGlyph __fl_glyphs[FL_RENDERER_MAX_GLYPHS];
static struct flVertex __fl_vertices[FL_RENDERER_MAX_VERTICES];
static struct flRenderBatch __fl_renderBatches[FL_RENDERER_MAX_RENDER_BATCHES];

char* flReadFileContents(const char *filepath)
{
	long size;
	char *buffer;
	FILE* fp = fopen(filepath, "rb");

	if (!fp) {
		fclose(fp);
		fputs("Failed to open file", stderr);
	}

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	rewind(fp);

	buffer = calloc(1, size + 1);

	if (!buffer) {
		fclose(fp);
		fputs("memory alloc fails", stderr);
	}

	if (1 != fread(buffer, size, 1, fp)) {
		fclose(fp);
		free(buffer);
		fputs("entire read fails", stderr);
	}

	fclose(fp);
	return buffer;
}

void flFreeBuffer(char *buffer)
{
	free(buffer);
}

void flMat4Identity(struct flMat4 *out)
{
	memset(out->data, 0, sizeof(struct flMat4));

	out->data[0 + 0 * 4] = 1.0f;
	out->data[1 + 1 * 4] = 1.0f;
	out->data[2 + 2 * 4] = 1.0f;
	out->data[3 + 3 * 4] = 1.0f;
}

void flMat4Ortho(float left, float right, float bottom, float top, float zNear, float zFar, struct flMat4 *out)
{
	flMat4Identity(out);

	out->data[0 + 0 * 4] = 2.0f / (right - left);
	out->data[1 + 1 * 4] = 2.0f / (top - bottom);
	out->data[2 + 2 * 4] = 2.0f / (zNear - zFar);

	out->data[3 * 4 + 0] = (left + right) / (left - right);
	out->data[3 * 4 + 1] = (bottom + top) / (bottom - top);
	out->data[3 * 4 + 2] = (zNear + zFar) / (zNear - zFar);
}

#ifndef FL_SDL2_NO_INCLUDE
void flWindowCreate(int width, int height, const char *title)
{
	__fl_window_width = width;
	__fl_window_height = height;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	__fl_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, __fl_window_width, __fl_window_height, SDL_WINDOW_OPENGL);
	SDL_GL_CreateContext(__fl_window);
	SDL_GL_SetSwapInterval(0);

	gladLoadGL();

	glViewport(0, 0, __fl_window_width, __fl_window_height);
	glClearColor(0, 0, 0, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	memset(__fl_keysDown, 0, FL_MAX_KEYS);
	memset(__fl_keysDownLast, 0, FL_MAX_KEYS);
	memset(__fl_keysPressed, 0, FL_MAX_KEYS);
}

int flWindowGetWidth()
{
	return __fl_window_width;
}

int flWindowGetHeight()
{
	return __fl_window_height;
}

void flWindowUpdate()
{
	for (int i = 0; i < FL_MAX_KEYS; i++)
		__fl_keysPressed[i] = __fl_keysDown[i] && !__fl_keysDownLast[i];
	memcpy(__fl_keysDownLast, __fl_keysDown, FL_MAX_KEYS);

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			__fl_window_should_close = true;
			break;
		case SDL_KEYDOWN:
			__fl_keysDown[e.key.keysym.scancode] = true;
			break;
		case SDL_KEYUP:
			__fl_keysDown[e.key.keysym.scancode] = false;
			break;
		}
	}

	SDL_GL_SwapWindow(__fl_window);
}

bool flWindowClose()
{
	return __fl_window_should_close;
}

void flWindowDestroy()
{
	SDL_DestroyWindow(__fl_window);
	SDL_Quit();
}

bool flIsKeyDown(enum flKey key) {
	return __fl_keysDown[key];
}

bool flIsKeyPress(enum flKey key) {
	return __fl_keysPressed[key];
}
#endif /* FL_SDL2_NO_INCLUDE */

flShader flShaderCreate()
{
	return glCreateProgram();
}

void flShaderAttach(flShader program, const char *src, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const char* const *)&src, 0);
	glCompileShader(shader);

	int status = 0;
	int maxLength = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		char *infoLog = malloc(maxLength * sizeof(char));
		glGetShaderInfoLog(shader, maxLength, NULL, infoLog);
		fputs("Error compiling shader\n", stderr);
		fputs(infoLog, stderr);
		flFreeBuffer(infoLog);
		glDeleteShader(shader);
	}

	glAttachShader(program, shader);
	glDeleteShader(shader);
}

void flShaderLink(flShader program)
{
	glLinkProgram(program);
	int status = 0;
	int maxLength = 0;

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		char *infoLog = malloc(maxLength * sizeof(char));
		glGetProgramInfoLog(program, maxLength, NULL, infoLog);
		fputs("Failed to link program\n", stderr);
		fputs(infoLog, stderr);
		flFreeBuffer(infoLog);
		glDeleteProgram(program);
	}

	glValidateProgram(program);
}

void flShaderDestroy(flShader program)
{
	glDeleteProgram(program);
}

void flShaderBind(flShader program)
{
	glUseProgram(program);
}

void flShaderBindAttribLocation(flShader program, int index, const char *name)
{
	glBindAttribLocation(program, index, name);
}

GLint flShaderGetUniformLocation(flShader program, const char *name)
{
	return glGetUniformLocation(program, name);
}

void flShaderUniform1f(GLint location, float value)
{
	glUniform1f(location, value);
}

void flShaderUniform1fv(GLint location, float value[], int size)
{
	glUniform1fv(location, size, value);
}

void flShaderUniform1i(GLint location, int value)
{
	glUniform1i(location, value);
}

void flShaderUniform1iv(GLint location, int value[], int size)
{
	glUniform1iv(location, size, value);
}

void flShaderUniform2f(GLint location, struct flVec2 *vec)
{
	glUniform2f(location, vec->x, vec->y);
}

void flShaderUniform3f(GLint location, struct flVec3 *vec)
{
	glUniform3f(location, vec->x, vec->y, vec->z);
}

void flShaderUniform4f(GLint location, struct flVec4 *vec)
{
	glUniform4f(location, vec->x, vec->y, vec->z, vec->w);
}

void flShaderUniformMatrix4fv(GLint location, struct flMat4 *matrix)
{
	glUniformMatrix4fv(location, 1, false, matrix->data);
}

struct flTexture flTextureLoadFromFile(const char *filepath)
{
	struct flTexture tmp;

	glGenTextures(1, &tmp.id);
	glBindTexture(GL_TEXTURE_2D, tmp.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int w, h, n;
	unsigned char* data = stbi_load(filepath, &w, &h, &n, 0);
	if (n == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else if (n == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	tmp.width = (float)w;
	tmp.height = (float)h;
	return tmp;
}

void flTextureDestroy(struct flTexture *texture)
{
	glDeleteTextures(1, &texture->id);
}

void flRendererInit()
{
	__fl_shader = flShaderCreate();
	flShaderAttach(__fl_shader, __fl_vertex_shader, GL_VERTEX_SHADER);
	flShaderAttach(__fl_shader, __fl_fragment_shader, GL_FRAGMENT_SHADER);
	flShaderLink(__fl_shader);

	flShaderBindAttribLocation(__fl_shader, 0, "position");
	flShaderBindAttribLocation(__fl_shader, 1, "uv");
	flShaderBindAttribLocation(__fl_shader, 2, "color");

	flShaderBind(__fl_shader);

	if (__fl_vao == 0) glGenVertexArrays(1, &__fl_vao);
	glBindVertexArray(__fl_vao);

	if (__fl_vbo == 0) glGenBuffers(1, &__fl_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, __fl_vbo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, FL_VERTEX_SIZE, (const void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, FL_VERTEX_SIZE, (const void*)(sizeof(struct flVec2)));
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, FL_VERTEX_SIZE, (const void*)(sizeof(struct flVec2) * 2));
}

void flRendererSetProjectionMatrix(struct flMat4* pr_matrix)
{
	GLint loc = flShaderGetUniformLocation(__fl_shader, "pr_matrix");
	flShaderUniformMatrix4fv(loc, pr_matrix);
}

void flRendererClear(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void flRendererBegin()
{
	__fl_glyphs_size = 0;
	memset(__fl_glyphs, 0, FL_GLYPH_SIZE * FL_RENDERER_MAX_GLYPHS);
	memset(__fl_renderBatches, 0, FL_RENDER_BATCH_SIZE * FL_RENDERER_MAX_GLYPHS);
	memset(__fl_vertices, 0, FL_VERTEX_SIZE * FL_RENDERER_MAX_GLYPHS * 6);
}

void flRendererDraw(struct flTexture *texture, struct flVec4 *destRectangle, struct flVec4 *srcRectangle, unsigned int color)
{
	if (__fl_glyphs_size >= FL_RENDERER_MAX_GLYPHS) {
		flRendererEnd();
		flRendererBegin();
	}

	struct flGlyph *__fl_tmp_glyph = &__fl_glyphs[__fl_glyphs_size++];
	__fl_tmp_glyph->textureId = texture->id;

	__fl_tmp_glyph->topLeft.position.x = destRectangle->x;
	__fl_tmp_glyph->topLeft.position.y = destRectangle->y;
	__fl_tmp_glyph->topLeft.uv.x = srcRectangle->x;
	__fl_tmp_glyph->topLeft.uv.y = srcRectangle->y;
	__fl_tmp_glyph->topLeft.color = color;

	__fl_tmp_glyph->bottomLeft.position.x = destRectangle->x;
	__fl_tmp_glyph->bottomLeft.position.y = destRectangle->y + destRectangle->w;
	__fl_tmp_glyph->bottomLeft.uv.x = srcRectangle->x;
	__fl_tmp_glyph->bottomLeft.uv.y = srcRectangle->y + srcRectangle->w;
	__fl_tmp_glyph->bottomLeft.color = color;

	__fl_tmp_glyph->bottomRight.position.x = destRectangle->x + destRectangle->z;
	__fl_tmp_glyph->bottomRight.position.y = destRectangle->y + destRectangle->w;
	__fl_tmp_glyph->bottomRight.uv.x = srcRectangle->x + srcRectangle->z;
	__fl_tmp_glyph->bottomRight.uv.y = srcRectangle->y + srcRectangle->w;
	__fl_tmp_glyph->bottomRight.color = color;

	__fl_tmp_glyph->topRight.position.x = destRectangle->x + destRectangle->z;
	__fl_tmp_glyph->topRight.position.y = destRectangle->y;
	__fl_tmp_glyph->topRight.uv.x = srcRectangle->x + srcRectangle->z;
	__fl_tmp_glyph->topRight.uv.y = srcRectangle->y;
	__fl_tmp_glyph->topRight.color = color;
}

static int md_comparator(const void *v1, const void *v2)
{
	const struct flGlyph *p1 = (struct flGlyph*)v1;
	const struct flGlyph *p2 = (struct flGlyph*)v2;
	return p1->textureId - p2->textureId;
}

void flRendererEnd()
{
	if (__fl_glyphs_size == 0) return;
	qsort(__fl_glyphs, __fl_glyphs_size, FL_GLYPH_SIZE, md_comparator);

	int crb = 0;
	__fl_renderBatches[crb].offset = 0;
	__fl_renderBatches[crb].numVertices = 6;
	__fl_renderBatches[crb].textureId = __fl_glyphs[0].textureId;

	int offset = 0;
	__fl_vertices[offset++] = __fl_glyphs[0].topLeft;
	__fl_vertices[offset++] = __fl_glyphs[0].bottomLeft;
	__fl_vertices[offset++] = __fl_glyphs[0].bottomRight;
	__fl_vertices[offset++] = __fl_glyphs[0].bottomRight;
	__fl_vertices[offset++] = __fl_glyphs[0].topRight;
	__fl_vertices[offset++] = __fl_glyphs[0].topLeft;

	for (int i = 1; i < __fl_glyphs_size; i++) {

		if (__fl_glyphs[i].textureId != __fl_glyphs[i - 1].textureId) {
			crb++;
			__fl_renderBatches[crb].offset = offset;
			__fl_renderBatches[crb].numVertices = 6;
			__fl_renderBatches[crb].textureId = __fl_glyphs[i].textureId;
		}
		else {
			__fl_renderBatches[crb].numVertices += 6;
		}

		__fl_vertices[offset++] = __fl_glyphs[i].topLeft;
		__fl_vertices[offset++] = __fl_glyphs[i].bottomLeft;
		__fl_vertices[offset++] = __fl_glyphs[i].bottomRight;
		__fl_vertices[offset++] = __fl_glyphs[i].bottomRight;
		__fl_vertices[offset++] = __fl_glyphs[i].topRight;
		__fl_vertices[offset++] = __fl_glyphs[i].topLeft;
	}

	glBindVertexArray(__fl_vao);
	glBindBuffer(GL_ARRAY_BUFFER, __fl_vbo);
	glBufferData(GL_ARRAY_BUFFER, FL_VERTEX_SIZE * offset, (const void*)0, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, FL_VERTEX_SIZE * offset, __fl_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (int i = 0; i < crb + 1; i++) {
		glBindTexture(GL_TEXTURE_2D, __fl_renderBatches[i].textureId);
		glDrawArrays(GL_TRIANGLES, __fl_renderBatches[i].offset, __fl_renderBatches[i].numVertices);
	}
}

void flRendererDestroy()
{
	flShaderDestroy(__fl_shader);
	glDeleteVertexArrays(1, &__fl_vao);
	glDeleteBuffers(1, &__fl_vbo);
}
#endif /* FL_IMPLEMENTATION */

/*
MIT License
Copyright (c) 2017 Manidakis Marinos
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/