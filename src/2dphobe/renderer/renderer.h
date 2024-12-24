#ifndef RENDERER_H_
#define RENDERER_H_

#define QUAD_VERT 6

#include "shader.h"

typedef struct
{
	float x, y;
} Rvec2_t;

typedef struct
{
	float x, y, z;
} Rvec3_t;

typedef struct
{
	Rvec2_t start;
	Rvec2_t offset;
} texcoords_data;

typedef struct
{
	Rvec3_t ndc;
	Rvec3_t color;
	Rvec2_t tex_coord;
	float tex_index;
	float local_mat_index;
} vertex_t;

typedef struct
{
	unsigned int ssbo;
	mat4 *local_mat_arr;
	int local_mat_arr_size;
	int local_mat_sp;
	mat4 view, proj;
} mvp_data;

typedef struct
{
	vertex_t *vert;
	int vert_size;
	int quad_count;
} vert_data;

typedef struct
{
	int max_vert;
	int max_tex;
} max_vars;

typedef struct
{
	int draw_count;
} debug_data;

typedef struct
{
	int width, height;
	unsigned int shader_id;
	unsigned int vao, vbo;
	vert_data vert;
	mvp_data mvp;
	max_vars mvars;
	unsigned int *texs;
	int tex_count;
	debug_data dbg;
} phobe_renderer;

int renderer_init(phobe_renderer *, int, int);
void renderer_draw_quad(phobe_renderer *, Rvec3_t, unsigned int, unsigned int);
void renderer_draw_quadc(phobe_renderer *, texcoords_data, Rvec3_t, unsigned int, unsigned int);
int renderer_load_tex(phobe_renderer *, unsigned int *, const char *);
void renderer_add_local_mat(phobe_renderer *, mat4, int *);
void renderer_draw(phobe_renderer *);
void renderer_flush(phobe_renderer *);
void renderer_start_loop();
void renderer_end_loop(phobe_renderer *);
void renderer_end(phobe_renderer *);

#endif