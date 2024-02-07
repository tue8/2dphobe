#ifndef OBJ_H_
#define OBJ_H_

#include "renderer/renderer.h"

#define v_x 0
#define v_y 1
#define v_z 2
#define BREAK_VEC3(v) (vec3) { v[v_x], v[v_y], v[v_z] }
#define COPY_VEC3(v0, v1) v0[v_x] = v1[v_x]; v0[v_y] = v1[v_y]; v0[v_z] = v1[v_z];

typedef struct
{
	mat4 model;
	unsigned int tex_id;
	unsigned int model_index;
} obj_t;

void obj_create(obj_t *, renderer_t *, vec3, vec3, unsigned int);
void obj_draw(obj_t *, renderer_t *);
void obj_get_pos(obj_t *, renderer_t *, vec3);
void obj_set_pos(obj_t *, renderer_t *, vec3);
void obj_add_pos(obj_t *, renderer_t *, vec3);
void obj_add_rot(obj_t *, renderer_t *, float, vec3);
void obj_get_scale(obj_t *, renderer_t *, vec3);
void obj_add_scale(obj_t *, renderer_t *, vec3);

#endif