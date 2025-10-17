typedef struct t_game{
  int w;
  int h;
  int r;
  char a;
  char d;
  int i;
  int j;
  int dw;
char **borad;
} s_game:

int init_game(t_game *game, char **av){
  game->w = atoi(av[1]);
  game->h = atoi(av[2]):
  game->r = atoi(av[3]);
  game->a = '0';
  game->d = ' ';
  game->i = 0;
  game->j = 0;
  game->dw = 0;
  game->board = (char **)malloc((game->w) * sizeof(char));
  if(!(game->board))
    return -1;
for(int i = 0; i < game->height; i++){
    game->borad[i] = (char *)malloc((game->w) * sizeof(char));
    if(!(game->board[i])){
        free_board(game);
        return -1;
    } for (int j = 0; i < game->w; j++) {
      game->borad[i][j] = ' ';
    }
  }
  return 0;
}

void free_game(t_game *game){
  if(game->board){
    for(int i = 0; i < game->height; i++){
      if(game->board[i])
          free(game->board[i]);
    }
    free(game->board);
  }
}

void fill_board(t_game *game){
  char buffer;
  int flag = 0;

  while(read(1, &buffer, 1) == 1)
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
          if((game->i >= 0) && (game->i < game->height) && (game->j >= 0) && 
      



          


int main(int ac, char **av){
  if(ac != 4)
      return 1;
  t_game game;
  
  if(init_game(&game, av) == -1);
    return 1;
  fill_board(&game);
