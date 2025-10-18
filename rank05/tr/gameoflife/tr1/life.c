#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_game{
  int w;
  int h;
  int r;
  char alive;
  char dead;
  int i;
  int j;
  int dw;
char **board;
} t_game;

void free_board(t_game *game){
   if(game->board){
        for(int i = 0; i < game->h; i++){
            if(game->board[i])
                free(game->board[i]);
        }
        free(game->board);
    }
}

int init_game(t_game *game, char **av){
  game->w = atoi(av[1]);
  game->h = atoi(av[2]);
  game->r = atoi(av[3]);
  game->alive = '0';
  game->dead = ' ';
  game->i = 0;
  game->j = 0;
  game->dw = 0;
  game->board = (char **)malloc((game->h) * sizeof(char *));
  if(!(game->board))
    return -1;
for(int i = 0; i < game->h; i++){
    game->board[i] = (char *)malloc((game->w) * sizeof(char));
    if(!(game->board[i])){
        free_board(game);
        return -1;
    } for (int j = 0; j < game->w; j++) {
      game->board[i][j] = ' ';
    }
  }
  return 0;
}

void free_game(t_game *game){
  if(game->board){
    for(int i = 0; i < game->h; i++){
      if(game->board[i])
          free(game->board[i]);
    }
    free(game->board);
  }
}

void fill_board(t_game *game){
  char buffer;
  int flag = 0;

  while(read(0, &buffer, 1) == 1)
  {
      switch(buffer){
        case 'w':
          if (game->i > 0)
            game->i--;
          break;
        case 's':
          if (game->i < (game->h - 1))
              game->i++;
          break;
        case 'a':
          if (game->j > 0)
            game->j--;
          break;
        case 'd':
          if (game->j < (game->w -1))
              game->j++;
          break;
        case 'x':
          game->dw = !(game->dw);
          break;
        default:
          flag = 1;
          break;
      }

      if(game->dw && (flag == 0))
      { 
          if((game->i >= 0) && (game->i < game->h) && (game->j >= 0) && (game->j < game->w))
              game->board[game->i][game->j] = game->alive; 
       
      }
    }
}

int count_neighbors(t_game *game, int i, int j){
        int count = 0;
        for(int di = -1; di < 2; di++){
            for(int dj = -1; dj < 2; dj++){
                if( di == 0 && dj == 0)
                    continue;
                int ni = i + di;
                int nj = j + dj;
                if((ni >= 0) && (ni < game->h) && (nj >= 0) && (nj < game->w)){
                    if(game->board[ni][nj] == game->alive)
                        count++;
                }
            }
        }
        return count;
}

int play(t_game *game){
    char **tmp = (char **)malloc((game->h) * sizeof(char *));
l   if(!tmp)
        return -1;

    for(int i = 0; i < game->h; i++){
        tmp[i] = (char *)malloc((game->w) * sizeof(char));
        if(!tmp[i])
            return -1;
    }
    for(int i = 0; i < game->h; i++){
        for(int j = 0; j < game->w; j++){
            int ne = count_neighbors(game, i, j);
            if(game->board[i][j] == game->alive){
                if(ne == 2 || ne == 3)
                    tmp[i][j] = game->alive;
                else
                    tmp[i][j] = game->dead;
            } else {
                if(ne == 3)
                    tmp[i][j] = game->alive;
                else   
                    tmp[i][j] = game->dead;
            }
        }
    }
    free_board(game);
    game->board = tmp;
    return 0;
}

void print_board(t_game *game){
    for(int i = 0; i < game->h; i++){
        for(int j = 0; j < game->w; j++)
            putchar(game->board[i][j]);
        putchar('\n');
    }
}

int main(int ac, char **av){
    if(ac != 4)
        return 1;
    t_game game;

    if(init_game(&game, av) == -1)
    return 1;
    fill_board(&game);

    for(int i = 0; i < game.r; i++){
        if(play(&game) == -1)
            free_board(&game);
    }
    print_board(&game);
    free_board(&game);
    return 0;
}


