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
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * \file fast-clear-rgb.c
 *
 * Clears a multisample RGB texture to 1,1,1,0 and then samples from
 * it in a shader to ensure that the alpha channel gets overriden to
 * 1. The reason for this test is that there is a bug in the i965
 * implementation of fast clears where the fast clear color doesn't
 * take into account the available components in the render target.
 * When it is later sampled it will read the fast clear color value
 * for the alpha instead of overriding it to 1.
 */

#include "piglit-util-gl.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_core_version = 31;
	config.window_visual = PIGLIT_GL_VISUAL_RGBA | PIGLIT_GL_VISUAL_DOUBLE;

PIGLIT_GL_TEST_CONFIG_END

static const char
vertex_source[] =
	"#version 140\n"
	"\n"
	"in vec4 piglit_vertex;\n"
	"\n"
	"void\n"
	"main()\n"
	"{\n"
	"	 gl_Position = piglit_vertex;\n"
	"}\n";

static const char
fragment_source[] =
	"#version 140\n"
	"#extension GL_ARB_texture_multisample : require\n"
	"\n"
	"uniform sampler2DMS tex;\n"
	"out vec4 color_out;\n"
	"\n"
	"void\n"
	"main()\n"
	"{\n"
	"	 color_out = texelFetch(tex, ivec2(0), 0);\n"
	"}\n";

enum piglit_result
piglit_display()
{
	/* Although we cleared the texture to 1,1,1,0, the alpha
	 * channel should end up as 1.0 when sampled.
	 */
	static const GLfloat expected_color[] = {
		1.0, 1.0, 1.0, 1.0
	};
	bool pass = true;

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	piglit_draw_rect(-1, -1, 2, 2);
	pass = piglit_probe_rect_rgba(0, 0, piglit_width, piglit_height,
				      expected_color) && pass;

	piglit_present_results();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	GLuint prog;
	GLint tex_location;
	GLuint tex;
	GLuint fbo;

	piglit_require_extension("GL_ARB_texture_multisample");

	prog = piglit_build_simple_program(vertex_source, fragment_source);
	glUseProgram(prog);
	tex_location = glGetUniformLocation(prog, "tex");
	glUniform1i(tex_location, 0);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
				1, /* samples */
				GL_RGB,
				1, 1, /* width/height */
				GL_FALSE /* fixed sample locations */);
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture(GL_FRAMEBUFFER,
			     GL_COLOR_ATTACHMENT0,
			     tex,
			     0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
	    GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "texture FBO not complete\n");
		piglit_report_result(PIGLIT_SKIP);
	}

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, piglit_winsys_fbo);
	glDeleteFramebuffers(1, &fbo);
}
