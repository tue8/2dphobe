#ifndef OBJ_H_
#define OBJ_H_

#include "renderer/renderer.h"

#define RAW_VEC3(pv) (vec3) {pv.x, pv.y, pv.z}
#define BREAK_VEC3(v) (vec3) { v[0], v[1], v[2] }
#define COPY_VEC3(v0, v1) v0[0] = v1[0]; v0[1] = v1[1]; v0[2] = v1[2];

typedef struct
{
	float x, y, z;
} phobe_vec3;


typedef struct
{
	mat4 local_mat;
	unsigned int tex_id;
	unsigned int local_mat_index;
	phobe_vec3 color;
} obj_t;

void obj_create(obj_t *, renderer_t *, phobe_vec3, phobe_vec3, phobe_vec3, unsigned int);
void obj_draw(obj_t *, renderer_t *);
void obj_get_pos(obj_t *, renderer_t *, phobe_vec3);
void obj_set_pos(obj_t *, renderer_t *, phobe_vec3);
void obj_add_pos(obj_t *, renderer_t *, phobe_vec3);
void obj_add_rot(obj_t *, renderer_t *, float, phobe_vec3);
void obj_get_scale(obj_t *, renderer_t *, phobe_vec3);
void obj_add_scale(obj_t *, renderer_t *, phobe_vec3);

#endif