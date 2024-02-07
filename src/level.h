#ifndef LEVEL_H_
#define LEVEL_H_

#include "brick.h"

typedef struct {
	brick_t *bricks;
	int brick_count;
	unsigned int solid_tex;
	unsigned int non_solid_tex;
} level_t;

int level_load_file(level_t *, renderer_t *, char *, int, int);

#endif