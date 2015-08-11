/*
 * Copyright (c) 2015 Intel Corporation
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

#include "piglit-util-gl.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_core_version = 33;

	config.window_visual = PIGLIT_GL_VISUAL_DOUBLE | PIGLIT_GL_VISUAL_RGB;

PIGLIT_GL_TEST_CONFIG_END

#define N_TESTS 128
#define STR(x) #x
#define STRINGIFY(x) STR(x)
#define N_TESTS_S STRINGIFY(N_TESTS)
static const char
vertex_shader_source[] =
	"#version 330\n"
	"in vec2 piglit_vertex;\n"
	"\n"
	"void\n"
	"main()\n"
	"{\n"
	"        gl_Position = vec4(piglit_vertex, 0.0, 1.0);\n"
	"}\n";

static const char
fragment_shader_source[] =
	"#version 330\n"
	"layout(location = 0) out int result;\n"
	"uniform isampler2D tex;\n"
	"uniform int other_int;\n"
	"\n"
	"void\n"
	"main()\n"
	"{\n"
	"        int test_num = int(gl_FragCoord.x);\n"
	"        ivec2 src = texelFetch(tex, ivec2(test_num, 0), 0).rg;\n"
	"        result = src.r * other_int;\n"
	"}\n";

enum piglit_result
piglit_display(void)
{
	return PIGLIT_FAIL;
}

void
piglit_init(int argc, char**argv)
{
	GLuint fbo, rb;
	GLuint tex;
	GLuint prog;
	GLuint uniform_location;
	int src_buf[N_TESTS * 2];
	int results[N_TESTS];
	bool pass = true;
	const char *note;
	int other_int;
	int i;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenRenderbuffers(1, &rb);
	glBindRenderbuffer(GL_RENDERBUFFER, rb);
	glRenderbufferStorage(GL_RENDERBUFFER,
			      GL_R32I,
			      N_TESTS, 1);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
				  GL_COLOR_ATTACHMENT0,
				  GL_RENDERBUFFER,
				  rb);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
	    GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "fbo incomplete\n");
		piglit_report_result(PIGLIT_FAIL);
	}

	other_int = (rand() & 0xffff) | (rand() << 16);

	for (i = 0; i < N_TESTS; i++) {
		src_buf[i * 2] = (rand() & 0xffff) | (rand() << 16);
		src_buf[i * 2 + 1] = other_int;
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,
		     0, /* level */
		     GL_RG32I,
		     N_TESTS, 1, /* width/height */
		     0, /* border */
		     GL_RG_INTEGER, GL_INT,
		     src_buf);

	prog = piglit_build_simple_program(vertex_shader_source,
					   fragment_shader_source);
	glUseProgram(prog);
	uniform_location = glGetUniformLocation(prog, "tex");
	glUniform1i(uniform_location, 0);
	uniform_location = glGetUniformLocation(prog, "other_int");
	glUniform1i(uniform_location, other_int);

	glClear(GL_COLOR_BUFFER_BIT);
	piglit_draw_rect(-1, -1, 2, 2);

	glReadPixels(0, 0, N_TESTS, 1,
		     GL_RED_INTEGER, GL_INT, results);

	for (i = 0; i < N_TESTS; i++) {
		if (src_buf[i * 2] * src_buf[i * 2 + 1] == results[i]) {
			note = "pass";
		} else {
			note = "FAIL";
			pass = false;
		}

		printf("%i * %i = %i %s\n",
		       src_buf[i * 2],
		       src_buf[i * 2 + 1],
		       results[i],
		       note);

	}

	piglit_report_result(pass ? PIGLIT_PASS : PIGLIT_FAIL);
}

