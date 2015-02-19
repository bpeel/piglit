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
 * Test using a 3D texture with mipmaps
 */


#include "piglit-util-gl.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 11;

	config.window_width = 700;
	config.window_height = 400;
	config.window_visual = PIGLIT_GL_VISUAL_DOUBLE | PIGLIT_GL_VISUAL_RGB;

PIGLIT_GL_TEST_CONFIG_END

#define TEX_WIDTH 16
#define TEX_HEIGHT 32
#define TEX_DEPTH 16
#define TEX_MAX MAX2(MAX2(TEX_WIDTH, TEX_HEIGHT), TEX_DEPTH)

static GLuint
generate_texture(void)
{
	uint8_t data[TEX_WIDTH * TEX_HEIGHT * TEX_DEPTH * 4];
	uint8_t *p;
	int width = TEX_WIDTH, height = TEX_HEIGHT, depth = TEX_DEPTH;
	int level = 0;
	int levels;
	int x, y, z;
	GLuint tex;

	levels = ffs(TEX_MAX);
	assert(1 << (levels - 1) == TEX_MAX);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_3D, tex);

	while (true) {
		p = data;

		for (z = 0; z < depth; z++) {
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					*(p++) = x < width / 2 ? 0 : 255;
					*(p++) = y < height / 2 ? 0 : 255;
					if (depth <= 1)
						*(p++) = 255;
					else
						*(p++) = z * 255 / (depth - 1);
					*(p++) = level * 255 / (levels - 1);
				}
			}
		}

		glTexImage3D(GL_TEXTURE_3D,
			     level,
			     GL_RGBA,
			     width, height, depth,
			     0, /* border */
			     GL_RGBA, GL_UNSIGNED_BYTE,
			     data);

		if (width == 1 && height == 1 && depth == 1)
			break;

		width = MAX2(width / 2, 1);
		height = MAX2(height / 2, 1);
		depth = MAX2(depth / 2, 1);
		level++;
	}

	glTexParameteri(GL_TEXTURE_3D,
			GL_TEXTURE_MIN_FILTER,
			GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_3D,
			GL_TEXTURE_MAG_FILTER,
			GL_NEAREST);

	return tex;
}

/**
 * Convenience function to draw an axis-aligned rectangle with 3-dimensional
 * texture coordinates where the third coordinate is constant.
 */
static GLvoid
draw_rect_tex_3d(float x, float y, float w, float h,
                 float tx, float ty, float tz, float tw, float th)
{
	float verts[4][4];
	float tex[4][3];

	verts[0][0] = x;
	verts[0][1] = y;
	verts[0][2] = 0.0;
	verts[0][3] = 1.0;
	tex[0][0] = tx;
	tex[0][1] = ty;
	tex[0][2] = tz;
	verts[1][0] = x + w;
	verts[1][1] = y;
	verts[1][2] = 0.0;
	verts[1][3] = 1.0;
	tex[1][0] = tx + tw;
	tex[1][1] = ty;
	tex[1][2] = tz;
	verts[2][0] = x;
	verts[2][1] = y + h;
	verts[2][2] = 0.0;
	verts[2][3] = 1.0;
	tex[2][0] = tx;
	tex[2][1] = ty + th;
	tex[2][2] = tz;
	verts[3][0] = x + w;
	verts[3][1] = y + h;
	verts[3][2] = 0.0;
	verts[3][3] = 1.0;
	tex[3][0] = tx + tw;
	tex[3][1] = ty + th;
	tex[3][2] = tz;

	glVertexPointer(4, GL_FLOAT, 0, verts);
	glTexCoordPointer(3, GL_FLOAT, 0, tex);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

static void
render_texture(void)
{
	int width = TEX_WIDTH, height = TEX_HEIGHT, depth = TEX_DEPTH;
	int x = 0, y = 0, max_height = 0, z;

	glEnable(GL_TEXTURE_3D);

	while (true) {
		for (z = 0; z < depth; z++) {
			if (x + width > piglit_width) {
				x = 0;
				y += max_height;
				max_height = 0;
			}
			draw_rect_tex_3d(x * 2.0f / piglit_width - 1.0f,
					 y * 2.0f / piglit_height - 1.0f,
					 width * 2.0f / piglit_width,
					 height * 2.0f / piglit_height,
					 0.0f, 0.0f, /* tx/ty */
					 (z + 0.5f) / depth,
					 1.0f, 1.0f /* tw/th */);
			x += width;
			if (height > max_height)
				max_height = height;
		}

		if (width == 1 && height == 1 && depth == 1)
			break;

		width = MAX2(width / 2, 1);
		height = MAX2(height / 2, 1);
		depth = MAX2(depth / 2, 1);
	}
}

static bool
check_texture(void)
{
	int width = TEX_WIDTH, height = TEX_HEIGHT, depth = TEX_DEPTH;
	int x = 0, y = 0, max_height = 0, z;
	int level = 0;
	int levels;
	bool pass = true;
	bool layer_pass;
	GLfloat color[4];

	levels = ffs(TEX_MAX);

	while (width >= 2 && height >= 2) {
		for (z = 0; z < depth; z++) {
			if (x + width > piglit_width) {
				x = 0;
				y += max_height;
				max_height = 0;
			}

			printf("Level %i z %i at %ix%i+%i+%i: ",
			       level, z,
			       width, height, x, y);

			color[0] = 0.0f;
			color[1] = 0.0f;
			color[2] = z / (depth - 1.0f);
			color[3] = level / (levels - 1.0f);

			layer_pass = piglit_probe_rect_rgba(x, y,
							    width / 2,
							    height / 2,
							    color);

			color[0] = 1.0f;

			layer_pass &= piglit_probe_rect_rgba(x + width / 2, y,
							     width / 2,
							     height / 2,
							     color);

			color[0] = 0.0f;
			color[1] = 1.0f;

			layer_pass &= piglit_probe_rect_rgba(x, y + height / 2,
							     width / 2,
							     height / 2,
							     color);

			color[0] = 1.0f;

			layer_pass &= piglit_probe_rect_rgba(x + width / 2,
							     y + height / 2,
							     width / 2,
							     height / 2,
							     color);

			printf("%s\n", layer_pass ? "pass" : "FAIL");

			pass = pass && layer_pass;

			x += width;
			if (height > max_height)
				max_height = height;
		}

		if (width == 1 && height == 1 && depth == 1)
			break;

		width = MAX2(width / 2, 1);
		height = MAX2(height / 2, 1);
		depth = MAX2(depth / 2, 1);
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
