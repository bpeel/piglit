/*
 * Copyright © 2017 Igalia S.L.
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
 * \file unsuccessful-recompile.c
 *
 * Render using a program with a uniform. Modify the uniform and then
 * do a recompile with an invalid shader source. This shouldn’t affect
 * the original program and it should render with the new uniform
 * value.
 */

#include "piglit-util-gl.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

        config.supports_gl_core_version = 31;
        config.window_visual = PIGLIT_GL_VISUAL_RGB | PIGLIT_GL_VISUAL_DOUBLE;

PIGLIT_GL_TEST_CONFIG_END

static const char vs_source[] =
        "#version 150\n"
        "in vec4 piglit_vertex;\n"
        "void main()\n"
        "{\n"
        "       gl_Position = piglit_vertex;\n"
        "}\n";

static const char fs_source[] =
        "#version 150\n"
        "uniform vec4 color;\n"
        "out vec4 color_out;\n"
        "void main()\n"
        "{\n"
        "       color_out = color;\n"
        "}\n";

static const float green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
static const float purple[] = { 0.5f, 0.0f, 0.5f, 1.0f };

static bool
try_render(const float *color)
{
        bool pass;

        piglit_draw_rect(-1, -1, 2, 2);

        if (!piglit_check_gl_error(GL_NO_ERROR)) {
                fprintf(stderr, "error while drawing\n");
                piglit_report_result(PIGLIT_FAIL);
        }

        pass = piglit_probe_rect_rgb(0, 0,
                                     piglit_width, piglit_height,
                                     color);
        if (!pass)
                fprintf(stderr, "render failed\n");

        return pass;
}

static GLuint
create_shader_program(GLenum type,
                      const char *source)
{
        GLuint prog;

        prog = glCreateShaderProgramv(type, 1, (const GLchar *const *) &source);

        piglit_link_check_status(prog);

        return prog;
}

static void
unsuccessful_compile(GLuint fs)
{
        char bad_source[sizeof fs_source];
        char *main_func;
        GLuint shader;
        GLint status;

        /* Create a shader source that shouldn’t compile */
        memcpy(bad_source, fs_source, sizeof fs_source);
        main_func = strstr(bad_source, "main");
        *main_func = 'p';

        shader = piglit_compile_shader_text(GL_FRAGMENT_SHADER, bad_source);

        glAttachShader(fs, shader);
        glLinkProgram(fs);

        glGetProgramiv(fs, GL_LINK_STATUS, &status);

        if (status) {
                fprintf(stderr, "Broken shader unexpectedly linked\n");
                piglit_report_result(PIGLIT_FAIL);
        }
}

enum piglit_result
piglit_display(void)
{
        bool pass = true;
        GLuint vs, fs, pipe;
        GLint color_uniform;

        glViewport(0, 0, piglit_width, piglit_height);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Create program pipeline. */
        glGenProgramPipelines(1, &pipe);
        glBindProgramPipeline(pipe);

        vs = create_shader_program(GL_VERTEX_SHADER, vs_source);
        pass = piglit_link_check_status(vs) && pass;

        fs = create_shader_program(GL_FRAGMENT_SHADER, fs_source);
        pass = piglit_link_check_status(fs) && pass;

        glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, vs);
        glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, fs);

        glBindProgramPipeline(pipe);
        glValidateProgramPipeline(pipe);

        if (!pass || !piglit_check_gl_error(GL_NO_ERROR)) {
                fprintf(stderr, "error building program/pipeline\n");
                piglit_report_result(PIGLIT_FAIL);
        }

        color_uniform = glGetUniformLocation(fs, "color");

        if (color_uniform == -1) {
                fprintf(stderr, "color uniform missing\n");
                piglit_report_result(PIGLIT_FAIL);
        }

        glProgramUniform4fv(fs, color_uniform, 1, green);

        if (!try_render(green))
                pass = false;

        glProgramUniform4fv(fs, color_uniform, 1, purple);

        unsuccessful_compile(fs);

        if (!try_render(purple))
                pass = false;

        piglit_present_results();

        return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
        piglit_require_gl_version(31);
        piglit_require_GLSL_version(150);
        piglit_require_extension("GL_ARB_separate_shader_objects");
}
