#ifndef PHOBE_H_
#define PHOBE_H_

#include <GLFW/glfw3.h>
#include "obj.h"
#include "renderer/renderer.h"
#include "text.h"
#include "utils/utils.h"

typedef struct
{
  int key;
  int is_held;
  int is_pressed;
} phobe_input;

typedef struct
{
  GLFWwindow *window;
  phobe_input input[1024];
  int queued_key;
  float last_frame;
  float last_sec_frame;
  int frame;
  float delta_time;
  int fps;
} phobe_context;


int phobe_init(phobe_context *context, char *name, unsigned int width, unsigned int height);
int phobe_shouldclose(phobe_context *context);
void phobe_startloop(phobe_context *context);
void phobe_endloop(phobe_context *context);
int phobe_key_pressed(phobe_context *context, int key);
int phobe_key_held(phobe_context *context, int key);

#endif