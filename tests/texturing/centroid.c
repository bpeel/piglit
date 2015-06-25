#include "piglit-util-gl.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_core_version = 33;

	config.window_visual = PIGLIT_GL_VISUAL_RGB | PIGLIT_GL_VISUAL_DOUBLE;

PIGLIT_GL_TEST_CONFIG_END

#define FB_WIDTH 4
#define FB_HEIGHT 4

static const char
vertex_source[] =
	"#version 330\n"
	"\n"
	"in vec4 piglit_vertex;\n"
	"out vec4 thing;\n"
	"\n"
	"void\n"
	"main()\n"
	"{\n"
	"        gl_Position = thing = piglit_vertex;\n"
	"}\n";

static const char
fragment_source[] =
	"#version 330\n"
	"#extension GL_ARB_gpu_shader5 : require\n"
	"\n"
	"in vec4 thing;\n"
	"layout(location = 0) out vec4 out_color;\n"
	"\n"
	"void\n"
	"main()\n"
	"{\n"
	"        out_color = interpolateAtCentroid(thing);\n"
	"}\n";

enum piglit_result
piglit_display(void)
{
	GLboolean pass = GL_TRUE;

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	GLuint program;
	GLuint fbo, rb;
	float buf[FB_WIDTH * FB_HEIGHT * 4], *p;
	int y, x;

	program = piglit_build_simple_program(vertex_source, fragment_source);
	glUseProgram(program);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenRenderbuffers(1, &rb);
	glBindRenderbuffer(GL_RENDERBUFFER, rb);
	glRenderbufferStorage(GL_RENDERBUFFER,
			      GL_RGBA32F,
			      FB_WIDTH, FB_HEIGHT);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
				  GL_COLOR_ATTACHMENT0,
				  GL_RENDERBUFFER,
				  rb);

	glViewport(0, 0, FB_WIDTH, FB_HEIGHT);

	piglit_draw_rect(-1, -1, 2, 2);

	glReadPixels(0, 0, FB_WIDTH, FB_HEIGHT, GL_RGBA, GL_FLOAT, buf);

	p = buf;

	for (y = 0; y < FB_HEIGHT; y++) {
		for (x = 0; x < FB_WIDTH; x++) {
			printf("%f,%f,%f,%f ",
			       p[0], p[1], p[2], p[3]);
			p += 4;
		}
		fputc('\n', stdout);
	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rb);
}
