#include "bsq.h"

int load_el(FILE *file, t_el *e)
{
  int res = fscanf(file, "%d %c %c %c", &(e->line), &(e->em), &(e->ob), &(e->full));

  if(res != 4)
    return -1;
  if(e->em == e->ob || e->em == e->full || e->ob == e->full)
    return -1;
  if(e->em < 32 || e->em > 126)
    return -1;
  if(e->ob < 32 || e->ob > 126)
    return -1;
  if(e->full < 32 || e->full >126)
    return -1;
  return 0;
}

void free_map(char **line){
  if(line){
    int j = 0;
    while(line[j] != NULL){
      if(line[j])
          free(line[j]);
      j++;
    }
    free(line);
  }
}

int double_free(char *line, t_ma *m)
{
  free(line);
  free_map(m->g);
  return -1;
}

char *ft_substr(char *line, int start, int len)
{
  char *tmp;
  tmp = (char *)malloc((len + 1));
  if(!tmp)
    return NULL;
  int j =0;
  for(int i = 0; i < len; i++)
  {
    if((i >= start) && (j < len)){
      tmp[j] = line[i];
      j++;
    }
  }
  tmp[j] = '\0';
  return tmp;
}

int check_el(char **m, char c1, char c2){
  for(int i = 0; m[i]; i++){
    for(int j = 0; m[i][j]; j++){
      if(m[i][j] != c1 && m[i][j] != c2)
        return -1;
    }
  }
  return 0;
}
        

int load_m(FILE *file, t_el *e, t_ma *m){
  m->h = e->line;
  m->g = (char **)malloc((m->h + 1) * (sizeof(char *)));
  m->g[m->h] = NULL;

  char *line = NULL;
  size_t n = 0;

  if(getline(&line, &n, file) == -1)
  {
    free_map(m->g);
    return -1;
  }

  for(int i = 0; i < m->h; i++){
    int res = getline(&line, &n, file);
    if(res == -1) {return double_free(line, m);}
    if(line[res-1] == '\n')
      res--;
    else {
      return double_free(line, m);
    }
    m->g[i] = ft_substr(line, 0, res);
    if(!m->g[i])
      return double_free(line, m);
    if(i == 0)
      m->w = res;
    else {
      if(m->w != res)
        return double_free(line, m);
    }
  }
  if(check_el(m->g, e->em, e->ob) == -1)
      return double_free(line, m);
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

void find_big_sq(t_el *e, t_ma *m, t_sq *s) {
  int res[m->h][m->w];
  for(int i = 0; i < m->h; i++){
      for(int j = 0; j < m->w; j++)
        res[i][j] = 0;
  }

  for(int i = 0; i < m->h; i++){
    for(int j = 0; j < m->w; j++){
        if(m->g[i][j] == e->ob)
          res[i][j] = 0;
        else if(i == 0 || j == 0)
          res[i][j] = 1;
        else{
          int min = find_min(res[i-1][j], res[i-1][j-1], res[i][j-1]);
          res[i][j] = min +1;
        }
        
        if(res[i][j] > s->size){
          s->size = res[i][j];
          s->i = i - s->size + 1;
          s->j = j - s->size + 1;
        }
    }
  }
}

void print_filled_sq(t_el *e, t_ma *m, t_sq *s)
{
  for(int i = s->i; i < s->i + s->size; i++){
    for(int j = s->j; j < s->j + s->size; j++){
      if((i < m->h) && (j < m->w))
        m->g[i][j] = e->full;
    }
  }

  for(int i = 0; i < m->h; i++)
    fprintf(stdout, "%s\n", m->g[i]);
}

int execute_bsq(FILE *file){
  t_el e;
  if(load_el(file, &e) == -1)
    return -1;

  t_ma m;
  if(load_m(file, &e, &m) == -1)
    return -1;

  t_sq sq;
  sq.size = 0;
  sq.i = 0;
  sq.j = 0;
  find_big_sq(&e, &m, &sq);
  print_filled_sq(&e, &m, &sq);
  free_map(m.g);
  return 0;
}

int file_pointer(char *name){
  FILE *file = fopen(name, "r");
  if(!file)
    return -1;
  int res = execute_bsq(file);
  fclose(file);
  return res;
}

