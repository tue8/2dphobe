#ifndef COLLISION_H_
#define COLLISION_H_

#include "obj.h"

typedef struct
{
	int x_overlap;
	int y_overlap;
	int collide_dir;
} collision_data;

int init_collision(collision_data *);
int check_collision(phobe_renderer *, phobe_obj *, phobe_obj *, collision_data *);

#endif