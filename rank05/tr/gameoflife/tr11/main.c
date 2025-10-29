#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_g{
    int w;
    int h;
    int it;
    char a;
    char d;
    int flag;
    int i;
    int j;
    char **b;

}t_g;


void free_tab(char **tab, int size){
    if(tab){
        for(int i = 0; i < size; i++){
            if(tab[i])
                free(tab[i]);
        }
        free(tab);
    }
}

int init_game(t_g *g, char **av){
    g->w = atoi(av[1]);
    g->h = atoi(av[2]);
    g->it = atoi(av[3]);
    g->a = 'O';
    g->d = ' ';
    g->flag = 0;
    g->i = 0;
    g->j = 0;
    g->b = (char **)malloc(g->h * sizeof(char *));
    if(!g->b) return -1;
    for(int i = 0; i < g->h; ++i){
        g->b[i] = (char *)malloc(g->w * sizeof(char));
        if(!g->b[i]){ free_tab(g->b, i); return -1;}
        for(int j = 0; j  < g->w; j++)
            g->b[i][j] = g->d;
    }
    return 0;
}

void fill_board(t_g *g){
    char bu;
    while(read(STDIN_FILENO, &bu, 1) == 1){
        int flag = 1;
        switch(bu){
            case 'w': if(g->i > 0) g->i--; break;
            case 's': if(g->i < g->h - 1) g->i++; break;
            case 'a': if(g->j > 0) g->j--; break;
            case 'd': if(g->j < g->w - 1) g->j++; break;
            case 'x': g->flag = !g->flag; break;
            default: flag = 0; break;
        }
        if(flag && g->flag){
            if(g->i >= 0 && g->i < g->h && g->j >= 0 && g->j < g->w)
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
            int ni = i + di;
            int nj = j + dj;
            if(ni >= 0 && ni < g->h && nj >= 0 && nj < g->w){
                if(g->b[ni][nj] == g->a)
                    count++;
            }
        }
    }
    return count;
}

int play(t_g *g){
    char **tmp = (char **)malloc(g->h * sizeof(char *));
    if(!tmp) return -1;
    for(int i = 0; i < g->h; ++i){
        tmp[i] = (char *)malloc(g->w * sizeof(char));
        if(!tmp[i]) {free_tab(g->b, i); return -1;}
        for(int j = 0; j  < g->w; j++)
            tmp[i][j] = g->d;
    }
    for(int i = 0; i < g->h; i++){
        for(int j = 0; j < g->w; j++){
            int ne = count_ne(g, i, j);
            if(g->b[i][j] == g->a){
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
    free_tab(g->b, g->h);
    g->b = tmp;
    return 0;
}

void print_b(t_g *g){
    for(int i = 0; i < g->h; i++){
        for(int j = 0; j < g->w; j++)
            putchar(g->b[i][j]);
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
        if(play(&g) == -1) return 1;
    }
    print_b(&g);
    free_tab(g.b, g.h);
    return 0;
}
