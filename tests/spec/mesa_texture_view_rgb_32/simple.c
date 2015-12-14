/*
 * Copyright © 2015 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
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
 */

/**
 * Creates an RGB8 format texture and then creates a view to it in
 * RGBA8 format and checks that it has the same colours.
 */

#include "piglit-util-gl.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 20;

	config.window_visual = PIGLIT_GL_VISUAL_RGBA | PIGLIT_GL_VISUAL_DOUBLE;

PIGLIT_GL_TEST_CONFIG_END

static const char
vertex_source[] =
	"attribute vec4 piglit_vertex;\n"
	"attribute vec2 piglit_texcoord;\n"
	"varying vec2 texcoord;\n"
	"\n"
	"void\n"
	"main()\n"
	"{\n"
	"        gl_Position = piglit_vertex;\n"
	"        texcoord = piglit_texcoord;\n"
	"}\n";

static const char
fragment_source[] =
	"uniform sampler2D tex;\n"
	"varying vec2 texcoord;\n"
	"\n"
	"void\n"
	"main()\n"
	"{\n"
	"        gl_FragColor = vec4(texture2D(tex, texcoord).rgb, 1.0);\n"
	"}\n";

static float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
static float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
static float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
static float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

enum piglit_result
piglit_display(void)
{
	bool pass = true;
	GLuint tex, view;
	GLubyte *data;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexStorage2D(GL_TEXTURE_2D,
		       1, /* levels */
		       GL_RGB8,
		       piglit_width, piglit_height);

	data = piglit_rgbw_image_ubyte(piglit_width, piglit_height,
				       GL_FALSE /* alpha */);
	glTexSubImage2D(GL_TEXTURE_2D,
			0, /* level */
			0, 0, /* x/y offset*/
			piglit_width, piglit_height,
			GL_RGBA, GL_UNSIGNED_BYTE,
			data);
	free(data);

	glGenTextures(1, &view);
	glTextureView(view,
		      GL_TEXTURE_2D,
		      tex,
		      GL_RGBA8,
		      0, /* minlevel */
		      1, /* numlevels */
		      0, /* minlayer */
		      1 /* numlayers */);
	glBindTexture(GL_TEXTURE_2D, view);

	pass = piglit_check_gl_error(GL_NO_ERROR) && pass;

	piglit_draw_rect_tex(-1, -1, /* x/y */
			     2, 2, /* w/h */
			     0, 0, /* tx/ty */
			     1, 1 /* tw/th */);

	glDeleteTextures(1, &tex);
	glDeleteTextures(1, &view);

	pass = piglit_probe_rect_rgba(0, 0,
				      piglit_width / 2, piglit_height / 2,
				      red) && pass;
	pass = piglit_probe_rect_rgba(piglit_width / 2, 0,
				      piglit_width / 2, piglit_height / 2,
				      green) && pass;
	pass = piglit_probe_rect_rgba(0, piglit_height / 2,
				      piglit_width / 2, piglit_height / 2,
				      blue) && pass;
	pass = piglit_probe_rect_rgba(piglit_width / 2, piglit_height / 2,
				      piglit_width / 2, piglit_height / 2,
				      white) && pass;

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	GLuint program;
	GLint tex_loc;

	piglit_require_extension("GL_MESA_texture_view_rgb_32");
	piglit_require_extension("GL_ARB_texture_view");
	piglit_require_extension("GL_ARB_texture_storage");

	program = piglit_build_simple_program(vertex_source, fragment_source);

	tex_loc = glGetUniformLocation(program, "tex");
	glUseProgram(program);
	glUniform1i(tex_loc, 0);
}
