#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct s_e{
  int line_count;
  char empty;
  char obstacle;
  char full;
}t_e;

typedef struct s_m{
  int width;
  int height;
  char **map;
}t_m;

typedef struct s_s{
  int i;
  int j;
  int size;
}t_s;


int l_e(FILE *file, t_e *e){
  int res = fscanf(file, "%d %c %c %c\n", &e->line_count, &e->empty, &e->obstacle, &e->full);
  if(res != 4 || e->line_count <= 0)
    return (-1);
  if(e->empty == e->obstacle || e->empty == e->full || e->obstacle == e->full)
    return (-1);
  if(e->empty < 32 || e->empty > 126 || e->obstacle < 32 || e->obstacle > 126 || e->full < 32 || e->full > 126)
    return (-1);
  return 0;
}

char *ft_strndup(const char *s, size_t n){
  char *dup = malloc(n + 1);
  if(!dup)
    return (NULL);
  for(size_t i = 0; i < n; i++)
    dup[i] = s[i];
  dup[n] = '\0';
  return (dup);
}

int validate_map(t_m *m, t_e *e){
  for(int i = 0; i < m->height; i++){
    for(int j = 0; j < m->width; j++){
      if(m->map[i][j] != e->empty && m->map[i][j] != e->obstacle)
        return (-1);
    }
  }
  return (0);
}

int load_map(FILE *file, t_e *e, t_m *m){
  m->height = e->line_count;
  m->map = malloc(sizeof(char*) * m->height);
  if(!m->map)
    return (-1);
  char *line = NULL;
  size_t len = 0;
  for(int i = 0; i < e->line_count; i++){
    int read = getline(&line, &len, file);
    if(read == -1)
      return free(line), (-1);
    if(line[read - 1] == '\n')
      line[--read] = '\0';
    m->map[i] = ft_strndup(line, read);
    if(!m->map[i])
      return free(line), (-1);
    if(i == 0){
      m->width = read;
    }else{
      if(read != m->width)
        return free(line), (-1);
    }
  }
  free(line);
  if(validate_map(m, e) == -1)
    return (-1);
  return 0;  
}

void free_map(t_m *m){
  for(int i = 0; i < m->height; i++){
    free(m->map[i]);
  }
  free(m->map);
}

int ft_min(int a, int b, int c){
  int min = a;
  if(b < min)
    min = b;
  if(c < min)
    min = c;
  return (min);
}

void find_big_sq(t_m *m, t_e *e, t_s *sq){
  int db[m->height][m->width];
  for(int i = 0; i < m->height; i++){
    for(int j = 0; j < m->width; j++){
      db[i][j] = 0;
    }
  }

  for(int i = 0; i < m->height; i++){
    for(int j = 0; j < m->width; j++){
      if(m->map[i][j] == e->obstacle){
        db[i][j] = 0;
      }else{
        if(i == 0 || j == 0){
          db[i][j] = 1;
        }else{
          db[i][j] = 1 + ft_min(db[i-1][j], db[i][j-1], db[i-1][j-1]);
        }
        if(db[i][j] > sq->size){
          sq->size = db[i][j];
          sq->i = i;
          sq->j = j;
        }
      }
    }
  }
}

void print_bsq(t_m *m, t_e *e, t_s *sq){
  for(int i = sq->i; i < sq->i + sq->size; i++){
    for(int j = sq->j; j < sq->j + sq->size; j++){
      m->map[i][j] = e->full;
    }
  }
  for(int i = 0; i < m->height; i++){
    fprintf(stdout, "%s\n", m->map[i]);
  }
}

int execute_bsq(FILE *file){
  t_e e;  
  if(l_e(file, &e) == -1)
    return (-1);
  t_m m;
  if(load_map(file, &e, &m) == -1){
    free_map(&m);
    return (-1);
  }
  t_s sq;
  sq.size = 0;
  sq.i = 0;
  sq.j = 0;
  find_big_sq(&m, &e, &sq);
  print_bsq(&m, &e, &sq);
  free_map(&m);
  return 0;
}

int file_p(char *filename){
  FILE *file = fopen(filename, "r");
  if(!file){
    fprintf(stderr, "map error\n");
    return (1);
  }
  if(execute_bsq(file) == -1){
    fprintf(stderr, "map error\n");
    fclose(file);
    return (1);
  }
  fclose(file);
  return (0);
}

int main(int ac, char **av){
  if(ac == 1){
    if(execute_bsq(stdin) == -1){
      fprintf(stderr, "map error\n");
      return (1);
    }
  } else if (ac == 2){
    if(file_p(av[1]) == 1)
      return (1);
  } else {
    for(int i = 1; i < ac; i++){
      if(file_p(av[i]) == 1)
        return (1);
      if(i < ac - 1)
        printf("\n");
    }
  }
  return (0);
}
