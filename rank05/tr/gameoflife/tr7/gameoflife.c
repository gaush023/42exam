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
  char **bd;
} t_g;

void double_free(char **tab, int h){
  if(tab){
    for(int i = 0; i < h; i++){
      free(tab[i]);
    }
    free(tab);
  }
}

void fill_board(t_g *g){
  char bu;
  int flag = 0;
  while(read(STDIN_FILENO, &bu, 1) == 1){
    
  }
}

int inti_game(t_game *g, char **av){
  g->w = atoi(av[1]);
  g->h = atoi(av[2]);
  g->it = atoi(av[3]);
  if(g->w < 0 || g->h < 0 || g->it < 0)
    return -1;
  g->a = '0';
  g->d = ' ';
  g->bd = (char **)malloc(g->h * sizeof(char *));
  if(!g->bd)
    return -1;
  for(int j = 0; j < g->h){
    g->bd[j] =  (char *)malloc(g->w * sizeof(char));
    if(!g->bd[j]){doble_free(g->bd); return -1}
    for(int i = 0; i < g->i; i++){
      g->bd[j][i] = c->d;
    }
  }
  return 0;
}

int main(int ac, char **av){
  if(ac != 4)
      return 1;
  t_g g;
  if(init_game(&g, av) == -1))
      return 1;
  fill_board(&g);
  
   
}


