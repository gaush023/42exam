#ifndef BSQ_H
#define BSQ_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_el
{
  int lines;
  char empty;
  char mark;
  char full;
} t_el;

typedef struct s_map
{
  char **grid;
  int w;
  int h;
} t_map;

typedef struct t_sq
{
  int size;
  int i;
  int j;
} t_sq;

int execute_bsq(FILE *file);
int file_pointer(char *name);

#endif
