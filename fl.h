/*
 * fl.h - v1.0 - MIT Simple OpenGL 2D renderer
 * https://github.com/Immigrant93/Flair
 * 
 * Do this:
 *      #define FL_IMPLEMENTATION
 * before you include this file in *one* of your C or C++ files to create the 
 * implementation
 * 
 * //i.e it should look like this
 * 
 * #include ...
 * #include ...
 * #include ...
 * #define FL_IMPLEMENTATION
 * #include "fl.h"
 * 
 * Usage example:
 * -----------------------------------------------------------------------------
 * // Initialize renderer
 * flRendererInit();
 * 
 * // Setup the projection matrix
 * struct flMat4 pr_matrix;
 * flMat4Ortho(0.0f, 640.0f, 480.0f, 0.0f, -1.0f, 1.0f, &pr_matrix);
 * flRendererSetProjectionMatrix(&pr_matrix);
 * 
 * // main game loop
 * while(true){ 
 *  // clear screen
 *  glClear(GL_COLOR_BUFFER_BIT);
 * 
 *  // Setup renderer for drawing
 *  flRendererBegin(); 
 * 
 *  // Push vertices to the renderer
 *  // srcRectangle { 0.0f, 0.0f, 1.0f, 1.0f } <-- Draw the whole texture
 *  // color: 0xFFFF0000 <-- solid blue color for blending
 *  flRendererDraw(textureId, (struct flVec4){0.0f, 0.0f, 32.0f, 32.0f}, 
 *      (struct flVec4){0.0f, 0.0f, 1.0f, 1.0f}, 0xFFFF0000 )
 *  
 *  // Setup batches and draw everything 
 *  flRendererEnd();
 *  
 *  // swap the window buffers 
 * }
 * 
 * flRendererDestroy();
 * -----------------------------------------------------------------------------
 */
#ifndef __FL_H__
#define __FL_H__

#if !defined __BEGIN_DECLS && !defined __END_DECLS
# ifdef	__cplusplus
#define __BEGIN_DECLS	extern "C" {
#define __END_DECLS	}
# else
#define __BEGIN_DECLS
#define __END_DECLS
#endif
#endif

#ifndef FLAPI
#ifdef FL_STATIC
#define FLAPI static
#else
#define FLAPI 
#endif
#endif

#ifndef bool
#ifndef __cplusplus
#define bool unsigned char
#define true 1
#define false 0
#endif
#endif

__BEGIN_DECLS

#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* memset */
#include <GL/glew.h> /* all the opengl stuff */

struct flVec2 {
    float x;
    float y;
};

struct flVec3 {
    float x;
    float y;
    float z;
};

struct flVec4 {
    float x;
    float y;
    float z;
    float w;
};

struct flMat4 {
    float data[16];
};

/**
 * Create an identity matrix.
 * @param out: the matrix to store the result.
 */
FLAPI void flMat4Identity(struct flMat4 *out);

/**
 * Create an orthographic projection matrix.
 * @param left
 * @param right
 * @param bottom
 * @param top
 * @param near
 * @param far
 * @param out: the matrix to store the result.
 */
FLAPI void flMat4Ortho(float left, float right, float bottom, float top, 
        float near, float far, struct flMat4 *out);

/**
 * Attach a shader to the program.
 * @param program: the program id generated with glCreateProgram.
 * @param src: the actual source of the shader. *NOT* the file path.
 * @param shaderType: the type of shader ie. GL_VERTEX_SHADER.
 * @return 0 on success
 */
FLAPI bool flShaderAttach(GLuint program, const char *src, GLenum shaderType);

/**
 * Link the program with the shaders attached.
 * @param program: the program id generated with glCreateProgram.
 * @return 0 on success.
 */
FLAPI bool flShaderLink(GLuint program);

/**
 * Initializes the renderer.
 * Creates and sets up the shader, the vertex array and the vertex buffer.
 * Should be called once and *AFTER* the OpenGL context has been created.
 */
FLAPI void flRendererInit();

/**
 * Set the projection matrix for the renderer to use.
 * It pushes it directly to OpenGL.
 * Call this after renderer has been initialized.
 * @param pr_matrix: the projection matrix to use.
 */
FLAPI void flRendererSetProjectionMatrix(struct flMat4 *pr_matrix);

/**
 * Begin the drawing sequence.
 */
FLAPI void flRendererBegin();

/**
 * Draw a textured rectangle.
 * The destination rectangle should have:
 *  destRect.x -> the x coordinate
 *  destRect.y -> the y coordinate
 *  destRect.z -> the width of the rectangle
 *  destRect.w -> the height of the rectangle
 * 
 * You have to compute the source rectangle before pushing it.
 * The renderer does not do that for you.
 * The values should be as the destRectangle (see above)
 * 
 * @param texture:  the texture id
 * @param destRectangle: the destination rectangle
 * @param srcRectangle: the source rectangle
 * @param color: the integer color to use for blending 0xAABBGGRR format
 */
FLAPI void flRendererDraw(GLuint texture, struct flVec4 destRectangle, 
        struct flVec4 srcRectangle, GLuint color);

/**
 * Here is where the actual drawing happens.
 * It sorts all the vertices and batches them based on the texture id
 * Each batch contains the offset in the vertices array that it starts from
 * as well as the number of vertices it holds.
 * The next batch starts from where the last one ended
 */
FLAPI void flRendererEnd();

/**
 * Clean up code.
 * Free the vertex array, the vertex buffer and delete the shader program
 */
FLAPI void flRendererDestroy();

__END_DECLS
#endif /* __FL_H__ */
/* -------------------------------------------------------------------------- */
/*                           END OF HEADER FILE                               */
/* -------------------------------------------------------------------------- */

#ifdef FL_IMPLEMENTATION

static unsigned int __fl_vao;
static unsigned int __fl_vbo;
static unsigned int __fl_shader;

static const char *__fl_vertex_shader =
"#version 150 \n"
"in vec2 position; \n"
"in vec2 uv; \n"
"in vec4 color; \n"
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
"out vec4 outColor; \n"
"uniform sampler2D textureSampler; \n"
"in vec2 vsUV; \n"
"in vec4 vsColor; \n"
"void main() { \n"
"    outColor = texture(textureSampler, vsUV) * vsColor; \n"
"} \n";

struct flVertex {
	struct flVec2 position;
	struct flVec2 uv;
	GLuint color;
};

struct flGlyph {
	GLuint texture;
	struct flVertex topLeft;
	struct flVertex bottomLeft;
	struct flVertex bottomRight;
	struct flVertex topRight;
};

struct flRenderBatch {
	int offset;
	int numVertices;
	GLuint texture;
};

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

/**
 * Create an identity matrix.
 * @param out: the matrix to store the result.
 */
FLAPI void flMat4Identity(struct flMat4 *out)
{
    memset(out->data, 0, sizeof(struct flMat4));
    
    out->data[0 * 4 + 0]  = 1.0f;
    out->data[1 * 4 + 1]  = 1.0f;
    out->data[2 * 4 + 2]  = 1.0f;
    out->data[3 * 4 + 3]  = 1.0f;
}

/**
 * Create an orthographic projection matrix.
 * @param left
 * @param right
 * @param bottom
 * @param top
 * @param near
 * @param far
 * @param out: the matrix to store the result.
 */
FLAPI void flMat4Ortho(float left, float right, float bottom, float top, 
        float near, float far, struct flMat4 *out) {
    flMat4Identity(out);
    
    out->data[0 * 4 + 0] = 2.0f / (right - left);
    out->data[1 * 4 + 1] = 2.0f / (top - bottom);
    out->data[2 * 4 + 2] = 2.0f / (near - far);
    
    /*
     * Set the last row to the appropriate values. Not the last column
     * OpenGL stores matrices column major
     */
    out->data[3 * 4 + 0] = (left + right) / (left - right);
    out->data[3 * 4 + 1] = (bottom + top) / (bottom - top);
    out->data[3 * 4 + 2] = (near + far) / (near - far);
}

/**
 * Attach a shader to the program.
 * @param program: the program id generated with glCreateProgram.
 * @param src: the actual source of the shader. *NOT* the file path.
 * @param shaderType: the type of shader ie. GL_VERTEX_SHADER.
 * @return 0 on success
 */
FLAPI bool flShaderAttach(GLuint program, const char *src, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
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
            return -1;
    }

    glAttachShader(program, shader);
    glDeleteShader(shader);
    return 0;
}

/**
 * Link the program with the shaders attached.
 * @param program: the program id generated with glCreateProgram.
 * @return 0 on success.
 */
FLAPI bool flShaderLink(GLuint program) {
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
        return -1;
    }

    glValidateProgram(program);
    return 0;
}

/**
 * Initializes the renderer.
 * Creates and sets up the shader, the vertex array and the vertex buffer.
 * Should be called once and *AFTER* the OpenGL context has been created.
 */
FLAPI void flRendererInit() {
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

    glVertexAttribPointer(0, 2, GL_FLOAT, false, FL_VERTEX_SIZE, 
        (const void *)0);

    glVertexAttribPointer(1, 2, GL_FLOAT, false, FL_VERTEX_SIZE, 
        (const void *)(sizeof(struct flVec2)));

    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, true, FL_VERTEX_SIZE, 
        (const void *)(sizeof(struct flVec2) * 2));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/**
 * Set the projection matrix for the renderer to use.
 * It pushes it directly to OpenGL.
 * Call this after renderer has been initialized.
 * @param pr_matrix: the projection matrix to use.
 */
FLAPI void flRendererSetProjectionMatrix(struct flMat4 *pr_matrix) {
    int loc = glGetUniformLocation(__fl_shader, "pr_matrix");
    glUniformMatrix4fv(loc, 1, false, pr_matrix->data);
}

/**
 * Begin the drawing sequence.
 */
FLAPI void flRendererBegin() {
    __fl_glyphs_size = 0;
    memset(__fl_glyphs, 0, FL_GLYPH_SIZE * FL_RENDERER_MAX_GLYPHS);
    memset(__fl_renderBatches, 0, FL_RENDER_BATCH_SIZE *FL_RENDERER_MAX_GLYPHS);
    memset(__fl_vertices, 0, FL_VERTEX_SIZE * FL_RENDERER_MAX_VERTICES);
}

/**
 * Draw a textured rectangle.
 * The destination rectangle should have:
 *  destRect.x -> the x coordinate
 *  destRect.y -> the y coordinate
 *  destRect.z -> the width of the rectangle
 *  destRect.w -> the height of the rectangle
 * 
 * You have to compute the source rectangle before pushing it.
 * The renderer does not do that for you.
 * The values should be as the destRectangle (see above)
 * 
 * @param texture:  the texture id
 * @param destRectangle: the destination rectangle
 * @param srcRectangle: the source rectangle
 * @param color: the integer color to use for blending 0xAABBGGRR format
 */
FLAPI void flRendererDraw(GLuint texture, struct flVec4 destRectangle, 
        struct flVec4 srcRectangle, GLuint color) {

    /*
     * if we reached the end of the array draw everything and start over
     */
    if (__fl_glyphs_size >= FL_RENDERER_MAX_GLYPHS) {
        flRendererEnd();
        flRendererBegin();
    }
    
    /*
     * Get the reference to the next element on the array
     */
    struct flGlyph *__fl_tmp_glyph = &__fl_glyphs[__fl_glyphs_size++];
    
    /*
     * Construct the new glyph
     */
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

/*
 * Integer comparator function for sorting the glyphs by texture id
 */
static int fl_glyph_comparator(const void *v1, const void *v2) {
    const struct flGlyph *p1 = (struct flGlyph *)v1;
    const struct flGlyph *p2 = (struct flGlyph *)v2;
    return p1->texture - p2->texture;
}

/**
 * Here is where the actual drawing happens.
 * It sorts all the vertices and batches them based on the texture id
 * Each batch contains the offset in the vertices array that it starts from
 * as well as the number of vertices it holds.
 * The next batch starts from where the last one ended
 */
FLAPI void flRendererEnd() {
    /*
     * No glyphs constructed. Nothing to do here
     */
    if (__fl_glyphs_size == 0) return;
    
    /*
     * Sort all the glyph by texture id
     */
    qsort(__fl_glyphs, __fl_glyphs_size, FL_GLYPH_SIZE, fl_glyph_comparator);

    /*
     * crb <-- current render batch
     * We setup the first manually 
     * We use a for loop for the rest of them
     */
    int crb = 0;
    __fl_renderBatches[crb].offset = 0;
    __fl_renderBatches[crb].numVertices = 6;
    __fl_renderBatches[crb].texture = __fl_glyphs[0].texture;
    
    /*
     * Use the sorted glyphs array to construct the vertices array
     * that will be pushed to OpenGL
     */
    int offset = 0;
    __fl_vertices[offset++] = __fl_glyphs[0].topLeft;
    __fl_vertices[offset++] = __fl_glyphs[0].bottomLeft;
    __fl_vertices[offset++] = __fl_glyphs[0].bottomRight;
    __fl_vertices[offset++] = __fl_glyphs[0].bottomRight;
    __fl_vertices[offset++] = __fl_glyphs[0].topRight;
    __fl_vertices[offset++] = __fl_glyphs[0].topLeft;
    
    /*
     * First batch was created. Setup the rest
     * On each iteration we check the previous vertex what texture id it has
     */
    int i;
    for (i = 1; i < __fl_glyphs_size; i++) {
        if (__fl_glyphs[i].texture != __fl_glyphs[i - 1].texture) {
            /*
             * Different texture id 
             * Setup a new render batch
             */
            crb++;
            __fl_renderBatches[crb].offset = offset;
            __fl_renderBatches[crb].numVertices = 6;
            __fl_renderBatches[crb].texture = __fl_glyphs[i].texture;
        }
        else {
            /*
             * Same texture id
             * Update the current render batch
             */
            __fl_renderBatches[crb].numVertices += 6;
        }
        __fl_vertices[offset++] = __fl_glyphs[i].topLeft;
        __fl_vertices[offset++] = __fl_glyphs[i].bottomLeft;
        __fl_vertices[offset++] = __fl_glyphs[i].bottomRight;
        __fl_vertices[offset++] = __fl_glyphs[i].bottomRight;
        __fl_vertices[offset++] = __fl_glyphs[i].topRight;
        __fl_vertices[offset++] = __fl_glyphs[i].topLeft;
    }

    /*
     * All render batches were created as well as the vertices array
     */
    glUseProgram(__fl_shader);
    glBindVertexArray(__fl_vao);
    glBindBuffer(GL_ARRAY_BUFFER, __fl_vbo);
    
    /*
     * Orphan the buffer. Faster this way
     */
    glBufferData(GL_ARRAY_BUFFER, FL_VERTEX_SIZE * offset, (const void *)0, 
        GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, FL_VERTEX_SIZE * offset, __fl_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /*
     * Iterate through the renderer batches and draw them
     */
    for (i = 0; i < crb + 1; i++) {
        glBindTexture(GL_TEXTURE_2D, __fl_renderBatches[i].texture);
        glDrawArrays(GL_TRIANGLES, __fl_renderBatches[i].offset, 
                __fl_renderBatches[i].numVertices);
    }
}

/**
 * Clean up code.
 * Free the vertex array, the vertex buffer and delete the shader program
 */
FLAPI void flRendererDestroy() {
    glDeleteProgram(__fl_shader);
    glDeleteVertexArrays(1, &__fl_vao);
    glDeleteBuffers(1, &__fl_vbo);
}

#endif /* FL_IMPLEMENTATION  */

/*
--------------------------------------------------------------------------------
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
--------------------------------------------------------------------------------
*/
