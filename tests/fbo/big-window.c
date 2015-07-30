/*
 * Copyright © 2009 Intel Corporation
 * Copyright (c) 2010 VMware, Inc.
 * Copyright © 2011 Red Hat Inc.
 * Copyright © 2014 Advanced Micro Devices, Inc.
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

/* GL3 requirement. */
#define MAX_DIM 8192

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 33;
	config.supports_gl_core_version = 33;

	config.window_width = MAX_DIM;
	config.window_height = MAX_DIM;

	config.window_visual = PIGLIT_GL_VISUAL_DOUBLE | PIGLIT_GL_VISUAL_RGB;

PIGLIT_GL_TEST_CONFIG_END

enum piglit_result
piglit_display(void)
{
	glClearColor(0.2, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	return PIGLIT_PASS;
}

void piglit_init(int argc, char **argv)
{
	if (piglit_get_gl_version() < 33) {
		piglit_report_result(PIGLIT_SKIP);
	}
}
