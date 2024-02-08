#include "obj.h"
#include "utils/utils.h"
#include "renderer/renderer.h"
#include <memory.h>

static void decompose_t(mat4 m, vec4 t)
{
	glm_vec4_copy(m[3], t);
}

void obj_create(obj_t *obj_p, renderer_t *renderer_p, vec3 v_pos, vec3 v_scale, unsigned int tex_id)
{
	glm_mat4_identity(obj_p->model);
	glm_translate(obj_p->model, BREAK_VEC3(v_pos));
	glm_scale(obj_p->model, BREAK_VEC3(v_scale));
	obj_p->tex_id = tex_id;
}

void obj_draw(obj_t *obj_p, renderer_t *renderer_p)
{
	renderer_add_local_mat(renderer_p, obj_p->model, &(obj_p->model_index));
	renderer_draw_quad(renderer_p, obj_p->tex_id, obj_p->model_index);
}

void obj_get_pos(obj_t *obj_p, renderer_t *renderer_p, vec3 v_pos)
{
	vec4 pos_v4;
	decompose_t(&(obj_p->model), &pos_v4);
	glm_vec3(pos_v4, v_pos);
}

void obj_set_pos(obj_t *obj_p, renderer_t *renderer_p, vec3 v_pos)
{
	COPY_VEC3(obj_p->model[3], v_pos);
}

void obj_add_pos(obj_t *obj_p, renderer_t *renderer_p, vec3 v_pos)
{
	glm_translate(obj_p->model, BREAK_VEC3(v_pos));
}

void obj_add_rot(obj_t *obj_p, renderer_t *renderer_p, float angle, vec3 v_axis)
{
	glm_rotate(obj_p->model, angle, BREAK_VEC3(v_axis));
}

void obj_get_scale(obj_t *obj_p, renderer_t *renderer_p, vec3 v_scale)
{
	mat4 rotation;
	glm_decompose_rs(obj_p->model, rotation, v_scale);
}

void obj_add_scale(obj_t *obj_p, renderer_t *renderer_p, vec3 v_scale)
{
	glm_scale(obj_p->model, BREAK_VEC3(v_scale));
}