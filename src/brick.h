#ifndef BRICK_H_
#define BRICK_H_

#include "engine/obj.h"

typedef struct
{
	obj_t base;
	int destroyed;
	int solid;
	int legit;
} brick_t;

#endif