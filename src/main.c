#include <stdlib.h>
#include "game.h"

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

int main(int argc, char **argv)
{
	phobe_game game;
	int width;
	int height;

	width = min_w;
	height = min_h;
	if (argc >= 3)
		get_res(argv, &width, &height);
	if (!game_init(&game, width, height))
		cleanexit(-1);
	cleanexit(0);
}