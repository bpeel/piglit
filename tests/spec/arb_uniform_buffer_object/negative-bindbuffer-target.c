/*
 * Copyright © 2012 Intel Corporation
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

/** @file negative-bindbuffer-target.c
 *
 * Tests for errors when binding with a bad target.
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 10;
	config.window_visual = PIGLIT_GL_VISUAL_RGBA | PIGLIT_GL_VISUAL_DOUBLE;

PIGLIT_GL_TEST_CONFIG_END

void
piglit_init(int argc, char **argv)
{
	/* We don't need to check extensions for these targets, since
	 * we're expecting INVALID_ENUM anyway.
	 */
	GLenum targets[] = {
		0xd0d0d0d0,
		GL_ARRAY_BUFFER,
		GL_ELEMENT_ARRAY_BUFFER,
		GL_COPY_READ_BUFFER,
		GL_COPY_WRITE_BUFFER,
		GL_PIXEL_PACK_BUFFER,
		GL_PIXEL_UNPACK_BUFFER,
		GL_TEXTURE_BUFFER
	};
	bool pass = true;
	int i;
	GLuint bo;

	piglit_require_extension("GL_ARB_uniform_buffer_object");

	glGenBuffers(1, &bo);
	glBindBuffer(GL_UNIFORM_BUFFER, bo);
	glBufferData(GL_UNIFORM_BUFFER, 1, NULL, GL_STATIC_READ);

	/* From the GL_ARB_uniform_buffer_object spec:
	 *
	 *      "The error INVALID_ENUM is generated by
	 *       BindBufferRange and BindBufferBase if <target> is not
	 *       an accepted indexable buffer object target."
	 */
	for (i = 0; i < ARRAY_SIZE(targets); i++) {
		glBindBufferBase(targets[i], 0, bo);
		if (!piglit_check_gl_error(GL_INVALID_ENUM)) {
			printf("Fail on target 0x%08x (%s)\n",
			       targets[i],
			       piglit_get_gl_enum_name(targets[i]));
			pass = false;
		}

		glBindBufferRange(targets[i], 0, bo, 0, 1);
		if (!piglit_check_gl_error(GL_INVALID_ENUM)) {
			printf("Fail on target 0x%08x (%s)\n",
			       targets[i],
			       piglit_get_gl_enum_name(targets[i]));
			pass = false;
		}
	}

	piglit_report_result(pass ? PIGLIT_PASS : PIGLIT_FAIL);
}

enum piglit_result piglit_display(void)
{
	/* UNREACHED */
	return PIGLIT_FAIL;
}
