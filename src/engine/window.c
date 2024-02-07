#include "window.h"
#include "utils/utils.h"
#include <glad/glad.h>

keyinp_data_t keyinp;
float delta_time;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keyinp.is_hold = TRUE;
		else if (action == GLFW_RELEASE)
			keyinp.is_hold = FALSE;
		keyinp.key = key;
	}
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int window_init(GLFWwindow **window_p, char *name, unsigned int width, unsigned int height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	*window_p = glfwCreateWindow(width, height, name, NULL, NULL);
	ASSERT(*window_p, "Failed to create GLFW window\n");
	glfwMakeContextCurrent(*window_p);
	glfwSetFramebufferSizeCallback(*window_p, framebuffer_size_callback);
	glfwSetKeyCallback(*window_p, key_callback);
	ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD\n");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return TRUE;
}

int window_startgame(game_t *game_p, GLFWwindow *window)
{
	double last_frame;
	double last_sec_frame;
	int frame;
	
	frame = 0;
	delta_time = 0.f;
	last_frame = 0.f;
	last_sec_frame = glfwGetTime();
	game_p->fps = 0;
	while (!glfwWindowShouldClose(window))
	{
		double curr_frame = glfwGetTime();
		delta_time = curr_frame - last_frame;
		last_frame = curr_frame;

		if (curr_frame - last_sec_frame >= 1.f)
		{
			game_p->fps = frame;
			frame = 0;
			last_sec_frame = curr_frame;
		}
		else
			frame++;

		glfwPollEvents();
		if (!game_process_input(game_p, &keyinp, delta_time))
			return FALSE;
		if (!game_update(game_p, delta_time))
			return FALSE;
		glClearColor(0.f, 0.f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (!game_render(game_p))
			return FALSE;
		glfwSwapBuffers(window);
	}
	game_cleanup(game_p);
	return TRUE;
}