#include "obj.h"
#include "utils/utils.h"
#include "renderer/renderer.h"
#include <memory.h>

static void decompose_t(mat4 m, vec4 t)
{
	glm_vec4_copy(m[3], t);
}

void obj_create(phobe_obj *obj_p, phobe_renderer *renderer_p, phobe_vec3 pos,
								phobe_vec3 scale, phobe_vec3 color, unsigned int tex_id)
{
	glm_mat4_identity(obj_p->local_mat);
	glm_translate(obj_p->local_mat, RAW_VEC3(pos));
	glm_scale(obj_p->local_mat, RAW_VEC3(scale));
	obj_p->tex_id = tex_id;
	obj_p->color = color;
}

void obj_draw(phobe_obj *obj_p, phobe_renderer *renderer_p)
{
	renderer_add_local_mat(renderer_p, obj_p->local_mat, &(obj_p->local_mat_index));
	renderer_draw_quad(renderer_p,
										 (Rvec3_t) { obj_p->color.x, obj_p->color.y, obj_p->color.z},
										 obj_p->tex_id,
										 obj_p->local_mat_index);
}

void obj_get_pos(phobe_obj *obj_p, phobe_renderer *renderer_p, phobe_vec3 pos)
{
	vec4 pos_v4;
	decompose_t(obj_p->local_mat, pos_v4);
	glm_vec3(pos_v4, RAW_VEC3(pos));
}

void obj_set_pos(phobe_obj *obj_p, phobe_renderer *renderer_p, phobe_vec3 pos)
{
	vec3 v_pos = {pos.x, pos.y, pos.z};
	COPY_VEC3(obj_p->local_mat[3], v_pos);
}

void obj_add_pos(phobe_obj *obj_p, phobe_renderer *renderer_p, phobe_vec3 pos)
{
	glm_translate(obj_p->local_mat, RAW_VEC3(pos));
}

void obj_add_rot(phobe_obj *obj_p, phobe_renderer *renderer_p, float angle, phobe_vec3 axis)
{
	glm_rotate(obj_p->local_mat, angle, RAW_VEC3(axis));
}

void obj_get_scale(phobe_obj *obj_p, phobe_renderer *renderer_p, phobe_vec3 scale)
{
	mat4 rotation;
	glm_decompose_rs(obj_p->local_mat, rotation, RAW_VEC3(scale));
}

void obj_add_scale(phobe_obj *obj_p, phobe_renderer *renderer_p, phobe_vec3 scale)
{
	glm_scale(obj_p->local_mat, RAW_VEC3(scale));
}