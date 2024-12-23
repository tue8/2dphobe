#ifndef GAME_H_
#define GAME_H_
#include <GLFW/glfw3.h>

#include "engine/text.h"
#include "engine/collision.h"

typedef struct
{
	int key;
	int is_held;
	int is_pressed;
} keyinp_data_t;

typedef struct
{
	int fps;
	int keys[1024];
	int width, height;
} phobe_game;

int game_init(phobe_game *, int, int);
int game_process_input(phobe_game *, keyinp_data_t *, float);
int game_update(phobe_game *, float);
int game_render(phobe_game *);
int game_cleanup(phobe_game *);

#endif