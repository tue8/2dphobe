#include "phobe.h"
#include <stdio.h>
#include "utils/utils.h"
#include <glad/glad.h>

phobe_context *context_ptr = NULL;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key >= 0 && key < 1024)
	{
		context_ptr->queued_key = key;
		if (action == GLFW_PRESS)
		{
			context_ptr->input[key].is_held = TRUE;
			context_ptr->input[key].is_pressed = TRUE;
		}
		else if (action == GLFW_RELEASE)
			context_ptr->input[key].is_held = FALSE;
	}
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int phobe_init(phobe_context *context, char *name, unsigned int width, unsigned int height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	context->window = glfwCreateWindow(width, height, name, NULL, NULL);
	ASSERT(context->window, "Failed to create GLFW window\n");
	glfwMakeContextCurrent(context->window);
	glfwSetFramebufferSizeCallback(context->window, framebuffer_size_callback);
	glfwSetKeyCallback(context->window, key_callback);
	ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD\n");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSwapInterval(0);
	context_ptr = context->window;

	context->last_frame = 0.f;
	context->last_sec_frame = (float)glfwGetTime();
	context->frame = 0;
	context->fps = 0;
	context->delta_time = 0.f;
	context->queued_key = -1;
	return TRUE;
}

int phobe_shouldclose(phobe_context *context)
{
	return glfwWindowShouldClose(context->window);
}

void phobe_startloop(phobe_context *context)
{
	float curr_frame = (float)glfwGetTime();
	context->delta_time = curr_frame - context->last_frame;
	context->last_frame = curr_frame;

	if (curr_frame - context->last_sec_frame >= 1.f)
	{
		context->fps = context->frame;
		context->frame = 0;
		context->last_sec_frame = curr_frame;
	}
	else
		context->frame++;

	glfwPollEvents();
}

void phobe_endloop(phobe_context *context)
{
	if (context->queued_key != -1)
		context->input[context->queued_key].is_pressed = FALSE;
	glfwSwapBuffers(context->window);
}

int phobe_key_pressed(phobe_context *context, int key)
{
	return context->input[key].is_pressed;
}

int phobe_key_held(phobe_context *context, int key)
{
	return context->input[key].is_held;
}