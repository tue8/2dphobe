#include "shader.h"
#include "renderer.h"
#include "../utils/utils.h"

#include <glad/glad.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

const char *vertshader_src =
"#version 430 core\n"
"layout (location = 0) in vec3 vpos;\n"
"layout (location = 1) in vec2 vtexcoord;\n"
"layout (location = 2) in float vtexid;\n"
"layout (location = 3) in float vlocal_mat_index;\n"
"layout (location = 4) in vec3 vcolor;\n"
"layout (std430, binding = 0) buffer vlocal_mats\n"
"{\n"
"    mat4 models[];\n"
"};\n"
"uniform mat4 view;\n"
"uniform mat4 proj;\n"

"out vec2 ftexcoord;\n"
"out float ftexid;\n"
"out vec3 fcolor;\n"

"void main()\n"
"{\n"
"   int index = int(vlocal_mat_index);\n"
"   gl_Position = proj * view * models[index] * vec4(vpos, 1.0);\n"
"   ftexcoord = vtexcoord;\n"
"   ftexid = vtexid;\n"
"   fcolor = vcolor;\n"
"}\0";

const char *fragshader_src =
"#version 430 core\n"
"in vec2 ftexcoord;\n"
"in float ftexid;"
"in vec3 fcolor;"

"uniform sampler2D texs[%d];\n"

"out vec4 fo;\n"

"void main()\n"
"{\n"
"   int index = int(ftexid);\n"
"   fo = vec4(fcolor, 1.f) * texture(texs[index], ftexcoord);\n"
"}\n\0";

int bind_shader(unsigned int id)
{
  glUseProgram(id);
  return FALSE;
}

int destroy_shader(unsigned int id)
{
  glDeleteProgram(id);
  return FALSE;
}

int init_shader(void *R_void)
{
  renderer_t *renderer_p;
  char link_buff[512];
  char *fragshader_buff;
  unsigned int vertshader, fragshader;
  
  /* forgive me lord */
  renderer_p = (renderer_t *)R_void;
  vertshader = glCreateShader(GL_VERTEX_SHADER);
  fragshader = glCreateShader(GL_FRAGMENT_SHADER);
  /* will the number of maximum amount of texture slots ever go beyond 2 digits? i dont know :P */
  fragshader_buff = malloc(strlen(fragshader_src) + 1);

  sprintf(fragshader_buff, fragshader_src, renderer_p->mvars.max_tex);
  create_shader(vertshader, &vertshader_src);
  create_shader(fragshader, &fragshader_buff);
  renderer_p->shader_id = glCreateProgram();
  glAttachShader(renderer_p->shader_id, vertshader);
  glAttachShader(renderer_p->shader_id, fragshader);
  ASSERT(link_shader(renderer_p->shader_id, link_buff), "Link error: %s\n", link_buff);
  glDeleteShader(vertshader);
  glDeleteShader(fragshader);
  free(fragshader_buff);
  bind_shader(0);
  return TRUE;
}

int create_shader(unsigned int id, const char **src)
{
  int res;
  glShaderSource(id, 1, src, NULL);
  glCompileShader(id);
  glGetShaderiv(id, GL_COMPILE_STATUS, &res);
  return res;
}

int link_shader(unsigned int id, char *err_buffer)
{
  int res;
  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &res);
  if (!res)
    glGetProgramInfoLog(id, 512, NULL, err_buffer);
  return res;
}

void set_uniform_mat4(unsigned int shader_id, const char *name, mat4 mat)
{
  glUniformMatrix4fv(glGetUniformLocation(shader_id, name), 1, GL_FALSE, mat);
}

void set_uniform_4f(unsigned int shader_id, const char *name,
  float f1, float f2, float f3, float f4)
{
  glUniform4f(glGetUniformLocation(shader_id, name), f1, f2, f3, f4);
}