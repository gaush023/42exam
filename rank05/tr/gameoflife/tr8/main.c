#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_g{
  int width;
  int height;
  int r;
  char a;
  char d;
  char **b;
  int i;
  int j;
  int flag;
} t_g;

void free_game(t_g *g){
  if(g->b){
    for(int i = 0; i <  g->height; i++){
            if(g->b[i])
              free(g->b[i]);
      }
    free(g->b);
    g->b = NULL;
    }
  }



int init_game(char **av, t_g *g){
  g->width = atoi(av[1]);
  g->height = atoi(av[2]);
  g->r = atoi(av[3]);
  g->a = '0';
  g->d = ' ';
  g->b = (char **)malloc(g->height * sizeof(char *));
  if(!g->b)
    return -1;
  for(int i = 0; i < g->height; i++){
    g->b[i] = (char *)malloc((g->width) * sizeof(char));
    if(!g->b[i]){ free_game(g); return -1;}
    for(int j = 0; j < g->width; j++)
      g->b[i][j] = ' ';
  }
  g->i = 0;
  g->j = 0;
  g->flag = 0;
  return 0;
}

void fill_board(t_g *g){
  char buf;
  while(read(STDIN_FILENO, &buf, 1) == 1){
    switch(buf){
      case 'w':
        if(g->i > 0)
          g->i--;
        break;
      case 's':
        if(g->i < g->height -1)
          g->i++;
        break;
      case 'a':
        if(g->j > 0)
            g->j--;
        break;
      case 'd':
          if(g->j < g->width -1)
            g->j++;
          break;
      case 'x':
          g->flag = !(g->flag);
          break;
      default:
          break;
    }
    if(g->flag){
        if(g->i >= 0 && g->i < g->height && g->j >= 0 && g->j < g->width)
            g->b[g->i][g->j] = g->a;
    }
  }
}

int count_ne(t_g *g, int i, int j){
  int count = 0;
  for(int di = -1; di < 2; di++){
    for(int dj = -1; dj < 2; dj++){
      if(di == 0 && dj == 0)
        continue;
      int ni = di + i;
      int nj = dj + j;
      if (ni >= 0 && ni < g->height && nj >= 0 && nj < g->width){
        if(g->b[ni][nj] == g->a)
            count++;
      }
    }
  }
  return count;
}



int play(t_g *g){
  char **tmp = (char **)malloc((g->height) * sizeof(char *));
   if(!tmp)
    return -1;
  for(int i = 0; i < g->height; i++){
    tmp[i] = (char *)malloc((g->width + 1 ) * sizeof(char));
    if(!tmp[i]){ free(tmp); return -1;}
    for(int j = 0; j < g->width; j++)
      tmp[i][j] = ' ';
  }
   for(int i = 0; i < g->height; i++){
    for(int j = 0; j < g->width; j++){
      int ne = count_ne(g, i, j);
      if(g->b[i][j] == g->a){
        if(ne == 2 || ne == 3)
          tmp[i][j] = g->a;
        else 
          tmp[i][j] = g->d;
        } else {
          if(ne == 3)
            tmp[i][j] = g->a;
          else
            tmp[i][j] = g->d;
        }
      } 
   }
  free_game(g);
  g->b = tmp;
  return 0;
}

void print_b(t_g *g){
  for(int i = 0; i < g->height; i++){
    for(int j = 0; j < g->width; j++){
        putchar(g->b[i][j]);
      }
    putchar('\n');
  }
}

int main(int ac, char **av){
  if(ac != 4)
      return 1;
  t_g g;
  if(init_game(av, &g) == -1)
    return 1;
  fill_board(&g);
  for (int i = 0; i < g.r; i++) {
  if(play(&g) == -1){
    free_game(&g);
    return 1;
  }
  }
  print_b(&g);
  free_game(&g);
  return 0;
}


