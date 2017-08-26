#ifndef __FL_H__
#define __FL_H__

#if defined(_WIN32) && !defined(__MINGW32__)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif
#ifndef _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_NON_CONFORMING_SWPRINTFS
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FLAPI
#ifdef FL_STATIC
#define FLAPI static
#else
#define FLAPI extern
#endif
#endif

#ifndef __cplusplus
#define bool unsigned char
#define false 0
#define true 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>

typedef struct flVec2 flVec2;
struct flVec2 {
	float x;
	float y;
};

typedef struct flVec3 flVec3;
struct flVec3 {
	float x;
	float y;
	float z;
};

typedef struct flVec4 flVec4;
struct flVec4 {
	float x;
	float y;
	float z;
	float w;
};

typedef struct flMat4 flMat4;
struct flMat4 {
	float data[16];
};

FLAPI void flMat4Identity(flMat4 *out);
FLAPI void flMat4Ortho(float left, float right, float bottom, float top, float zNear, float zFar, flMat4 *out);
FLAPI void flShaderAttach(unsigned int program, const char *src, GLenum shaderType);
FLAPI void flShaderLink(unsigned int program);
FLAPI void flRendererInit();
FLAPI void flRendererSetProjectionMatrix(flMat4 *pr_matrix);
FLAPI void flRendererBegin();
FLAPI void flRendererDraw(unsigned int texture, flVec4 destRectangle, flVec4 srcRectangle, unsigned int color);
FLAPI void flRendererEnd();
FLAPI void flRendererDestroy();

#ifdef __cpluscplus
}
#endif

#endif /* __FL_H__ */
/*----------------------------------------------------------------------------*/
/*                        END OF HEADER FILE                                  */
/*----------------------------------------------------------------------------*/

#ifdef FL_IMPLEMENTATION

static unsigned int __fl_vao;
static unsigned int __fl_vbo;
static unsigned int __fl_shader;

static const char *__fl_vertex_shader =
"#version 150 \n"
"layout(location = 0) in vec2 position; \n"
"layout(location = 1) in vec2 uv; \n"
"layout(location = 2) in vec4 color; \n"
"uniform mat4 pr_matrix = mat4(1.0); \n"
"out vec2 vsUV; \n"
"out vec4 vsColor; \n"
"void main() { \n"
"    gl_Position = pr_matrix * vec4(position, 0.0, 1.0); \n"
"    vsUV = uv; \n"
"    vsColor = color; \n"
"} \n";

static const char *__fl_fragment_shader =
"#version 150 \n"
"layout(location = 0) out vec4 outColor; \n"
"uniform sampler2D textureSampler; \n"
"in vec2 vsUV; \n"
"in vec4 vsColor; \n"
"void main() { \n"
"    outColor = texture(textureSampler, vsUV) * vsColor; \n"
"} \n";

typedef struct flVertex flVertex;
struct flVertex {
	flVec2 position;
	flVec2 uv;
	unsigned int color;
};

typedef struct flGlyph flGlyph;
struct flGlyph {
	unsigned int texture;
	flVertex topLeft;
	flVertex bottomLeft;
	flVertex bottomRight;
	flVertex topRight;
};

typedef struct flRenderBatch flRenderBatch;
struct flRenderBatch {
	int offset;
	int numVertices;
	unsigned int texture;
};

#define FL_VERTEX_SIZE sizeof(flVertex)
#define FL_GLYPH_SIZE sizeof(flGlyph)
#define FL_RENDER_BATCH_SIZE sizeof(flRenderBatch)
#define FL_RENDERER_MAX_GLYPHS 1000
#define FL_RENDERER_MAX_VERTICES FL_RENDERER_MAX_GLYPHS * 6
#define FL_RENDERER_MAX_RENDER_BATCHES FL_RENDERER_MAX_GLYPHS

static int __fl_glyphs_size = 0;
static struct flGlyph __fl_glyphs[FL_RENDERER_MAX_GLYPHS];
static struct flVertex __fl_vertices[FL_RENDERER_MAX_VERTICES];
static struct flRenderBatch __fl_renderBatches[FL_RENDERER_MAX_RENDER_BATCHES];

void flMat4Identity(flMat4 *out) {
	memset(out->data, 0, sizeof(flMat4));

	out->data[0 + 0 * 4] = 1.0f;
	out->data[1 + 1 * 4] = 1.0f;
	out->data[2 + 2 * 4] = 1.0f;
	out->data[3 + 3 * 4] = 1.0f;
}

void flMat4Ortho(float left, float right, float bottom, float top, float zNear, float zFar, flMat4 *out) {
	flMat4Identity(out);

	out->data[0 + 0 * 4] = 2.0f / (right - left);
	out->data[1 + 1 * 4] = 2.0f / (top - bottom);
	out->data[2 + 2 * 4] = 2.0f / (zNear - zFar);

	out->data[3 * 4 + 0] = (left + right) / (left - right);
	out->data[3 * 4 + 1] = (bottom + top) / (bottom - top);
	out->data[3 * 4 + 2] = (zNear + zFar) / (zNear - zFar);
}

void flShaderAttach(unsigned int program, const char *src, GLenum shaderType) {
	unsigned int shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const char *const *)&src, 0);
	glCompileShader(shader);

	int status = 0;
	int maxLength = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != true) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		char *infoLog = (char *)malloc(maxLength * sizeof(char));
		glGetShaderInfoLog(shader, maxLength, NULL, infoLog);
		printf("Failed to compile shader \n");
		printf("%s\n", infoLog);
		free(infoLog);
		glDeleteShader(shader);
	}

	glAttachShader(program, shader);
	glDeleteShader(shader);
}

void flShaderLink(unsigned int program) {
	glLinkProgram(program);
	int status = 0;
	int maxLength = 0;

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != true) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		char *infoLog = (char *)malloc(maxLength * sizeof(char));
		glGetProgramInfoLog(program, maxLength, NULL, infoLog);
		printf("Failed to link program\n");
		printf("%s\n", infoLog);
		free(infoLog);
		glDeleteProgram(program);
	}

	glValidateProgram(program);
}

void flRendererInit() {
	__fl_shader = glCreateProgram();
	flShaderAttach(__fl_shader, __fl_vertex_shader, GL_VERTEX_SHADER);
	flShaderAttach(__fl_shader, __fl_fragment_shader, GL_FRAGMENT_SHADER);
	flShaderLink(__fl_shader);

	int __fl_shader_attrib0 = glGetAttribLocation(__fl_shader, "position");
	int __fl_shader_attrib1 = glGetAttribLocation(__fl_shader, "uv");
	int __fl_shader_attrib2 = glGetAttribLocation(__fl_shader, "color");

	glBindAttribLocation(__fl_shader, __fl_shader_attrib0, "position");
	glBindAttribLocation(__fl_shader, __fl_shader_attrib1, "uv");
	glBindAttribLocation(__fl_shader, __fl_shader_attrib2, "color");

	glUseProgram(__fl_shader);

	if (__fl_vao == 0) glGenVertexArrays(1, &__fl_vao);
	glBindVertexArray(__fl_vao);

	if (__fl_vbo == 0) glGenBuffers(1, &__fl_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, __fl_vbo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 2, GL_FLOAT, false, FL_VERTEX_SIZE, (const void *)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, FL_VERTEX_SIZE, (const void *)(sizeof(flVec2)));
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, true, FL_VERTEX_SIZE, (const void *)(sizeof(flVec2) * 2));
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void flRendererSetProjectionMatrix(flMat4 *pr_matrix) {
	int loc = glGetUniformLocation(__fl_shader, "pr_matrix");
	glUniformMatrix4fv(loc, 1, false, pr_matrix->data);
}

void flRendererBegin() {
	__fl_glyphs_size = 0;
	memset(__fl_glyphs, 0, FL_GLYPH_SIZE * FL_RENDERER_MAX_GLYPHS);
	memset(__fl_renderBatches, 0, FL_RENDER_BATCH_SIZE * FL_RENDERER_MAX_GLYPHS);
	memset(__fl_vertices, 0, FL_VERTEX_SIZE * FL_RENDERER_MAX_GLYPHS * 6);
}

void flRendererDraw(unsigned int texture, flVec4 destRectangle, flVec4 srcRectangle, unsigned int color) {
	if (__fl_glyphs_size >= FL_RENDERER_MAX_GLYPHS) {
		flRendererEnd();
		flRendererBegin();
	}

	flGlyph *__fl_tmp_glyph = &__fl_glyphs[__fl_glyphs_size++];
	__fl_tmp_glyph->texture = texture;

	__fl_tmp_glyph->topLeft.position.x = destRectangle.x;
	__fl_tmp_glyph->topLeft.position.y = destRectangle.y;
	__fl_tmp_glyph->topLeft.uv.x = srcRectangle.x;
	__fl_tmp_glyph->topLeft.uv.y = srcRectangle.y;
	__fl_tmp_glyph->topLeft.color = color;

	__fl_tmp_glyph->bottomLeft.position.x = destRectangle.x;
	__fl_tmp_glyph->bottomLeft.position.y = destRectangle.y + destRectangle.w;
	__fl_tmp_glyph->bottomLeft.uv.x = srcRectangle.x;
	__fl_tmp_glyph->bottomLeft.uv.y = srcRectangle.y + srcRectangle.w;
	__fl_tmp_glyph->bottomLeft.color = color;

	__fl_tmp_glyph->bottomRight.position.x = destRectangle.x + destRectangle.z;
	__fl_tmp_glyph->bottomRight.position.y = destRectangle.y + destRectangle.w;
	__fl_tmp_glyph->bottomRight.uv.x = srcRectangle.x + srcRectangle.z;
	__fl_tmp_glyph->bottomRight.uv.y = srcRectangle.y + srcRectangle.w;
	__fl_tmp_glyph->bottomRight.color = color;

	__fl_tmp_glyph->topRight.position.x = destRectangle.x + destRectangle.z;
	__fl_tmp_glyph->topRight.position.y = destRectangle.y;
	__fl_tmp_glyph->topRight.uv.x = srcRectangle.x + srcRectangle.z;
	__fl_tmp_glyph->topRight.uv.y = srcRectangle.y;
	__fl_tmp_glyph->topRight.color = color;
}

static int md_comparator(const void *v1, const void *v2) {
	const flGlyph *p1 = (flGlyph *)v1;
	const flGlyph *p2 = (flGlyph *)v2;
	return p1->texture - p2->texture;
}

void flRendererEnd() {
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

	int i;
	for (i = 1; i < __fl_glyphs_size; i++) {

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

	glUseProgram(__fl_shader);
	glBindVertexArray(__fl_vao);
	glBindBuffer(GL_ARRAY_BUFFER, __fl_vbo);
	glBufferData(GL_ARRAY_BUFFER, FL_VERTEX_SIZE * offset, (const void *)0, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, FL_VERTEX_SIZE * offset, __fl_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (i = 0; i < crb + 1; i++) {
		glBindTexture(GL_TEXTURE_2D, __fl_renderBatches[i].texture);
		glDrawArrays(GL_TRIANGLES, __fl_renderBatches[i].offset, __fl_renderBatches[i].numVertices);
	}
}

void flRendererDestroy() {
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
