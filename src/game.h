#ifndef GAME_H_
#define GAME_H_
#include <GLFW/glfw3.h>

#include "engine/text.h"
#include "engine/collision.h"

typedef struct
{
	int key;
	int is_hold;
} keyinp_data_t;

typedef struct
{
	int fps;
	int keys[1024];
	int width, height;
} game_t;

/***/

typedef struct
{
	GLFWwindow *window;
	renderer_t renderer;
} game_data;

/***/

int game_init(game_t *, int, int);
int game_process_input(game_t *, keyinp_data_t *, float);
int game_update(game_t *, float);
int game_render(game_t *);
int game_cleanup(game_t *);

#endif