#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"

typedef struct s_g{
    int w ;
    int h ;
    int l ;
    char a;
    char d;
    int i;
    int j;
    int dw;
    char **bd;
}t_g;

void free_board(t_g *g){
    if(g->bd){
        for( int i = 0; i < g->h; i++){
            if(g->bd[i])
                free(g->bd[i]);
        }
    }
    free(g->bd);
}

int init_game(t_g *g, char **av){
  g->w = atoi(av[1]);
  g->h = atoi(av[2]);
  g->l = atoi(av[3]);
  if(g->w <= 0 || g->h <= 0 || g->l < 0)
    return -1;
  g->a = '0';
  g->d = ' ';
  g->i = 0;
  g->j = 0;
  g->dw = 0;
  g->bd = (char **)malloc(g->h * sizeof(char *));
  if(!g->bd)
    return -1;
  for(int i = 0; i < g->h; i++){
    g->bd[i] = malloc(g->w * sizeof(char));
    if(!g->bd[i]){free_board(g); return -1;}
    for(int j = 0; j < g->w; j++){
      g->bd[i][j] = ' ';
    }
  }
  return 0;
}

void fill_board(t_g *g){
  char bu;
  int flag = 0;
  while(read(STDIN_FILENO, &bu, 1) == 1){
    switch(bu){
      case 'w':
        if(g->i > 0)
          g->i--;
        break;
      case 's':
        if(g->i < g->h - 1)
          g->i++;
        break;
      case 'a':
        if(g->j > 0)
          g->j--;
        break;
      case 'd':
        if(g->j < g->w - 1)
          g->j++;
        break;
      case 'x':
        g->dw = !g->dw;
        break;
      default:
        flag = 1;
        break;
    }
    if(g->dw && flag == 0){
      if(g->i >= 0 && g->i < g->h && g->j >= 0 && g->j < g->w){
        g->bd[g->i][g->j] = g->a;
      }
    }
  }
}

int count_ne(t_g *g, int x, int y){
  int count = 0;
  for(int i = -1; i <= 1; i++){
    for(int j = -1; j <= 1; j++){
      if(i == 0 && j == 0)
        continue;
      int ni = x + i;
      int nj = y + j;
      if(ni >= 0 && ni < g->h && nj >= 0 && nj < g->w){
        if(g->bd[ni][nj] == g->a)
          count++;
      }
    }
  }
  return count;
}

void print_b(t_g *g){
  for(int i = 0; i < g->h; i++){
    for(int j = 0; j < g->w; j++){
      putchar(g->bd[i][j]);
    }
    putchar('\n');
  }
}

int play(t_g *g){
  char **tmp = (char **)malloc(g->h * sizeof(char *));
  if(!tmp)
    return -1;
  for(int i = 0; i < g->h; i++){
    tmp[i] = malloc(g->w * sizeof(char));
    if(!tmp[i]){free_board(g); return -1;}
    for(int j = 0; j < g->w; j++){
      tmp[i][j] = ' ';
    }
  }
  for(int i = 0; i < g->h; i++){
    for(int j = 0; j < g->w; j++){
      int ne = count_ne(g, i, j);
      if(g->bd[i][j] == g->a){
        if(ne ==2 || ne == 3)
          tmp[i][j] = g->a;
        else 
          tmp[i][j] = g->d;
      }else{
        if(ne == 3)
          tmp[i][j] = g->a;
        else 
          tmp[i][j] = g->d;
      }
    }
  }
  free_board(g);
  g->bd = tmp;
  return 0;
}
  

int main(int ac, char **av){
    if(ac != 4){
      return 1;
    }
    t_g g;
    if(init_game(&g, av) == -1){
      return 1;
    }
    fill_board(&g);
    for(int i = 0; i < g.l; i++){
      if(play(&g) == -1){
        free_board(&g);
        return 1;
      }
    }
    print_b(&g);
    free_board(&g);
    return 0;
}
