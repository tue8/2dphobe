#ifndef TEXT_H_
#define TEXT_H_

#include "obj.h"

#define TEXT_WHITE (Rvec3_t) { 1.f, 1.f, 1.f }
#define TEXT_BLACK (Rvec3_t) { 0.f, 0.f, 0.f }
#define TEXT_RED (Rvec3_t) { 1.f, 0.f, 0.f }
#define TEXT_GREEN (Rvec3_t) { 0.f, 1.f, 0.f }
#define TEXT_BLUE (Rvec3_t) { 0.f, 0.f, 1.f }
#define TEXT_RGB(r, g, b) (Rvec3_t) { r / 255.f, g / 255.f, b / 255.f }

int text_init(renderer_t *);
void render_text(renderer_t *, char *, Rvec3_t, vec3, vec3);

#endif