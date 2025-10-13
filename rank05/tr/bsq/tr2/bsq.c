#include "bsq.h"

int load_el(FILE *file, t_el *el)
{
  int ret = fscanf(file, "%d %c %c %c", &(el->lines), &(el->empty), &(el->mark), &(el->full));

  if((ret != 4))
    return -1;
  if(el->lines <= 0)
    return -1;
  if(el->empty == el-> mark || el->empty == el->full || el->full == el->mark)
    return -1;
  if(el->empty < 32 || el->empty >126)
    return -1;
  if(el->mark < 32 || el->mark > 126)
    return -1;
  if(el->full < 32 || el->full > 126)
    return -1;
  return 0;
}

void free_map(char **m)
{
  int i = 0;
  if(m)
  {
    while(m[i] != NULL){
      if(m[i]) 
        free(m[i]);
      i++;
    }
    free(m);
  }
}

char *ft_substr(char *s, int start, int len)
{
  char *tmp = (char *)malloc(len + 1);
  if(!tmp)
    return NULL;
  int i = 0;
  int j = 0;
  while(s[i]){
    if((i >= start) && (j < len))
    {
      tmp[j] = s[i];
      j++;
    }
    i++;
  }
  tmp[j] = '\0';
  return tmp;
}

int el_che(char **m, char c1, char c2){
  int i = 0;
  while(m[i]){
    int j = 0;
    while(m[i][j]){
      if((m[i][j] != c1) && (m[i][j] != c2))
        return -1;
      j++;
    }
    i++;
  }
  return 0;
}

int load_map(FILE *file, t_el *el, t_map *m)
{
  m->h = el->lines;
  m->grid = (char **)malloc((m->h + 1) * (sizeof(char *)));
  m->grid[m->h] = NULL;

  char *line = NULL;
  size_t len = 0;
  
  if(getline(&line, &len, file) == -1)
  {
    free_map(m->grid);
    return -1;
  }
  
  for(int i = 0; i < m->h; i++){
    int r = getline(&line, &len, file);
    if (r == -1)
    {
      free(line);
      free_map(m->grid);
      return -1;
    }
    if(line[r - 1] == '\n')
      r--;
    else{
      free(line);
      free_map(m->grid);
      return -1;
    }
    m->grid[i] = ft_substr(line, 0, r);
    if(!(m->grid[i])){
      free(line);
      free_map(m->grid);
      return -1;
    }
    if(i == 0)
      m->w = r;
    else{
      if(m->w != r){
        free(line);
        free_map(m->grid);
        return -1;
      }
    }
  }
  
  if(el_che(m->grid, el->empty, el->mark) == -1){
    free(line);
    free_map(m->grid);
    return -1;
  }
  free(line);
  return 0;
}

int find_min(int n1, int n2, int n3)
{
  int min = n1;

  if(min > n2)
    min = n2;
  if(min > n3)
    min = n3;
  return min;
}

void find_big_sq(t_map *m, t_sq *sq, t_el *el){
  int res[m->h][m->w];
  for(int i = 0; i < m->h; i++)
  {
    for(int j = 0; j < m->w; j++)
      res[i][j] = 0;
  }

  for(int i = 0; i < m->h; i++)
  {
    for(int j = 0; j < m->w; j++)
    {
      if(m->grid[i][j] == el->mark)
        res[i][j] = 0;
      else if(i == 0 || j == 0)
        res[i][j] = 1;
      else {
        int min = find_min(res[i-1][j], res[i-1][j-1], res[i][j -1]);
        res[i][j] = min + 1;
      }

      if(res[i][j] > sq->size)
      {
        sq->size = res[i][j];
        sq->i = i - sq->size + 1;
        sq->j = j - sq->size + 1;
      }
    }
  }
}

void print_filled_sq(t_map *m, t_sq *s, t_el *e)
{
  for(int i = s->i; i < s->i + s->size; i++)
  {
    for(int j = s->j; j < s->j + s->size; j++)
    {
      if((i < m->h) && ( j < m->w))
        m->grid[i][j] = e->full;
    }
  }

  for(int i = 0; i < m->h; i++)
    fprintf(stdout, "%s\n", m->grid[i]);
}

int execute_bsq(FILE *file)
{
  t_el el;
  if(load_el(file, &el) == -1)
    return -1;

  t_map m;
  if(load_map(file, &el, &m) == -1)
    return -1;

  t_sq sq;
  sq.size = 0;
  sq.i = 0;
  sq.j = 0;

  find_big_sq(&m, &sq, &el);
  print_filled_sq(&m, &sq, &el);
  free(m.grid);
  return 0;
}

int file_pointer(char *name)
{
  FILE *file = fopen(name, "r");
  if(!file)
    return -1;
  int ret = execute_bsq(file);
  fclose(file);
  return ret;
}

