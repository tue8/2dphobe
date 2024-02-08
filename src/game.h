#ifndef GAME_H_
#define GAME_H_
#include <GLFW/glfw3.h>

#include "engine/text.h"
#include "engine/collision.h"

#include "ball.h"
#include "level.h"

typedef struct
{
	int is_held;
	int is_pressed;
} keyinp_data_t;

typedef struct
{
	int fps;
	int keys[1024];
	int width, height;
} game_t;

/***/

typedef enum
{
	GAME_ACTIVE,
	GAME_START,
	GAME_WIN
} game_state;

typedef struct
{
	float player_width;
	float player_height;
	float player_speed;
	float player_pos_x;
	float player_pos_y;
	float shoot_dir;
} g_player_data;

typedef struct
{
	float ball_speed;
} g_ball_data;

typedef struct
{
	float char_size;
	float char_margin;
} g_text_data;

typedef struct
{
	game_state state;

	GLFWwindow *window;
	renderer_t renderer;
	obj_t player;
	ball_t ball;
	level_t level;

	g_player_data player_data;
	g_ball_data ball_data;

	collision_data *bricks_col_data;
	collision_data player_col_data;

	g_text_data text_data;
} game_data;

/***/

int game_init(game_t *, char *, int, int);
int game_process_input(game_t *, keyinp_data_t *, float);
int game_update(game_t *, float);
int game_render(game_t *);
int game_cleanup(game_t *);

#endif