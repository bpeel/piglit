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

#include "piglit-util-gl.h"

/**
 * @file point-multi-element-vertex-id.c
 *
 * Tests gl_VertexID with multiple attributes to try and trigger a bug
 * in Mesa on BDW.
 */

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 20;

	config.window_visual = PIGLIT_GL_VISUAL_RGBA | PIGLIT_GL_VISUAL_DOUBLE;

PIGLIT_GL_TEST_CONFIG_END

static const char
vertex_shader[] =
	"#version 130\n"
	"#extension GL_ARB_explicit_attrib_location : require\n"
	"\n"
	"layout(location = 0) in vec2 piglit_vertex;\n"
	"layout(location = 1) in vec3 vertex_color;\n"
	"layout(location = 2) in vec2 vertex_offset;\n"
	"\n"
	"void\n"
	"main()\n"
	"{\n"
	"        gl_Position = vec4(piglit_vertex + vertex_offset, 0.0, 1.0);\n"
	"        gl_FrontColor = vec4(vertex_color, 1.0);\n"
	"}\n";

static const char
vertex_shader_instance_id[] =
	"#version 130\n"
	"#extension GL_ARB_explicit_attrib_location : require\n"
	"#extension GL_ARB_draw_instanced : require\n"
	"\n"
	"layout(location = 0) in vec2 piglit_vertex;\n"
	"layout(location = 1) in vec3 vertex_color;\n"
	"layout(location = 2) in vec2 vertex_offset;\n"
	"\n"
	"void\n"
	"main()\n"
	"{\n"
	"        gl_Position = vec4(piglit_vertex + vertex_offset, 0.0, 1.0);\n"
	"        gl_Position.y -= 1.0;\n"
	"        gl_FrontColor = vec4(vertex_color.rg,\n"
	"                             float(gl_InstanceID),\n"
	"                             1.0);\n"
	"}\n";

struct vertex {
	float x, y;
};

struct instance {
	float r, g, b;
	float x, y;
};

static const struct vertex
vertices[] = {
	/* Triangle strip forming a square in the top-left quadrant */
	{ -1, 0 },
	{ 0, 0 },
	{ -1, 1 },
	{ 0, 1 }
};

static const struct instance
instances[] = {
	/* Red square on the left */
	{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
	/* Blue square on the left */
	{ 0.0f, 0.0f, 1.0f, 1.0f, 0.0f }
};

static const GLfloat
red[] = { 1.0f, 0.0f, 0.0f };
static const GLfloat
blue[] = { 0.0f, 0.0f, 1.0f };

enum piglit_result
piglit_display(void)
{
	GLuint vbo, ibo;
	GLuint program_normal, program_iid;
	bool pass = true;

	program_normal =
		piglit_build_simple_program(vertex_shader, NULL);
	program_iid =
		piglit_build_simple_program(vertex_shader_instance_id, NULL);

	glClear(GL_COLOR_BUFFER_BIT);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
		     sizeof vertices,
		     vertices,
		     GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, /* location */
			      2, /* size */
			      GL_FLOAT,
			      GL_FALSE, /* normalized */
			      sizeof (struct vertex),
			      (void *) (intptr_t)
			      offsetof(struct vertex, x));

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ARRAY_BUFFER, ibo);
	glBufferData(GL_ARRAY_BUFFER,
		     sizeof instances,
		     instances,
		     GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, /* location */
			      3, /* size */
			      GL_FLOAT,
			      GL_FALSE, /* normalized */
			      sizeof (struct instance),
			      (void *) (intptr_t)
			      offsetof(struct instance, r));
	glVertexAttribDivisor(1, 1);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, /* location */
			      2, /* size */
			      GL_FLOAT,
			      GL_FALSE, /* normalized */
			      sizeof (struct instance),
			      (void *) (intptr_t)
			      offsetof(struct instance, x));
	glVertexAttribDivisor(2, 1);

	glUseProgram(program_normal);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP,
			      0, /* first */
			      ARRAY_SIZE(vertices),
			      2 /* primcount */);

	glUseProgram(program_iid);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP,
			      0, /* first */
			      ARRAY_SIZE(vertices),
			      2 /* primcount */);

	glUseProgram(0);
	glDeleteProgram(program_iid);
	glDeleteProgram(program_normal);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);

	pass = piglit_probe_rect_rgb(0, 0,
				     piglit_width / 2, piglit_height,
				     red) && pass;
	pass = piglit_probe_rect_rgb(piglit_width / 2, 0,
				     piglit_width / 2, piglit_height,
				     blue) && pass;

	piglit_present_results();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	piglit_require_GLSL_version(130);
	piglit_require_extension("GL_ARB_explicit_attrib_location");
	piglit_require_extension("GL_ARB_draw_instanced");
	piglit_require_extension("GL_ARB_instanced_arrays");
}
