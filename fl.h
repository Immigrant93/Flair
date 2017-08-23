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

#if defined(_WIN32) && !defined(__MINGW32__)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif /* _CRT_SECURE_NO_WARNINGS */
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif /* _CRT_NONSTDC_NO_DEPRECATE */
#ifndef _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_NON_CONFORMING_SWPRINTFS
#endif /* _CRT_NONSTDC_NO_DEPRECATE */
#endif /* defined(_WIN32) && !defined(__MINGW32__) */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef FL_STATIC
#define FLAPI static
#else
#define FLAPI extern
#endif /* FL_STATIC */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef FL_GLEW_NO_INCLUDE
#include <GL/glew.h>
#endif /* FL_GLEW_NO_INCLUDE */

/*
 * Read file contents
 * @return char* buffer with the contents of the file
 * Remember to call flFreeBuffer(char *buffer) after you are done using the buffer
 */
FLAPI char* flReadFileContents(const char *filepath);

/*
 * Free the buffer allocated from flReadFileContents
 */
FLAPI void flFreeBuffer(char *buffer);

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
FLAPI void flMat4Identity(struct flMat4 *out);

/*
 * Create an orthographic projection matrix
 * Stores the matrix at {@param out}
 */
FLAPI void flMat4Ortho(float left, float right, float bottom, float top, float zNear, float zFar, struct flMat4 *out);

typedef unsigned int flShader;
typedef unsigned int flTexture;

/*
 * Create the shader program
 */
FLAPI flShader flShaderCreate();

/*
 * Attach a shader to the program
 * @param const char *src contains the source of the shader, not the path to the file containing the source
 * @param GLenum shaderType is the type of shader { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, ... }
 */
FLAPI void flShaderAttach(flShader program, const char *src, GLenum shaderType);

/*
 * Link the shaders
 */
FLAPI void flShaderLink(flShader program);

/*
 * Destroy the shader program
 */
FLAPI void flShaderDestroy(flShader program);

/*
 * Use the shader program
 */
FLAPI void flShaderBind(flShader program);

/*
 * Bind the attribute location in shader program
 */
FLAPI void flShaderBindAttribLocation(flShader program, int index, const char *name);

/*
 * Get the uniform location
 */
FLAPI int flShaderGetUniformLocation(flShader program, const char *name);

/*
 * Set values for different type of uniforms in shader program
 */
/* float uniform */
FLAPI void flShaderUniform1f(int location, float value);
/* float array uniform */
FLAPI void flShaderUniform1fv(int location, float value[], int size);
/* int uniform */
FLAPI void flShaderUniform1i(int location, int value);
/* int array uniform */
FLAPI void flShaderUniform1iv(int location, int value[], int size);
/* vec2 uniform */
FLAPI void flShaderUniform2f(int location, struct flVec2 *vec);
/* vec3 uniform */
FLAPI void flShaderUniform3f(int location, struct flVec3 *vec);
/* vec4 uniform */
FLAPI void flShaderUniform4f(int location, struct flVec4 *vec);
/* mat4 uniform */
FLAPI void flShaderUniformMatrix4fv(int location, struct flMat4 *matrix);

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
	flTexture texture;
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
	flTexture texture;
};

/*
 * Initializes the renderer
 * Create the shader program
 * Bind attributes
 * Creates the Vertex Array and the Vertex Buffer
 * Enables vertex attrib arrays and set the vertex pointers
 */
FLAPI void flRendererInit();

/**
 * Set the projection matrix to be used
 * Default is identity matrix
 */
FLAPI void flRendererSetProjectionMatrix(struct flMat4 *pr_matrix);

/*
 * Clear the back buffer with rgba color
 */
FLAPI void flRendererClear(float r, float g, float b, float a);

/*
 * Begin the process of batching vertices
 */
FLAPI void flRendererBegin();

/*
 * Constructs the flGlyphs
 */
FLAPI void flRendererDraw(flTexture texture, struct flVec4 *destRectangle, struct flVec4 *srcRectangle, unsigned int color);

/*
 * Sort all glyphs by texture. Create the render batches and the vertices array
 * Push data to OpenGL and draw everything on the screen
 */
FLAPI void flRendererEnd();

/*
 * Clean up method. Destroys the shader program as well
 */
FLAPI void flRendererDestroy();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __FL_H__ */
/*----------------------------------------------------------------------------*/
/*                        END OF HEADER FILE                                  */
/*----------------------------------------------------------------------------*/

#ifdef FL_IMPLEMENTATION

static unsigned int __fl_vao;
static unsigned int __fl_vbo;
static flShader __fl_shader;

static const char *__fl_vertex_shader =
"#version 330 core                                          \n"
"                                                           \n"
"layout(location = 0) in vec2 position;                     \n"
"layout(location = 1) in vec2 uv;                           \n"
"layout(location = 2) in vec4 color;                        \n"
"                                                           \n"
"uniform mat4 pr_matrix = mat4(1.0);                        \n"
"                                                           \n"
"out vec2 vsUV;                                             \n"
"out vec4 vsColor;                                          \n"
"                                                           \n"
"void main() {                                              \n"
"	gl_Position = pr_matrix * vec4(position, 0.0, 1.0);     \n"
"	vsUV = uv;                                              \n"
"	vsColor = color;                                        \n"
"}                                                          \n";

static const char *__fl_fragment_shader =
"#version 330 core                                          \n"
"                                                           \n"
"layout(location = 0) out vec4 outColor;                    \n"
"uniform sampler2D textureSampler;                          \n"
"                                                           \n"
"in vec2 vsUV;                                              \n"
"in vec4 vsColor;                                           \n"
"                                                           \n"
"void main() {                                              \n"
"	outColor = texture(textureSampler, vsUV) * vsColor;     \n"
"}                                                          \n";

#define FL_VERTEX_SIZE sizeof(struct flVertex)
#define FL_GLYPH_SIZE sizeof(struct flGlyph)
#define FL_RENDER_BATCH_SIZE sizeof(struct flRenderBatch)
#define FL_RENDERER_MAX_GLYPHS 1000
#define FL_RENDERER_MAX_VERTICES FL_RENDERER_MAX_GLYPHS * 6
#define FL_RENDERER_MAX_RENDER_BATCHES FL_RENDERER_MAX_GLYPHS
static unsigned int __fl_glyphs_size = 0;
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

	buffer = (char*)calloc(1, size + 1);

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
		char *infoLog = (char*)malloc(maxLength * sizeof(char));
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
		char *infoLog = (char*)malloc(maxLength * sizeof(char));
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

int flShaderGetUniformLocation(flShader program, const char *name)
{
	return glGetUniformLocation(program, name);
}

void flShaderUniform1f(int location, float value)
{
	glUniform1f(location, value);
}

void flShaderUniform1fv(int location, float value[], int size)
{
	glUniform1fv(location, size, value);
}

void flShaderUniform1i(int location, int value)
{
	glUniform1i(location, value);
}

void flShaderUniform1iv(int location, int value[], int size)
{
	glUniform1iv(location, size, value);
}

void flShaderUniform2f(int location, struct flVec2 *vec)
{
	glUniform2f(location, vec->x, vec->y);
}

void flShaderUniform3f(int location, struct flVec3 *vec)
{
	glUniform3f(location, vec->x, vec->y, vec->z);
}

void flShaderUniform4f(int location, struct flVec4 *vec)
{
	glUniform4f(location, vec->x, vec->y, vec->z, vec->w);
}

void flShaderUniformMatrix4fv(int location, struct flMat4 *matrix)
{
	glUniformMatrix4fv(location, 1, false, matrix->data);
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
	int loc = flShaderGetUniformLocation(__fl_shader, "pr_matrix");
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

void flRendererDraw(flTexture texture, struct flVec4 *destRectangle, struct flVec4 *srcRectangle, unsigned int color)
{
	if (__fl_glyphs_size >= FL_RENDERER_MAX_GLYPHS) {
		flRendererEnd();
		flRendererBegin();
	}

	struct flGlyph *__fl_tmp_glyph = &__fl_glyphs[__fl_glyphs_size++];
	__fl_tmp_glyph->texture = texture;

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
	return p1->texture - p2->texture;
}

void flRendererEnd()
{
	if (__fl_glyphs_size == 0) return;
	qsort(__fl_glyphs, __fl_glyphs_size, FL_GLYPH_SIZE, md_comparator);

	int crb = 0;
	__fl_renderBatches[crb].offset = 0;
	__fl_renderBatches[crb].numVertices = 6;
	__fl_renderBatches[crb].texture = __fl_glyphs[0].texture;

	int offset = 0;
	__fl_vertices[offset++] = __fl_glyphs[0].topLeft;
	__fl_vertices[offset++] = __fl_glyphs[0].bottomLeft;
	__fl_vertices[offset++] = __fl_glyphs[0].bottomRight;
	__fl_vertices[offset++] = __fl_glyphs[0].bottomRight;
	__fl_vertices[offset++] = __fl_glyphs[0].topRight;
	__fl_vertices[offset++] = __fl_glyphs[0].topLeft;

	for (int i = 1; i < __fl_glyphs_size; i++) {

		if (__fl_glyphs[i].texture != __fl_glyphs[i - 1].texture) {
			crb++;
			__fl_renderBatches[crb].offset = offset;
			__fl_renderBatches[crb].numVertices = 6;
			__fl_renderBatches[crb].texture = __fl_glyphs[i].texture;
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
	flShaderBind(__fl_shader);
	for (int i = 0; i < crb + 1; i++) {
		glBindTexture(GL_TEXTURE_2D, __fl_renderBatches[i].texture);
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