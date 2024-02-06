#include "level.h"
#include "utils/utils.h"
#include <string.h>

static void init_brick(brick_t *brick_p, int destroyed, int solid)
{
    brick_p->legit = TRUE;
    brick_p->destroyed = destroyed;
    brick_p->solid = solid;
}

static int process_level(level_t *level_p, renderer_t *renderer_p, char *src, int width,
    int height, int row, int col)
{
    int brick_count;
    float brick_width;
    float brick_height;
    int x;
    int y;
    int i;

    brick_count = 0;
    brick_width = (float) width / (float) row;
    brick_height = (float) height / (float) col;
    i = 0;
    for (y = 0; y < col; y++)
    {        
        for (x = 0; x < row;)
        {
            char brick_type;

            level_p->bricks[brick_count].legit = FALSE;
            brick_type = *(src + i);
            if (brick_type == '1' || brick_type == '2')
            {
                int solid;
                solid = (brick_type == '2') ? TRUE : FALSE;

                obj_create(&(level_p->bricks[brick_count].base), renderer_p,
                    (vec3) { brick_width * x, brick_height * y, 0.f },
                    (vec3) { brick_width, brick_height, 0.f },
                    solid ? level_p->solid_tex : level_p->non_solid_tex);
                init_brick(&(level_p->bricks[brick_count]), FALSE, solid);
            }

            if (brick_type != '\n')
            {
                x++;
                brick_count++;
            }  
            i++;
        }
    }
    return TRUE;
}

static int level_load(level_t *level_p, renderer_t *renderer_p, char *src, int width, int height)
{
    int row;
    int col;
    char *curr_line;

    row = 0;
    col = 0;
    curr_line = _strdup(src);
    while (curr_line)
    {
        char *next_line = strchr(curr_line, '\n');
        if (next_line)
            *next_line = '\0';
        if (row == 0)
            row = strlen(curr_line);
        if (next_line)
            *next_line = '\n';
        curr_line = next_line ? (next_line + 1) : NULL;
        col++;
    }
    // printf("ROW: %d, COL: %d\n", row, col);
    free(curr_line);
    level_p->brick_count = row * col;
    level_p->bricks = malloc(sizeof(brick_t) * level_p->brick_count);
    return (row > 0 && col > 0) ?
        process_level(level_p, renderer_p, src, width, height, row, col) : FALSE;
}

int level_load_file(level_t *level_p, renderer_t *renderer_p, char *file_dir, int width, int height)
{
    FILE *f;
    size_t fsize;
    char *src;
    int res;

    f = fopen(file_dir, "r");
    if (f == NULL)
        return FALSE;
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    src = malloc(fsize);
    rewind(f);
    fread(src, sizeof(char), fsize, f);
    src[fsize - 2] = '\0';
    res = level_load(level_p, renderer_p, src, width, height);
    free(src);
    return res;
}