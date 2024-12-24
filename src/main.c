#include <stdio.h>
#include <stdlib.h>
#include "2dphobe/phobe.h"

const int min_w = 800;
const int min_h = 600;

const int max_w = 1920;
const int max_h = 1080;

static void cleanexit(int code)
{
	glfwTerminate();
	exit(code);
}

static void get_res(char **argv, int *w_p, int *h_p)
{
	long l_w;
	long l_h;
	l_w = strtol(argv[1], (char **)NULL, 10);
	l_h = strtol(argv[2], (char **)NULL, 10);
	if (l_w > max_w || l_w < min_w || l_h > max_h || l_h < min_h)
	{
		fprintf(stderr, "The window size must be within the range of %d x %d -> %d x %d\n",
			min_w, min_h, max_w, max_h);
		cleanexit(-1);
	}
	*w_p = (int)l_w;
	*h_p = (int)l_h;
}

static void render_text_shadow(phobe_renderer *renderer, char *str,
															 phobe_vec3 pos, phobe_vec3 scale)
{
	render_text(renderer,
							str,
							TEXT_BLACK,
							(phobe_vec3) {
		pos.x + (scale.x / 5.f),
			pos.y + (scale.y / 5.f),
			pos.z
	},
							scale);
	render_text(renderer, str, TEXT_WHITE, pos, scale);
}

int main(int argc, char **argv)
{
	phobe_context context;
	int width;
	int height;

	width = min_w;
	height = min_h;
	if (argc >= 3)
		get_res(argv, &width, &height);


	const float char_size = 20.f;
	const float char_margin = 25.f;
	const float quad_size = 5.f;

	phobe_renderer renderer;
	phobe_obj *quads;
	int quad_count = 0;

	if (!phobe_init(&context, "thang", width, height))
		cleanexit(-1);


	ASSERT(renderer_init(&renderer, width, height), "Could not initialize renderer\n");
	ASSERT(text_init(&renderer), "Could not initialize text renderer\n");

	unsigned int default_tex_id;
	char *default_tex_dir = working_dir("\\data\\textures\\default.png");
	int res = renderer_load_tex(&renderer, &default_tex_id, default_tex_dir);
	free(default_tex_dir);

	if (!res)
		cleanexit(-1);

	quads = (phobe_obj *)malloc(sizeof(phobe_obj) * ((width * height) / quad_size));
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


	while (!phobe_shouldclose(&context))
	{
		phobe_startloop(&context);
		renderer_start_loop();

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

		sprintf(fps, "FPS: %d", context.fps);
		sprintf(draw_call, "Draw calls: %d", renderer.dbg.draw_count + 1);

		render_text_shadow(&renderer, fps,
											 (phobe_vec3) {
			5.f, 5.f, 0.f
		}, (phobe_vec3) { char_size, char_size, char_margin });
		render_text_shadow(&renderer, draw_call,
											 (phobe_vec3) {
			5.f, 5.f + char_size, 0.f
		}, (phobe_vec3) { char_size, char_size, char_margin });

		renderer_draw(&renderer);
		renderer_end_loop(&renderer);
		phobe_endloop(&context);
	}

	free(quads);
	cleanexit(0);
}