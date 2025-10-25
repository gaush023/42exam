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
  int status;
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
    int flag = 0;
        switch(bu){
            case 'w': if( g->i > 0) g->i--; break;
            case 's': if( g->i < g->h -1) g->i++; break;
            case 'a': if( g->j > 0) g->j--; break;
            case 'd': if( g->j < g->w -1) g->j++; break;
            case 'x': g->status = !(g->status); break;
            default: flag = 1; break;

        }
        if(flag == 0 && g->status){
            if(g->i >= 0 && g->i < g->h && g->j >= 0 && g->j < g->w)
                g->bd[g->i][g->j] = g->a;
        }
    }
}

int init_game(t_g *g, char **av){
    g->w = atoi(av[1]);
    g->h = atoi(av[2]);
    g->it = atoi(av[3]);
    if(g->w < 0 || g->h < 0 || g->it < 0)
    return -1;
    g->a = '0';
    g->d = ' ';
    g->i = 0;
    g->j = 0;
    g->bd = (char **)malloc(g->h * sizeof(char *));
    if(!g->bd)
    return -1;
    for(int j = 0; j < g->h; j++){
        g->bd[j] =  (char *)malloc(g->w * sizeof(char));
        if(!g->bd[j]){double_free(g->bd, g->h); return -1;}
        for(int i = 0; i < g->w; i++){
            g->bd[j][i] = g->d;
        }
    }
    g->status = 0;
    return 0;
}

int count_ne(t_g *g, int i, int j){ 
    int count = 0;
    for(int di = -1; di <= 1; di++){
        for(int dj = -1; dj <= 1; dj++){
            if(di == 0 && dj == 0)
                continue;
            int ni = i + di;
            int nj = j + dj;
            if(ni >= 0 && ni < g->h && nj >= 0 && nj < g->w)
                if(g->bd[ni][nj] == g->a)
                    count++;
        }
    }
    return count;
}

int play(t_g *g){
    char **tmp = (char **)malloc(g->h * sizeof(char *));
    if(!tmp)
        return -1;
    for(int i = 0; i < g->h; i++){
        tmp[i] = (char *)malloc(g->w * sizeof(char));
        if(!tmp[i]){ double_free(g->bd, g->h); return -1; }
    }

    for(int i = 0; i < g->h; i++){
        for(int j = 0; j < g->w; j++){
            int ne = count_ne(g, i, j);
            if(g->bd[i][j] == g->a){
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
    double_free(g->bd, g->h);
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
    if(ac != 4)
        return 1;
    t_g g;
    if(init_game(&g, av) == -1)
        return 1;
    fill_board(&g);
    for(int i = 0; i < g.it; i++){
        if(play(&g) == -1)
            return 1;
    }
    print_b(&g);
    double_free(g.bd, g.h);
    return 0;
}


