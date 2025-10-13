#ifndef BSQ_H
#define BSQ_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_el{
  int line;
  char em;
  char ob;
  char full;
} t_el;

typedef struct s_ma{
  char **g;
  int w;
  int h;
} t_ma;

typedef struct s_sq{
  int size;
  int i;
  int j;
} t_sq;

int file_pointer(char *name);

int execute_bsq(FILE *file);

#endif
