#include "game.h"
#include "utils/utils.h"
#include <string.h>
#include <memory.h>

const char *ball_tex_dir = "\\Breakout\\data\\textures\\ball.png";
const char *non_solid_tex_dir = "\\Breakout\\data\\textures\\non_solid.png";
const char *solid_tex_dir = "\\Breakout\\data\\textures\\solid.png";
const char *paddle_tex_dir = "\\Breakout\\data\\textures\\paddle.png";
const char *lvl_dir = "\\Breakout\\data\\levels\\default.lvl";

game_data G;

const char *play_msg = "Press Enter to play";

const int char_size = 20.f;
const int char_margin = 25.f;

static void init_ball_data()
{
	G.ball.radius = 30.f;
	G.ball_data.ball_speed = 300.f;
	G.ball.v_velocity[v_x] = G.ball_data.ball_speed;
	G.ball.v_velocity[v_y] = -G.ball_data.ball_speed;
	G.ball.stucked = TRUE;
}

static void init_player_data(int width, int height)
{
	G.player_data.player_width = 100.f;
	G.player_data.player_height = 20.f;
	G.player_data.player_speed = 400.f;
	G.player_data.player_pos_x = (float)width / 2.f - G.player_data.player_width / 2.f;
	G.player_data.player_pos_y = (float)height - G.player_data.player_height;
}

static void init_collision_data()
{
	G.bricks_col_data = malloc(sizeof(collision_data) * G.level.brick_count);
	for (int i = 0; i < G.level.brick_count; i++)
		init_collision(&(G.bricks_col_data[i]));
	init_collision(&(G.player_col_data));
}

static int init_level(unsigned int solid_tex, unsigned int non_solid_tex,
					   int width, int height)
{
	G.level.solid_tex = solid_tex;
	G.level.non_solid_tex = non_solid_tex;
	if (!level_load_file(&(G.level), &(G.renderer), lvl_dir, width, height / 2))
		return FALSE;
	return TRUE;
}

int game_init(game_t *game_p, char *name, int width, int height)
{
	unsigned int ball_tex;
	unsigned int solid_tex;
	unsigned int non_solid_tex;
	unsigned int paddle_tex;

	game_p->width = width;
	game_p->height = height;

	ASSERT(window_init(&(G.window), name, width, height), "Could not initialize window\n");
	ASSERT(renderer_init(&(G.renderer), width, height), "Could not initialize renderer\n");

	if (!renderer_load_tex(&(G.renderer), &non_solid_tex, non_solid_tex_dir) ||
		!renderer_load_tex(&(G.renderer), &ball_tex, ball_tex_dir) ||
		!renderer_load_tex(&(G.renderer), &solid_tex, solid_tex_dir) ||
		!renderer_load_tex(&(G.renderer), &paddle_tex, paddle_tex_dir))
		return FALSE;

	ASSERT(text_init(&(G.renderer)), "Could not initialize text renderer\n");
	ASSERT(init_level(solid_tex, non_solid_tex, width, height), "Could not initialize level\n");

	init_collision_data();
	init_player_data(width, height);
	init_ball_data();

	obj_create(&(G.player), &(G.renderer), (vec3) { G.player_data.player_pos_x,
													G.player_data.player_pos_y, 0.f },
										   (vec3) { G.player_data.player_width,
													G.player_data.player_height, 0.f }, paddle_tex);

	obj_create(&(G.ball.base), &(G.renderer), (vec3) { 0.f, 0.f, 0.f },
										(vec3) { G.ball.radius, G.ball.radius, 0.f }, ball_tex);

	G.state = GAME_START;
	ASSERT(window_startgame(game_p, G.window), "error n shi\n");
	return TRUE;
}

static int reset_ball_pos()
{
	vec3 v_plrenderer_pos;
	G.ball.stucked = TRUE;
	obj_get_pos(&(G.player), &(G.renderer), v_plrenderer_pos);
	obj_set_pos(&(G.ball.base), &(G.renderer),
		(vec3) { v_plrenderer_pos[v_x] + (G.player_data.player_width / 2.f - G.ball.radius / 2.f),
				 v_plrenderer_pos[v_y] + (-G.ball.radius * 2.f), 0.f});
	return TRUE;
}

int game_process_input(game_t *game_p, keyinp_data_t *keyinp_p, float dt)
{
	if (keyinp_p->key == GLFW_KEY_ENTER && G.state != GAME_ACTIVE)
		G.state = GAME_ACTIVE;

	if (G.state == GAME_ACTIVE)
	{
		vec3 v_plrenderer_pos;

		obj_get_pos(&(G.player), &(G.renderer), v_plrenderer_pos);
		if (keyinp_p->key == GLFW_KEY_A && keyinp_p->is_hold)
			if (v_plrenderer_pos[v_x] >= 0.f)
				v_plrenderer_pos[v_x] -= G.player_data.player_speed * dt;
		if (keyinp_p->key == GLFW_KEY_D && keyinp_p->is_hold)
			if (v_plrenderer_pos[v_x] <= G.renderer.width - G.player_data.player_width)
				v_plrenderer_pos[v_x] += G.player_data.player_speed * dt;
		if (keyinp_p->key == GLFW_KEY_SPACE && keyinp_p->is_hold)
			G.ball.stucked = FALSE;

		obj_set_pos(&(G.player), &(G.renderer),
			(vec3) { v_plrenderer_pos[v_x], v_plrenderer_pos[v_y], 0.f });
		if (G.ball.stucked)
			reset_ball_pos();
		fflush(stdout);
	}
	return TRUE;
}

static void move_ball(float dt)
{
	vec3 v_ball_pos;
	int hit_right;

	obj_get_pos(&(G.ball.base), &(G.renderer), v_ball_pos);

	v_ball_pos[v_x] += G.ball.v_velocity[v_x] * dt;
	v_ball_pos[v_y] += G.ball.v_velocity[v_y] * dt;
	obj_set_pos(&(G.ball.base), &(G.renderer), (vec3) { v_ball_pos[v_x], v_ball_pos[v_y], 0.f });
	hit_right = FALSE;
	if (v_ball_pos[v_x] < 0.f || (hit_right = (v_ball_pos[v_x] + G.ball.radius > G.renderer.width)))
	{
		G.ball.v_velocity[v_x] *= -1;
		v_ball_pos[v_x] = (hit_right) ? G.renderer.width - G.ball.radius : 0.f;
	}

	if (v_ball_pos[v_y] < 0.f)
	{
		G.ball.v_velocity[v_y] *= -1;
		v_ball_pos[v_y] = 0.f;
	}

	if (v_ball_pos[v_y] > G.renderer.height)
	{
		reset_ball_pos();
		G.ball.v_velocity[v_x] *= (G.ball.v_velocity[v_x] < 0) ? -1 : 1;
		G.ball.v_velocity[v_y] *= -1;
	}
}

static void push_ball_oncollide(int collide_dir, float dt)
{
	vec3 v_ball_pos;
	G.ball.v_velocity[collide_dir] *= -1;
	obj_get_pos(&(G.ball.base), &(G.renderer), v_ball_pos);
	v_ball_pos[collide_dir] += G.ball.v_velocity[collide_dir] * dt;
	obj_set_pos(&(G.ball.base), &(G.renderer), (vec3) { v_ball_pos[v_x], v_ball_pos[v_y], 0.f });
}

static void ball_brick_collide(float dt)
{
	for (int i = 0; i < G.level.brick_count; i++)
	{
		brick_t *brick;
		collision_data *brick_col_data;

		brick = &(G.level.bricks[i]);
		brick_col_data = &(G.bricks_col_data[i]);
		if (!brick->destroyed && check_collision(&(G.renderer), &(G.ball.base),
			&(brick->base), brick_col_data))
		{
			if (brick->solid)
			{
				if (brick_col_data->collide_dir != -1)
					push_ball_oncollide(brick_col_data->collide_dir, dt);
			}
			else
				brick->destroyed = TRUE;
		}
	}
}

static void ball_player_collide(float dt)
{
	if (check_collision(&(G.renderer), &(G.ball.base), &(G.player), &(G.player_col_data)))
		push_ball_oncollide(G.player_col_data.collide_dir, dt);
}

static void ball_oncollide(float dt)
{
	ball_brick_collide(dt);
	ball_player_collide(dt);
}

static void check_player_win()
{
	int destroyed_count;
	int nonsolid_count;
	
	destroyed_count = 0;
	nonsolid_count = G.level.brick_count;
loop:
	for (int i = 0; i < G.level.brick_count; i++)
	{
		brick_t *brick = &(G.level.bricks[i]);

		if (G.state == GAME_WIN)
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
		reset_ball_pos();
		destroyed_count = 0;
		nonsolid_count = G.level.brick_count;
		G.state = GAME_WIN;
		goto loop;
	}
}

int game_update(game_t *game_p, float dt)
{
	if (G.state == GAME_ACTIVE)
	{
		check_player_win();
		if (!G.ball.stucked)
		{
			move_ball(dt);
			ball_oncollide(dt);
		}
	}
	return TRUE;
}

int game_render(game_t *game_p)
{
	char fps[10];
	char draw_call[15];

	switch (G.state)
	{
	case GAME_ACTIVE:
		for (int i = 0; i < G.level.brick_count; i++)
		{
			brick_t *brick;

			brick = &(G.level.bricks[i]);
			if (!brick->destroyed)
				obj_draw(&(brick->base), &(G.renderer));
		}
		obj_draw(&(G.player), &(G.renderer));
		obj_draw(&(G.ball.base), &(G.renderer));
		break;
	case GAME_START:
	case GAME_WIN:
		render_text(&(G.renderer), play_msg, TEXT_WHITE,
			(vec3) { game_p->width / 2 - (strlen(play_msg) * char_size) / 2,
					 game_p->height / 2, 0.f },
			(vec3) { char_size, char_size, char_margin });
		break;
	}

	sprintf(fps, "FPS: %d", game_p->fps);
	sprintf(draw_call, "Draw calls: %d", G.renderer.dbg.draw_count + 1);

	render_text(&(G.renderer), fps, TEXT_WHITE,
		(vec3) { 5.f, 5.f, 0.f }, (vec3) { char_size, char_size, char_margin });
	render_text(&(G.renderer), draw_call, TEXT_WHITE,
		(vec3) { 5.f, 5.f + char_size, 0.f }, (vec3) { char_size, char_size, char_margin });

	renderer_draw(&(G.renderer));
	renderer_end_loop(&(G.renderer));
	return TRUE;
}

int game_cleanup(game_t *game_p)
{
	free(G.level.bricks);
	free(G.bricks_col_data);
	renderer_end(&(G.renderer));
	return TRUE;
}