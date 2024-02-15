#include "game.h"
#include "utils/utils.h"
#include <string.h>
#include <memory.h>

/* a desperate attempt to save my code from turning into a pile of shit */
#define game_data game_p->data

const char *ball_tex_dir = "\\Breakout\\data\\textures\\ball.png";
const char *non_solid_tex_dir = "\\Breakout\\data\\textures\\non_solid.png";
const char *solid_tex_dir = "\\Breakout\\data\\textures\\solid.png";
const char *paddle_tex_dir = "\\Breakout\\data\\textures\\paddle.png";

const char *lvl_dir = "\\Breakout\\data\\levels\\default.lvl";

static void init_ball_data(game_t *game_p)
{
	game_data.ball.radius = 30.f;
	game_data.ball_data.ball_speed = 300.f;
	game_data.ball.v_velocity[v_x] = game_data.ball_data.ball_speed;
	game_data.ball.v_velocity[v_y] = -game_data.ball_data.ball_speed;
	game_data.ball.stucked = TRUE;
}

static void init_player_data(game_t *game_p, int width, int height)
{
	game_data.player_data.player_width = 100.f;
	game_data.player_data.player_height = 20.f;
	game_data.player_data.player_speed = 400.f;
	game_data.player_data.player_pos_x = (float)width / 2.f - game_data.player_data.player_width / 2.f;
	game_data.player_data.player_pos_y = (float)height - game_data.player_data.player_height;
	game_data.player_data.shoot_dir = TRUE; /* true = right, false = left */
}

static void init_collision_data(game_t *game_p)
{
	game_data.bricks_col_data = malloc(sizeof(collision_data) * game_data.level.brick_count);
	for (int i = 0; i < game_data.level.brick_count; i++)
		init_collision(&(game_data.bricks_col_data[i]));
	init_collision(&(game_data.player_col_data));
}

static int init_level(game_t *game_p, unsigned int solid_tex, unsigned int non_solid_tex,
									  int width, int height)
{
	game_data.level.solid_tex = solid_tex;
	game_data.level.non_solid_tex = non_solid_tex;
	if (!level_load_file(&(game_data.level), &(game_data.renderer), lvl_dir, width, height / 2))
		return FALSE;
	return TRUE;
}

static void init_text_data(game_t *game_p)
{
	game_data.text_data.char_size = 15.f;
	game_data.text_data.char_margin = 20.f;
}

int game_init(game_t *game_p, char *name, int width, int height)
{
	unsigned int ball_tex;
	unsigned int solid_tex;
	unsigned int non_solid_tex;
	unsigned int paddle_tex;

	game_p->width = width;
	game_p->height = height;

	ASSERT(window_init(&(game_data.window), name, width, height), "Could not initialize window\n");
	ASSERT(renderer_init(&(game_data.renderer), width, height), "Could not initialize renderer\n");

	if (!renderer_load_tex(&(game_data.renderer), &non_solid_tex, non_solid_tex_dir) ||
		!renderer_load_tex(&(game_data.renderer), &ball_tex, ball_tex_dir) ||
		!renderer_load_tex(&(game_data.renderer), &solid_tex, solid_tex_dir) ||
		!renderer_load_tex(&(game_data.renderer), &paddle_tex, paddle_tex_dir))
		return FALSE;

	ASSERT(text_init(&(game_data.renderer)), "Could not initialize text renderer\n");
	ASSERT(init_level(game_p, solid_tex, non_solid_tex, width, height), "Could not initialize level\n");

	init_text_data(game_p);
	init_collision_data(game_p);
	init_player_data(game_p, width, height);
	init_ball_data(game_p);

	obj_create(&(game_data.player), &(game_data.renderer), (vec3) { game_data.player_data.player_pos_x,
													game_data.player_data.player_pos_y, 0.f },
										   (vec3) { game_data.player_data.player_width,
													game_data.player_data.player_height, 0.f }, paddle_tex);

	obj_create(&(game_data.ball.base), &(game_data.renderer), (vec3) { 0.f, 0.f, 0.f },
		(vec3) { game_data.ball.radius, game_data.ball.radius, 0.f }, ball_tex);

	game_data.state = GAME_START;
	ASSERT(window_startgame(game_p, game_data.window), "error n shi\n");
	return TRUE;
}

static int reset_ball_pos(game_t *game_p)
{
	vec3 v_player_pos;
	game_data.ball.stucked = TRUE;
	obj_get_pos(&(game_data.player), &(game_data.renderer), v_player_pos);
	obj_set_pos(&(game_data.ball.base), &(game_data.renderer),
		(vec3) { v_player_pos[v_x] + (game_data.player_data.player_width / 2.f - game_data.ball.radius / 2.f),
				 v_player_pos[v_y] + (-game_data.ball.radius * 2.f), 0.f});
	return TRUE;
}

int game_process_input(game_t *game_p, keyinp_data_t *keyinp, float dt)
{
	if (keyinp[GLFW_KEY_ENTER].is_pressed && game_data.state != GAME_ACTIVE)
		game_data.state = GAME_ACTIVE;

	if (game_data.state == GAME_ACTIVE)
	{
		vec3 v_player_pos;

		obj_get_pos(&(game_data.player), &(game_data.renderer), v_player_pos);
		if (keyinp[GLFW_KEY_A].is_held)
			if (v_player_pos[v_x] >= 0.f)
				v_player_pos[v_x] -= game_data.player_data.player_speed * dt;
		if (keyinp[GLFW_KEY_D].is_held)
			if (v_player_pos[v_x] <= game_data.renderer.width - game_data.player_data.player_width)
				v_player_pos[v_x] += game_data.player_data.player_speed * dt;
		if (keyinp[GLFW_KEY_SPACE].is_pressed)
			game_data.ball.stucked = FALSE;
		if (keyinp[GLFW_KEY_C].is_pressed && game_data.ball.stucked)
		{
			game_data.player_data.shoot_dir = !game_data.player_data.shoot_dir;
			game_data.ball.v_velocity[v_x] *= -1;
		}

		obj_set_pos(&(game_data.player), &(game_data.renderer),
			(vec3) { v_player_pos[v_x], v_player_pos[v_y], 0.f });
		if (game_data.ball.stucked)
			reset_ball_pos(game_p);
		fflush(stdout);
	}
	return TRUE;
}

static void move_ball(game_t *game_p, float dt)
{
	vec3 v_ball_pos;
	int hit_right;

	obj_get_pos(&(game_data.ball.base), &(game_data.renderer), v_ball_pos);

	v_ball_pos[v_x] += game_data.ball.v_velocity[v_x] * dt;
	v_ball_pos[v_y] += game_data.ball.v_velocity[v_y] * dt;
	obj_set_pos(&(game_data.ball.base), &(game_data.renderer),
				(vec3) { v_ball_pos[v_x], v_ball_pos[v_y], 0.f });
	hit_right = FALSE;
	if (v_ball_pos[v_x] < 0.f ||
		(hit_right = (v_ball_pos[v_x] + game_data.ball.radius > game_data.renderer.width)))
	{
		game_data.ball.v_velocity[v_x] *= -1;
		v_ball_pos[v_x] = (hit_right) ? game_data.renderer.width - game_data.ball.radius : 0.f;
	}

	if (v_ball_pos[v_y] < 0.f)
	{
		game_data.ball.v_velocity[v_y] *= -1;
		v_ball_pos[v_y] = 0.f;
	}

	if (v_ball_pos[v_y] > game_data.renderer.height)
	{
		reset_ball_pos(game_p);
		game_data.ball.v_velocity[v_x] *=
			((game_data.player_data.shoot_dir && game_data.ball.v_velocity[v_x] < 0) ||
			 (!game_data.player_data.shoot_dir && game_data.ball.v_velocity[v_x] > 0)) ? -1 : 1;
		game_data.ball.v_velocity[v_y] *= -1;
	}
}

static void push_ball_oncollide(game_t *game_p, int collide_dir, float dt)
{
	vec3 v_ball_pos;
	game_data.ball.v_velocity[collide_dir] *= -1;
	obj_get_pos(&(game_data.ball.base), &(game_data.renderer), v_ball_pos);
	v_ball_pos[collide_dir] += game_data.ball.v_velocity[collide_dir] * dt;
	obj_set_pos(&(game_data.ball.base),
				&(game_data.renderer),
				(vec3) { v_ball_pos[v_x], v_ball_pos[v_y], 0.f });
}

static void ball_brick_collide(game_t *game_p, float dt)
{
	for (int i = 0; i < game_data.level.brick_count; i++)
	{
		brick_t *brick;
		collision_data *brick_col_data;

		brick = &(game_data.level.bricks[i]);
		brick_col_data = &(game_data.bricks_col_data[i]);
		if (!brick->destroyed && check_collision(&(game_data.renderer), &(game_data.ball.base),
			&(brick->base), brick_col_data))
		{
			if (brick->solid)
			{
				if (brick_col_data->collide_dir != -1)
					push_ball_oncollide(game_p, brick_col_data->collide_dir, dt);
			}
			else
				brick->destroyed = TRUE;
		}
	}
}

static void ball_player_collide(game_t *game_p, float dt)
{
	if (check_collision(&(game_data.renderer),
						&(game_data.ball.base),
						&(game_data.player),
						&(game_data.player_col_data)))
		push_ball_oncollide(game_p, game_data.player_col_data.collide_dir, dt);
}

static void ball_oncollide(game_t *game_p, float dt)
{
	ball_brick_collide(game_p, dt);
	ball_player_collide(game_p, dt);
}

static void check_player_win(game_t *game_p)
{
	int destroyed_count;
	int nonsolid_count;
	
	destroyed_count = 0;
	nonsolid_count = game_data.level.brick_count;
loop:
	for (int i = 0; i < game_data.level.brick_count; i++)
	{
		brick_t *brick = &(game_data.level.bricks[i]);

		if (game_data.state == GAME_WIN)
		{
			brick->destroyed = FALSE;
			continue;
		}

		if (brick->solid || !brick->legit)
			nonsolid_count--;
		else if (brick->destroyed)
			destroyed_count++;
	}

	if (destroyed_count == nonsolid_count)
	{
		reset_ball_pos(game_p);
		destroyed_count = 0;
		nonsolid_count = game_data.level.brick_count;
		game_data.state = GAME_WIN;
		goto loop;
	}
}

int game_update(game_t *game_p, float dt)
{
	if (game_data.state == GAME_ACTIVE)
	{
		check_player_win(game_p);
		if (!game_data.ball.stucked)
		{
			move_ball(game_p, dt);
			ball_oncollide(game_p, dt);
		}
	}
	return TRUE;
}

int game_render(game_t *game_p)
{
	/* hardcoded so yeah, pretty fucking terrible */
	char fps[10];
	char draw_call[15];
	char playmsg[30];
	char rdir_msg[14];
	char ldir_msg[13];

	switch (game_data.state)
	{
	case GAME_ACTIVE:
		for (int i = 0; i < game_data.level.brick_count; i++)
		{
			brick_t *brick;

			brick = &(game_data.level.bricks[i]);
			if (!brick->destroyed)
				obj_draw(&(brick->base), &(game_data.renderer));
		}
		obj_draw(&(game_data.player), &(game_data.renderer));
		obj_draw(&(game_data.ball.base), &(game_data.renderer));

		sprintf(rdir_msg, "%c Shoot right", (game_data.player_data.shoot_dir) ? '*' : ' ');
		sprintf(ldir_msg, "%c Shoot left", (!game_data.player_data.shoot_dir) ? '*' : ' ');

		render_text(&(game_data.renderer), rdir_msg,
			(game_data.player_data.shoot_dir) ? TEXT_RED : TEXT_WHITE,
			(vec3) { 5.f, game_p->height - game_data.text_data.char_size * 2, 0.f },
			(vec3) { game_data.text_data.char_size,
					 game_data.text_data.char_size,
					 game_data.text_data.char_margin });

		render_text(&(game_data.renderer), ldir_msg,
			(!game_data.player_data.shoot_dir) ? TEXT_RED : TEXT_WHITE,
			(vec3) { 5.f, game_p->height - game_data.text_data.char_size, 0.f },
			(vec3) { game_data.text_data.char_size,
					 game_data.text_data.char_size,
					 game_data.text_data.char_margin });
		break;
	case GAME_START:
	case GAME_WIN:
		sprintf(playmsg, "%s Press Enter to play!", (game_data.state == GAME_WIN) ? "You won!" : "");

		render_text(&(game_data.renderer), playmsg, TEXT_WHITE,
			(vec3) { game_p->width / 2 - (strlen(playmsg) * game_data.text_data.char_size) / 2,
					 (float)game_p->height / 2, 0.f },
			(vec3) { game_data.text_data.char_size,
					 game_data.text_data.char_size,
					 game_data.text_data.char_margin });
		break;
	}

	sprintf(fps, "FPS: %d", game_p->fps);
	sprintf(draw_call, "Draw calls: %d", game_data.renderer.dbg.draw_count + 1);

	render_text(&(game_data.renderer), fps, TEXT_WHITE,
		(vec3) { 5.f, 5.f, 0.f },
		(vec3) { game_data.text_data.char_size,
				 game_data.text_data.char_size,
				 game_data.text_data.char_margin });

	render_text(&(game_data.renderer), draw_call, TEXT_WHITE,
		(vec3) { 5.f, 5.f + game_data.text_data.char_size, 0.f },
		(vec3) { game_data.text_data.char_size,
				 game_data.text_data.char_size,
				 game_data.text_data.char_margin });

	renderer_draw(&(game_data.renderer));
	renderer_end_loop(&(game_data.renderer));
	return TRUE;
}

int game_cleanup(game_t *game_p)
{
	free(game_data.level.bricks);
	free(game_data.bricks_col_data);
	renderer_end(&(game_data.renderer));
	return TRUE;
}