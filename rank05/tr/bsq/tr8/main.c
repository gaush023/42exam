#include <stdio.h>
#include <stdlib.h>


typedef struct s_e{
  int l;
  char a;
  char b;
  char c;
} t_e;

typedef struct s_m{
  int h;
  int w;
  char **m;
} t_m;

typedef struct t_s{
  int size;
  int i;
  int j;
} t_s;

int l_e(FILE *file, t_e *e){
  int res = fscanf(file, "%d %c %c %c", &e->l, &e->a, &e->b, &e->c);
  
  if(res != 4 || e->l < 2)
    return -1;
  if(e->a < 32 || e->a > 126)
    return -1;
  if(e->b < 32 || e->b > 126)
    return -1;
  if(e->c < 32 || e->c > 126)
    return -1;
  if(e->a == e->b || e->a == e->c || e->b == e->c)
    return -1;
  return 0;
}

void double_free(char *line, t_m *m){
  if(line)
      free(line);
  if(m->m){
    for(int i = 0; i < m->h; i++){
      if(m->m[i])
        free(m->m[i]);
    }
    free(m->m);
  }

}

char *copy_line(char *line, int res){
  char *tmp;
  tmp = (char *)malloc(res + 1 * sizeof(char));
  if(!tmp)
    return NULL;
  for(int i = 0; i < res; i++)
    tmp[i] = line[i];
  tmp[res] = '\0';
  return tmp;
}

int check_m(t_m *m, char c1, char c2){
  for(int i = 0; i < m->h; i++){
    for(int j = 0; j < m->w; j++){
      if(m->m[i][j] != c1 && m->m[i][j] != c2)
        return -1;
    }
  }
  return 0;
}

int find_min(int a, int b, int c){
  int min = a;
  if(b < min)
    min = b;
  if(c < min)
    min = c;
  return min;
}

void find_b(t_e *e, t_m *m, t_s *s){
    int mx[m->h][m->w];
    for(int i = 0; i < m->h; i++){
      for(int j = 0; j < m->w; j++)
        mx[i][j] = 0;
    }
    for(int i = 0; i < m->h; i++){
      for(int j = 0; j < m->w; j++){
        if(m->m[i][j] == e->b )
          mx[i][j] = 0;
        else {
          if(i == 0 || j == 0){
            mx[i][j] = 1;
          } else {
          mx[i][j] = 1 + find_min(mx[i - 1][j], mx[i-1][j-1], mx[i][j -1]);
          }
          if(mx[i][j] > s->size){
            s->size = mx[i][j];
            s->i = i - s->size + 1;
            s->j = j - s->size + 1;
          }
        }
      }
    }
    for(int i = s->i; i < s->i + s->size; i++){
      for(int j = s->j; j < s->j + s->size; j++){
          m->m[i][j] = e->c;
      }
    }
    for(int i = 0; i < m->h; i++){
      printf("%s\n", m->m[i]);
    }
}

int l_m(FILE *file, t_e *e, t_m *m){
  m->h = e->l;
  m->m = (char **)malloc(m->h * sizeof(char *));
  if(!m->m)
      return -1;

  char *line = NULL;
  size_t len = 0;
  getline(&line, &len, file); // to consume the newline after the first line
  for(int i = 0; i < m->h; i++){
    ssize_t res = getline(&line, &len, file);
    if(res == -1){ double_free(line, m); return -1;}
    if(line[res -1] == '\n')
      line[res--] = '\0';
    else {
      return -1;
    }
    m->m[i] = copy_line(line, res);
    if(!m->m[i]){ double_free(line, m); return -1;}
    if(i == 0)
      m->w = res;
    else {
      if(m->w != res){double_free(line, m); return -1;}
    }
  }
  free(line);
  if(check_m(m, e->a, e->b) == -1){ double_free(NULL, m); return -1;}
  return 0;
}

int e_b(FILE *file){
  t_e e;
  if(l_e(file, &e) == -1) { printf("Error: WTF, follw file rules\n"); return -1;}
  t_m m;
  if(l_m(file, &e, &m) == -1) { printf("Error: What's this??\n"); return -1;}
  t_s s;
  s.size = 0;
  s.i = 0;
  s.j = 0;
  find_b(&e, &m, &s);
  double_free(NULL, &m);
  return 0;
}

int r_f(char *name){
  FILE *file;
  file = fopen(name, "r");
  if(!file){printf("Error: cannot laod file"); return -1;}
  return e_b(file);
}


int main(int ac, char **av){
  if(ac == 1){
    if(e_b(stdin) == -1)
      return 1;
  } else {
    for(int i  = 1; i < ac; i++){
      if(r_f(av[i]) == -1){
        return 1;
      }
      if(i < ac -1)
        printf("\n");
    }  
  }
}
