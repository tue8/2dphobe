#ifndef BALL_H_
#define BALL_H_

#include "engine/obj.h"

typedef struct
{
	obj_t base;
	int stucked;
	float radius;
	vec2 v_velocity;
} ball_t;

#endif