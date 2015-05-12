#include "piglit-util-gl.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

config.supports_gl_core_version = 33;

config.window_visual = PIGLIT_GL_VISUAL_DOUBLE | PIGLIT_GL_VISUAL_RGB;

PIGLIT_GL_TEST_CONFIG_END

static const GLfloat red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
static const GLfloat green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };

#define TEX_WIDTH 128
#define TEX_HEIGHT 128

enum piglit_result
piglit_display(void)
{
	bool pass = true;
	GLuint rb, fbo;

	piglit_present_results();

	glGenRenderbuffers(1, &rb);
	glBindRenderbuffer(GL_RENDERBUFFER, rb);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER,
					 2, /* samples */
					 GL_RGBA,
					 TEX_WIDTH, TEX_HEIGHT);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
				  GL_COLOR_ATTACHMENT0,
				  GL_RENDERBUFFER,
				  rb);

	glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_SCISSOR_TEST);
	glScissor(TEX_WIDTH / 2, 0, TEX_WIDTH / 2, TEX_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, piglit_winsys_fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlitFramebuffer(0, 0, TEX_WIDTH, TEX_HEIGHT,
			  0, 0, TEX_WIDTH, TEX_HEIGHT,
			  GL_COLOR_BUFFER_BIT,
			  GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, piglit_winsys_fbo);

	pass = piglit_probe_rect_rgb(0, 0,
				     TEX_WIDTH / 2, TEX_HEIGHT,
				     green) && pass;
	pass = piglit_probe_rect_rgb(TEX_WIDTH / 2, 0,
				     TEX_WIDTH / 2, TEX_HEIGHT,
				     red) && pass;

	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &rb);

	piglit_present_results();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
}
