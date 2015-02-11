/*
 * Copyright (c) 2015 Intel Corporation
 *
 * Permission is hereby , free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *
 * Test using 1D array texture with mipmaps
 */


#include "piglit-util-gl.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 10;

	config.window_width = 700;
	config.window_height = 400;
	config.window_visual = PIGLIT_GL_VISUAL_DOUBLE | PIGLIT_GL_VISUAL_RGB;

PIGLIT_GL_TEST_CONFIG_END

#define TEX_WIDTH 16
#define TEX_DEPTH 16

static const char
vertex_shader[] =
	"attribute vec2 piglit_vertex;\n"
	"attribute vec2 piglit_texcoord;\n"
	"varying vec2 texcoord;\n"
	"\n"
	"void\n"
	"main(void)\n"
	"{\n"
	"        gl_Position = vec4(piglit_vertex, 0.0, 1.0);\n"
	"        texcoord = piglit_texcoord;\n"
	"}\n";

static const char
fragment_shader[] =
	"#extension GL_EXT_texture_array : require\n"
	"varying vec2 texcoord;\n"
	"uniform sampler1DArray tex;\n"
	"\n"
	"void\n"
	"main(void)\n"
	"{\n"
	"        gl_FragColor = texture1DArray(tex, texcoord);\n"
	"}\n";

static GLuint
generate_texture(void)
{
	uint8_t data[TEX_WIDTH * TEX_DEPTH * 4];
	uint8_t *p;
	int width = TEX_WIDTH;
	int level = 0;
	int levels;
	int x, y;
	GLuint tex;

	levels = ffs(TEX_WIDTH);
	assert(1 << (levels - 1) == TEX_WIDTH);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_1D_ARRAY, tex);

	while (width >= 1) {
		p = data;

		for (y = 0; y < TEX_DEPTH; y++) {
			for (x = 0; x < width; x++) {
				*(p++) = x < width / 2 ? 0 : 255;
				*(p++) = y * 255 / (TEX_DEPTH - 1);
				*(p++) = level * 255 / (levels - 1);
				*(p++) = 255;
			}
		}

		glTexImage2D(GL_TEXTURE_1D_ARRAY,
			     level,
			     GL_RGBA,
			     width, TEX_DEPTH,
			     0, /* border */
			     GL_RGBA, GL_UNSIGNED_BYTE,
			     data);

		level++;
		width /= 2;
	}

	glTexParameteri(GL_TEXTURE_1D_ARRAY,
			GL_TEXTURE_MIN_FILTER,
			GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_1D_ARRAY,
			GL_TEXTURE_MAG_FILTER,
			GL_NEAREST);

	return tex;
}

static void
render_texture(void)
{
	GLint program;
	int width = TEX_WIDTH;
	GLint tex_location;
	int x = 0, y;

	program = piglit_build_simple_program(vertex_shader,
					      fragment_shader);

	glUseProgram(program);
	tex_location = glGetUniformLocation(program, "tex");
	glUniform1i(tex_location, 0);

	while (width >= 1) {
		for (y = 0; y < TEX_DEPTH; y++) {
			piglit_draw_rect_tex(x * 2.0f / piglit_width - 1.0f,
					     y * 2.0f / piglit_height - 1.0f,
					     width * 2.0f / piglit_width,
					     2.0f / piglit_height,
					     0.0f, y, /* tx/ty */
					     1.0f, 0.0f /* tw/th */);
		}

		x += width;
		width /= 2;
	}

	glUseProgram(0);
	glDeleteProgram(program);
}

static bool
check_texture(void)
{
	int width = TEX_WIDTH;
	int level = 0;
	int levels;
	int x = 0, y;
	bool pass = true;
	bool layer_pass;
	GLfloat color[4];

	levels = ffs(TEX_WIDTH);

	while (width >= 2) {
		for (y = 0; y < TEX_DEPTH; y++) {
			printf("Level %i layer %i: ", level, y);

			color[0] = 0.0f;
			color[1] = y / (TEX_DEPTH - 1.0f);
			color[2] = level / (levels - 1.0f);
			color[3] = 255.0f;

			layer_pass = piglit_probe_rect_rgb(x, y,
							   width / 2,
							   1, /* height */
							   color);

			color[0] = 1.0f;

			layer_pass = piglit_probe_rect_rgb(x + width / 2, y,
							   width / 2,
							   1, /* height */
							   color) && layer_pass;

			printf("%s\n", layer_pass ? "pass" : "FAIL");

			pass = pass && layer_pass;
		}

		x += width;
		width /= 2;
		level++;
	}

	return pass;
}

enum piglit_result
piglit_display(void)
{
	bool pass = true;
	GLuint tex;

	tex = generate_texture();
	render_texture();
	pass = check_texture() && pass;

	glDeleteTextures(1, &tex);

	piglit_present_results();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}


void
piglit_init(int argc, char **argv)
{
	piglit_require_extension("GL_EXT_texture_array");
}
