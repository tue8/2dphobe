#include "utils.h"
#include <stdio.h>

char *working_dir(const char *dir)
{
  char curr_dir[255];
  if (GetCurrentDirectoryA(sizeof(curr_dir), &curr_dir) == 0)
  {
    fprintf(stderr, "nahhh\n");
    return NULL;
  }
  char *new_dir = malloc(strlen(curr_dir) + strlen(dir) + 1);
  sprintf(new_dir, "%s%s", curr_dir, dir);
  return new_dir;
}