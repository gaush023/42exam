#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_g{
  int w;
  int h;
  int it;
  char a;
  char d;
  int i;
  int j;
  int dw;
  char **bd;
} t_g;

void free_b(t_g *g){
  if(g->bd){
      for(int i = 0; i < g->h; i++){
       if(g->bd[i])
          free(g->bd[i]);
      }
    free(g->bd);
  }
}

int init_game(char **av, t_g *g){
  g->w = atoi(av[1]);
  g->h = atoi(av[2]);
  if(g->w == 0 || g->h == 0)
      return -1;
  g->it = atoi(av[3]);
  g->a = '0';
  g->d = ' ';
  g->i = 0;
  g->j = 0;
  g->dw = 0;
  g->bd = (char **)malloc(g->h * sizeof(char *));
  if(!g->bd){return -1;}
  for(int i = 0; i < g->h; i++){
    g->bd[i] = (char *)malloc(g->w * sizeof(char));
    if(!g->bd[i]){free_b(g); return -1;}
      for(int j = 0; j < g->w; j++){
        g->bd[i][j] = ' ';
      }
  }
  return 0;
}

void fill_board(t_g *g){
  char bu;
  while(read(STDIN_FILENO, &bu, 1) == 1){
  int flag =0;
    switch(bu){
      case 'w': if( g->i > 0) g->i--; break;
      case 's': if( g->i < g->h - 1) g->i++; break;
      case 'a': if( g->j > 0) g->j--; break;
      case 'd': if( g->j < g->w - 1) g->j++; break;
      case 'x': g->dw = !g->dw; break;
      default: flag = 1; break;
    }
    if(g->dw && flag == 0){
      if(g->i >= 0 && g->i < g->h && g->j >= 0 && g->j < g->w)
        g->bd[g->i][g->j] = g->a;
    }
  }
}

void double_free(char **tmp, t_g *g){
  if(tmp){
    for(int i = 0; i < g->h; i++){
      if(tmp[i])
          free(tmp[i]);
    }
    free(tmp);
  }
}

int count_ne(t_g *g, int i, int j){
  int count = 0;  
  for(int di = -1; di < 2; di++){
    for(int dj = -1; dj < 2; dj++){
      if(di == 0 && dj == 0)
          continue;
      int ni = i + di;
      int nj = j + dj;
      if(ni >= 0 && ni < g->h && nj >= 0 && nj < g->w){
          if(g->bd[ni][nj] == g->a)
              count++;
      }
    }
  }
  return count;
}

int play(t_g *g){
  char **tmp = (char **)malloc(g->h * sizeof(char *));
  if(!tmp){free_b(g); return -1;}
  for(int i = 0; i < g->h; i++){
    tmp[i] = malloc(g->w * sizeof(char));
    if(!tmp[i]){ double_free(tmp, g);}
  }
  for(int i = 0; i < g->h; i++){
    for(int j = 0; j < g->w; j++){
      int ne = count_ne(g, i, j);
      if(g->bd[i][j] == g->a){
        if(ne == 2 || ne == 3)
            tmp[i][j] = g->a;
        else {
            tmp[i][j] = g->d;
        }
      } else {
          if(ne == 3)
            tmp[i][j] = g->a;
          else {
            tmp[i][j] = g->d;
          }
      }
    }
  }
  free_b(g);
  g->bd = tmp;
  return 0;
}

void print_b(t_g *g){
  for(int i = 0; i < g->h; i++){
    for(int j = 0; j < g->w; j++){
        putchar(g->bd[i][j]);
    }
    putchar('\n');
  }
}


int main(int ac, char **av){
  t_g g;
  if(ac != 4)
      return 1;
  if(init_game(av, &g) == -1)
      return 1;
  fill_board(&g);
  for(int i = 0; i < g.it; i++){
    if(play(&g) == -1)  
      return 1;
  }
  print_b(&g);
  free_b(&g);
  return 0;
}
