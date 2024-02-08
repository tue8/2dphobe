#include "renderer.h"
#include "../utils/utils.h"

#include <glad/glad.h>
#include <memory.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MAX_QUAD 1000

static void renderer_get_max(renderer_t *renderer_p)
{
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &(renderer_p->mvars.max_tex));
	renderer_p->mvars.max_vert = MAX_QUAD * QUAD_VERT;
}

static void renderer_init_dbg(renderer_t *renderer_p)
{
	renderer_p->dbg.draw_count = 0;
}

static void renderer_init_vao(renderer_t *renderer_p)
{
	glGenVertexArrays(1, &(renderer_p->vao));
	glGenBuffers(1, &(renderer_p->vbo));
	glBindVertexArray(renderer_p->vao);

	glBindBuffer(GL_ARRAY_BUFFER, renderer_p->vbo);
	glBufferData(GL_ARRAY_BUFFER, renderer_p->vert.vert_size, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertex_t),
		(void *)offsetof(vertex_t, ndc));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(vertex_t),
		(void *)offsetof(vertex_t, tex_coord));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
		sizeof(vertex_t),
		(void *)offsetof(vertex_t, tex_index));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,
		sizeof(vertex_t),
		(void *)offsetof(vertex_t, local_mat_index));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertex_t),
		(void *)offsetof(vertex_t, color));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

static void renderer_init_mvp(renderer_t *renderer_p)
{
	renderer_p->mvp.local_mat_sp = 0;
	renderer_p->mvp.local_mat_arr_size = sizeof(mat4) * MAX_QUAD;
	renderer_p->mvp.local_mat_arr = malloc(renderer_p->mvp.local_mat_arr_size);

	glGenBuffers(1, &(renderer_p->mvp.ssbo));
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer_p->mvp.ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, renderer_p->mvp.local_mat_arr_size, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, renderer_p->mvp.ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glm_mat4_identity(renderer_p->mvp.view);
	glm_mat4_identity(renderer_p->mvp.proj);
	glm_ortho(0.f, (float)renderer_p->width, (float)renderer_p->height,
					0.f, -1.f, 1.f, renderer_p->mvp.proj);
}

static void renderer_init_vertices(renderer_t *renderer_p)
{
	renderer_p->vert.vert_size = sizeof(vertex_t) * renderer_p->mvars.max_vert;
	renderer_p->vert.vert = malloc(renderer_p->vert.vert_size);
	memset(renderer_p->vert.vert, 0, renderer_p->vert.vert_size);
	renderer_p->vert.quad_count = 0;
}

static void renderer_init_textures(renderer_t *renderer_p)
{
	int tex_size;
	tex_size = sizeof(unsigned int) * renderer_p->mvars.max_tex;
	renderer_p->texs = malloc(tex_size);
	memset(renderer_p->texs, 0, tex_size);
	renderer_p->tex_count = 0;
}


int renderer_init(renderer_t *renderer_p, int width, int height)
{
	renderer_p->width = width;
	renderer_p->height = height;
	renderer_get_max(renderer_p);
	if (!init_shader(renderer_p))
		return FALSE;
	renderer_init_dbg(renderer_p);
	renderer_init_vertices(renderer_p);
	renderer_init_vao(renderer_p);
	renderer_init_mvp(renderer_p);
	renderer_init_textures(renderer_p);
	return TRUE;
}

static void renderer_draw_call(renderer_t *renderer_p)
{
	glBindVertexArray(renderer_p->vao);
	glDrawArrays(GL_TRIANGLES, 0, QUAD_VERT * renderer_p->vert.quad_count);
}

static void renderer_set_samplers(renderer_t *renderer_p)
{
	unsigned int texs_loc;
	int samplers[32];

	for (int i = 0; i < 32; i++)
		samplers[i] = i;
	texs_loc = glGetUniformLocation(renderer_p->shader_id, "texs");
	glUniform1iv(texs_loc, 32, samplers);
}

static void renderer_submit_mvp(renderer_t *renderer_p)
{
	set_uniform_mat4(renderer_p->shader_id, "view", renderer_p->mvp.view);
	set_uniform_mat4(renderer_p->shader_id, "proj", renderer_p->mvp.proj);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer_p->mvp.ssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (renderer_p->mvp.local_mat_sp + 1) * sizeof(mat4)
												, renderer_p->mvp.local_mat_arr);
}

void renderer_add_local_mat(renderer_t *renderer_p, mat4 local_mat, int *local_mat_index)
{
	if (renderer_p->mvp.local_mat_sp + 1 >= MAX_QUAD)
		renderer_draw(renderer_p);
	memcpy(renderer_p->mvp.local_mat_arr + renderer_p->mvp.local_mat_sp, local_mat, sizeof(mat4));
	*local_mat_index = renderer_p->mvp.local_mat_sp++;
}

static void renderer_add_quad(renderer_t *renderer_p, vertex_t *quad_data)
{
	memcpy(renderer_p->vert.vert + (QUAD_VERT * renderer_p->vert.quad_count), quad_data,
			sizeof(vertex_t) * QUAD_VERT);
	renderer_p->vert.quad_count++;
}

static void renderer_submit_textures(renderer_t *renderer_p)
{
	for (int i = 0; i < renderer_p->tex_count; i++)
		glBindTextureUnit(i, *(renderer_p->texs + i));
}

void renderer_flush(renderer_t *renderer_p)
{
	renderer_p->vert.quad_count = 0;
	renderer_p->tex_count = 0;
	renderer_p->mvp.local_mat_sp = 0;

	memset(renderer_p->vert.vert, 0, renderer_p->vert.vert_size);
	memset(renderer_p->texs, 0, renderer_p->mvars.max_tex * sizeof(unsigned int));
	memset(renderer_p->mvp.local_mat_arr, 0, renderer_p->mvp.local_mat_arr_size);
}

static float get_tex_index(renderer_t *renderer_p, unsigned int tex_id)
{
	float tex_index;
	int tex_find;
	int i;
	tex_find = 0;

	for (i = 0; i < renderer_p->tex_count; i++)
	{
		if (*(renderer_p->texs + i) == tex_id)
		{
			tex_index = (float)i;
			tex_find = 1;
			break;
		}
	}

	if (!tex_find)
	{
		if (renderer_p->tex_count > renderer_p->mvars.max_tex - 1)
			renderer_draw(renderer_p);
		*(renderer_p->texs + renderer_p->tex_count) = tex_id;
		tex_index = (float)renderer_p->tex_count++;
	}

	return tex_index;
}

static void fill_vertex_data(vertex_t *quad, int i, Rvec3_t ndc, Rvec2_t tex_coord,
							Rvec3_t color, float tex_index, float local_mat_index)
{
	quad[i].ndc = ndc;
	quad[i].tex_coord = tex_coord;
	quad[i].tex_index = tex_index;
	quad[i].local_mat_index = local_mat_index;
	quad[i].color = color;
}

static void fill_quad_data(vertex_t *quad, texcoords_data tex_coords, Rvec3_t color,
										  float tex_index, float local_mat_index)
{
	fill_vertex_data(quad, 0,
		(Rvec3_t) { 0.f, 0.f, 0.f },
		(Rvec2_t) { tex_coords.start.x, tex_coords.start.y },
		color, tex_index, local_mat_index);

	fill_vertex_data(quad, 1,
		(Rvec3_t) { 1.f, 0.f, 0.f },
		(Rvec2_t) { tex_coords.start.x + tex_coords.offset.x,
					tex_coords.start.y },
		color, tex_index, local_mat_index);

	fill_vertex_data(quad, 2,
		(Rvec3_t) { 1.f, 1.f, 0.f },
		(Rvec2_t) { tex_coords.start.x + tex_coords.offset.x,
					tex_coords.start.y + tex_coords.offset.y },
		color, tex_index, local_mat_index);

	fill_vertex_data(quad, 3,
		(Rvec3_t) { 1.f, 1.f, 0.f },
		(Rvec2_t) { tex_coords.start.x + tex_coords.offset.x,
					tex_coords.start.y + tex_coords.offset.y },
		color, tex_index, local_mat_index);

	fill_vertex_data(quad, 4,
		(Rvec3_t) { 0.f, 1.f, 0.f },
		(Rvec2_t) { tex_coords.start.x,
					tex_coords.start.y + tex_coords.offset.y },
		color, tex_index, local_mat_index);

	fill_vertex_data(quad, 5,
		(Rvec3_t) { 0.f, 0.f, 0.f },
		(Rvec2_t) { tex_coords.start.x, tex_coords.start.y },
		color, tex_index, local_mat_index);
}

void renderer_draw_quadc(renderer_t *renderer_p, texcoords_data tex_coords, Rvec3_t color,
										  unsigned int tex_id, unsigned int local_mat_index)
{
	vertex_t res[QUAD_VERT];
	float tex_index;

	tex_index = get_tex_index(renderer_p, tex_id);
	fill_quad_data(res, tex_coords, color, tex_index, (float)local_mat_index);
	renderer_add_quad(renderer_p, res);
}

void renderer_draw_quad(renderer_t *renderer_p, unsigned int tex_id, unsigned int local_mat_index)
{
	renderer_draw_quadc(renderer_p, (texcoords_data) { .offset.x = 1.f, .offset.y = 1.f,
												.start.x = 0.f, .start.y = 0.f },
									(Rvec3_t) { 1.f, 1.f, 1.f }, tex_id, local_mat_index);
}

static void renderer_submit_vert(renderer_t *renderer_p)
{
	glBindBuffer(GL_ARRAY_BUFFER, renderer_p->vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, renderer_p->vert.quad_count * (QUAD_VERT * sizeof(vertex_t)),
					renderer_p->vert.vert);
}

int renderer_load_tex(renderer_t *renderer_p, unsigned int *tex_p, const char *tex_dir)
{
	int width, height, color_channel;
	unsigned char *data;
	unsigned int tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	data = stbi_load(tex_dir, &width, &height, &color_channel, 0);
	ASSERT(data, "Failed to load texture\n");
	if (color_channel == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else if (color_channel == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	memcpy(tex_p, &tex, sizeof(unsigned int));
	return TRUE;
}

void renderer_draw(renderer_t *renderer_p)
{
	renderer_submit_vert(renderer_p);
	bind_shader(renderer_p->shader_id);
	renderer_set_samplers(renderer_p);
	renderer_submit_mvp(renderer_p);
	renderer_submit_textures(renderer_p);
	renderer_draw_call(renderer_p);
	renderer_flush(renderer_p);
	renderer_p->dbg.draw_count++;
}

void renderer_end_loop(renderer_t *renderer_p)
{
	renderer_p->dbg.draw_count = 0;
}

void renderer_end(renderer_t *renderer_p)
{
	free(renderer_p->vert.vert);
	free(renderer_p->mvp.local_mat_arr);
	glDeleteVertexArrays(1, &(renderer_p->vao));
	glDeleteBuffers(1, &(renderer_p->vbo));
	destroy_shader(renderer_p->shader_id);
}