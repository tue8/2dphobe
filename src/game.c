#include "game.h"
#include "utils/utils.h"
#include <string.h>
#include <memory.h>

const int char_size = 20.f;
const int char_margin = 25.f;

int game_init(game_t *game_p, int width, int height)
{
	game_p->width = width;
	game_p->height = height;

	ASSERT(window_init(&(game_p->data.window), "2dphobe", width, height), "Could not initialize window\n");
	ASSERT(renderer_init(&(game_p->data.renderer), width, height), "Could not initialize renderer\n");
	ASSERT(text_init(&(game_p->data.renderer)), "Could not initialize text renderer\n");
	ASSERT(window_startgame(game_p, game_p->data.window), "error n shi\n");
	return TRUE;
}

int game_process_input(game_t *game_p, keyinp_data_t *keyinp_p, float dt)
{
	return TRUE;
}

int game_update(game_t *game_p, float dt)
{
	return TRUE;
}

int game_render(game_t *game_p)
{
	char fps[10];
	char draw_call[15];

	sprintf(fps, "FPS: %d", game_p->fps);
	sprintf(draw_call, "Draw calls: %d", game_p->data.renderer.dbg.draw_count + 1);

	render_text(&(game_p->data.renderer), fps, TEXT_WHITE,
		(vec3) { 5.f, 5.f, 0.f }, (vec3) { char_size, char_size, char_margin });
	render_text(&(game_p->data.renderer), draw_call, TEXT_WHITE,
		(vec3) { 5.f, 5.f + char_size, 0.f }, (vec3) { char_size, char_size, char_margin });

	renderer_draw(&(game_p->data.renderer));
	renderer_end_loop(&(game_p->data.renderer));
	return TRUE;
}

int game_cleanup(game_t *game_p)
{
	renderer_end(&(game_p->data.renderer));
	return TRUE;
}