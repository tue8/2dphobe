#include "text.h"
#include "utils/utils.h"
#include <string.h>

#define CHAR_COUNT 84
#define CHAR_OFFSET 18.f
#define FONT_TEX_SIZE_X 574.f
#define FONT_TEX_SIZE_Y 55.f

#define SET_COORDS(c_x, c_y) \
{ .offset.x = CHAR_OFFSET / FONT_TEX_SIZE_X, .offset.y = CHAR_OFFSET / FONT_TEX_SIZE_Y, \
  .start.x = c_x / FONT_TEX_SIZE_X, .start.y = c_y / FONT_TEX_SIZE_Y } \

typedef struct
{
	char c;
	texcoords_data coords;
} char_texcoords_data;

unsigned int font_tex_id;
const char_texcoords_data char_texcoords[CHAR_COUNT];
const char *font_tex_dir = "\\Breakout\\data\\fonts\\font.png";

static void setup_row(int *i, char c, int char_range, float x_offset, float y_offset)
{
	int i_range;

	i_range = *i + char_range;
	for (; *i <= i_range; (*i)++)
	{
		char_texcoords_data d;
		d = (char_texcoords_data){ c, SET_COORDS(x_offset, y_offset)};
		memcpy(char_texcoords + (*i), &d, sizeof(char_texcoords_data));
		c++;
		x_offset += CHAR_OFFSET;
	}
}

static void setup_char(int *i, char c, float x_offset, float y_offset)
{
	memcpy(char_texcoords + (*i), &((char_texcoords_data) { c, SET_COORDS(x_offset, y_offset) }),
		sizeof(char_texcoords_data));
	(*i)++;
}

int text_init(renderer_t *renderer_p)
{
	int row_index;

	row_index = 0;
	setup_row(&row_index, 'a', (int)('z' - 'a'), 0.f, 0.f);
	setup_row(&row_index, 'A', (int)('Z' - 'A'), 0.f, 19.f);
	setup_row(&row_index, '0', (int)('9' - '0'), 0.f, 37.f);

	setup_char(&row_index, ' ', 556.f, 0.f);
	setup_char(&row_index, '.', 182.f, 37.f);
	setup_char(&row_index, ':', 198.f, 37.f);
	setup_char(&row_index, ',', 216.f, 37.f);
	setup_char(&row_index, ';', 234.f, 37.f);
	setup_char(&row_index, '(', 252.f, 37.f);
	setup_char(&row_index, '*', 271.f, 37.f);
	setup_char(&row_index, '!', 288.f, 37.f);
	setup_char(&row_index, '?', 306.f, 37.f);
	setup_char(&row_index, ')', 378.f, 37.f);
	setup_char(&row_index, '#', 396.f, 37.f);
	setup_char(&row_index, '$', 414.f, 37.f);
	setup_char(&row_index, '%', 468.f, 37.f);
	setup_char(&row_index, '&', 504.f, 37.f);
	setup_char(&row_index, '-', 524.f, 37.f);
	setup_char(&row_index, '+', 540.f, 37.f);
	setup_char(&row_index, '@', 556.f, 37.f);

	if (!renderer_load_tex(renderer_p, &font_tex_id, font_tex_dir))
		return FALSE;
	return TRUE;
}

static texcoords_data find_charcoords(char c)
{
	for (int i = 0; i < CHAR_COUNT; i++)
		if (char_texcoords[i].c == c)
			return char_texcoords[i].coords;
	return (texcoords_data) SET_COORDS(0.f, 0.f);
}

void render_text(renderer_t *renderer_p, char *str, Rvec3_t Rv_color, vec3 v_pos, vec3 v_scale)
{
	for (int i = 0; i < strlen(str); ++i)
	{
		mat4 model;
		int model_index;

		glm_mat4_identity(model);
		glm_translate(model, (vec3) { v_pos[v_x] + (v_scale[v_z] * i), v_pos[v_y], 0.f });
		glm_scale(model, BREAK_VEC3(v_scale));

		renderer_push_model(renderer_p, model, &model_index);
		renderer_draw_quadc(renderer_p, find_charcoords(*(str + i)), Rv_color, font_tex_id, model_index);
	}
}