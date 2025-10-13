#ifndef BSQ_H
#define BSQ_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_el
{
  int lines;
  char empty;
  char ob;  
  char full;
} t_el;

typedef struct s_map
{
  char **grid;
  int width;
  int height;
} t_map;

typedef struct s_sq
{
  int size;
  int i;
  int j;
} t_sq;



int execute_bsq(FILE* file);
int convert_file_pointer(char* name);

#endif
