#include "piglit-util-gl.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

config.supports_gl_core_version = 33;

config.window_visual = PIGLIT_GL_VISUAL_DOUBLE | PIGLIT_GL_VISUAL_RGB;

PIGLIT_GL_TEST_CONFIG_END

static const GLfloat red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
static const GLfloat green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };

static const char
vertex_source[] =
	"#version 130\n"
	"\n"
	"in vec4 piglit_vertex;\n"
	"in vec2 piglit_texcoord;\n"
	"out vec2 texcoord;\n"
	"\n"
	"void\n"
	"main()\n"
	"{\n"
	"        gl_Position = piglit_vertex;\n"
	"        texcoord = piglit_texcoord;\n"
	"}\n";

static const char
fragment_source[] =
	"#version 130\n"
	"#extension GL_ARB_texture_multisample : require\n"
	"\n"
	"in vec2 texcoord;\n"
	"out vec4 color;\n"
	"uniform sampler2DMS tex;\n"
	"\n"
	"void\n"
	"main()\n"
	"{\n"
	"        color = texelFetch(tex, "
	"ivec2(texcoord * textureSize(tex)), 0);\n"
	"}\n";

static GLuint program;

enum piglit_result
piglit_display(void)
{
	bool pass = true;
	GLuint tex, fbo;

	piglit_present_results();

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 2,
				GL_RGBA, 16, 16, GL_FALSE);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glFramebufferTexture(GL_FRAMEBUFFER,
			     GL_COLOR_ATTACHMENT0,
			     tex,
			     0);

	glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_SCISSOR_TEST);
	glScissor(8, 0, 8, 16);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	glDeleteFramebuffers(1, &fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, piglit_winsys_fbo);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	piglit_draw_rect_tex(-1.0f, -1.0f,
			     16 * 2.0f / piglit_width,
			     16 * 2.0f / piglit_height,
			     0.0f, 0.0f, 1.0f, 1.0f);

	pass = piglit_probe_rect_rgb(0, 0, 8, 16, green) && pass;
	pass = piglit_probe_rect_rgb(8, 0, 8, 16, red) && pass;

	glDeleteTextures(1, &tex);

	piglit_present_results();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	GLuint tex;

	program = piglit_build_simple_program(vertex_source, fragment_source);
	glUseProgram(program);
	tex = glGetUniformLocation(program, "tex");
	glUniform1i(tex, 0);
}
