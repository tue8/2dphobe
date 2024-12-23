#ifndef TEXT_H_
#define TEXT_H_

#include "obj.h"

#define TEXT_WHITE (phobe_vec3) { 1.f, 1.f, 1.f }
#define TEXT_BLACK (phobe_vec3) { 0.f, 0.f, 0.f }
#define TEXT_RED (phobe_vec3) { 1.f, 0.f, 0.f }
#define TEXT_GREEN (phobe_vec3) { 0.f, 1.f, 0.f }
#define TEXT_BLUE (phobe_vec3) { 0.f, 0.f, 1.f }
#define TEXT_RGB(r, g, b) (phobe_vec3) { r / 255.f, g / 255.f, b / 255.f }

int text_init(phobe_renderer *);
void render_text(phobe_renderer *, char *, phobe_vec3, phobe_vec3, phobe_vec3);

#endif