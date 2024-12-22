#include "collision.h"
#include "utils/utils.h"

int init_collision(collision_data *col_data)
{
	col_data->x_overlap = FALSE;
	col_data->y_overlap = FALSE;
	col_data->collide_dir = -1;
	return TRUE;
}

int check_collision(renderer_t *renderer_p, obj_t *a, obj_t *b, collision_data *col_data)
{
	phobe_vec3 a_pos   = { 0.f, 0.f, 0.f };
	phobe_vec3 b_pos   = { 0.f, 0.f, 0.f };
	phobe_vec3 a_scale = { 0.f, 0.f, 0.f };
	phobe_vec3 b_scale = { 0.f, 0.f, 0.f };
	int x_overlap;
	int y_overlap;
	int precollide;

	obj_get_pos(a, renderer_p, a_pos);
	obj_get_pos(b, renderer_p, b_pos);
	obj_get_scale(a, renderer_p, a_scale);
	obj_get_scale(b, renderer_p, b_scale);

	/* right side of a overlaps with left side of b and vice versa */
	x_overlap = (a_pos.x + a_scale.x > b_pos.x && 
							 b_pos.x + b_scale.x > a_pos.x);
	/* top side of a overlaps with bottom side of b and vice versa */
	y_overlap = (a_pos.y + a_scale.y > b_pos.y &&
							 b_pos.y + b_scale.y > a_pos.y);

	if (x_overlap && y_overlap)
			col_data->collide_dir = (col_data->x_overlap) ? 1 : 0;
	else
	{
		col_data->x_overlap = x_overlap;
		col_data->y_overlap = y_overlap;
	}

	return (x_overlap && y_overlap);
}