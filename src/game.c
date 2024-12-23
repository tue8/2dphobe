#include "game.h"
#include "utils/utils.h"
#include <string.h>
#include <memory.h>

const int char_size = 20.f;
const int char_margin = 25.f;
const int quad_size = 5;

GLFWwindow *window;
phobe_renderer renderer;
phobe_obj *quads;
int quad_count = 0;

int game_init(phobe_game *game_p, int width, int height)
{
	game_p->width = width;
	game_p->height = height;

	ASSERT(window_init(&window, "2dphobe", width, height), "Could not initialize window\n");
	ASSERT(renderer_init(&renderer, width, height), "Could not initialize renderer\n");
	ASSERT(text_init(&renderer), "Could not initialize text renderer\n");

	unsigned int default_tex_id;
	char *default_tex_dir = working_dir("\\data\\textures\\default.png");
	int res = renderer_load_tex(&renderer, &default_tex_id, default_tex_dir);
	free(default_tex_dir);

	if (!res)
		return FALSE;

	quads = (phobe_obj *)malloc(sizeof(phobe_obj) * (width * height) / quad_size);
	for (float i = 0.f; i < width; i += quad_size)
	{
		for (float j = 0.f; j < height; j += quad_size)
		{
			phobe_obj quad;
			phobe_vec3 pos = { i, j, 0.f };
			phobe_vec3 size = { quad_size, quad_size, 0.f };
			phobe_vec3 color = { 0.f, 1.f, 0.f };
			obj_create(&quad, &renderer, pos, size, color, default_tex_id);
			quads[quad_count++] = quad;
		}
	}

	ASSERT(window_startgame(game_p, window), "error n shi\n");
	return TRUE;
}

int game_process_input(phobe_game *game_p, keyinp_data_t *keyinp_p, float dt)
{
	return TRUE;
}

int game_update(phobe_game *game_p, float dt)
{
	return TRUE;
}

static void render_text_shadow(phobe_renderer *renderer, char *str,
															 phobe_vec3 pos, phobe_vec3 scale)
{
	render_text(renderer,
							str,
							TEXT_BLACK,
							(phobe_vec3) {pos.x + (scale.x / 5.f),
														pos.y + (scale.y / 5.f),
														pos.z },
							scale);
	render_text(renderer, str, TEXT_WHITE, pos, scale);
}

int game_render(phobe_game *game_p)
{
	for (int i = 0; i < quad_count; i++)
	{
		float r = (rand() % (100 + 1)) / 100.f;
		float g = (rand() % (100 + 1)) / 100.f;
		float b = (rand() % (100 + 1)) / 100.f;
		quads[i].color = (phobe_vec3){ r, g, b };
		obj_draw(&quads[i], &renderer);
	}

	char fps[10];
	char draw_call[15];

	sprintf(fps, "FPS: %d", game_p->fps);
	sprintf(draw_call, "Draw calls: %d", renderer.dbg.draw_count + 1);

	render_text_shadow(&renderer, fps,
		(phobe_vec3) { 5.f, 5.f, 0.f }, (phobe_vec3) { char_size, char_size, char_margin });
	render_text_shadow(&renderer, draw_call,
		(phobe_vec3) { 5.f, 5.f + char_size, 0.f }, (phobe_vec3) { char_size, char_size, char_margin });

	renderer_draw(&renderer);
	renderer_end_loop(&renderer);
	return TRUE;
}

int game_cleanup(phobe_game *game_p)
{
	free(quads);
	renderer_end(&renderer);
	return TRUE;
}