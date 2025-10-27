#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

typedef struct s_g{
    int w;
    int h;
    int it;
    char d;
    char a;
    int flag;
    int i;
    int j;
    char **bd;
} t_g;

void free_tab(char **line, int size){
    if(line){
        for(int i = 0; i < size; i++){
            if(line[i])
                free(line[i]);
        }
        free(line);
    } 
}

int init_game(t_g *g, char **av){
    g->w = atoi(av[1]);
    g->h = atoi(av[2]);
    g->it = atoi(av[3]);
    if(g->w <= 0 || g->h <= 0 || g->it < 0)
        return -1;
    g->a = 'O';
    g->d = ' ';
    g->flag = 0;
    g->i = 0;
    g->j = 0;
    g->bd = (char **)malloc(g->h * sizeof(char *));
    if(!g->bd) return -1;
    for(int i = 0; i < g->h; i++){
        g->bd[i] = (char *)malloc(g->w * sizeof(char));
        if(!g->bd[i]) {free_tab(g->bd, i); return -1;}
        for(int j = 0; j < g->w; j++)
            g->bd[i][j] = g->d;
    }
    return 0;
}

void fill_board(t_g *g){
    char bu;
    while(read(STDIN_FILENO, &bu, 1) == 1){
        int flag = 0;
        switch(bu){
            case 'w':if(g->i > 0) g->i--; break;
            case 's':if(g->i < g->h - 1) g->i++; break;
            case 'a':if(g->j > 0) g->j--; break;
            case 'd':if(g->j < g->w - 1) g->j++; break;
            case 'x':g->flag = !g->flag; break;
            default: flag = 1; break;
        }
        if(!flag && g->flag){
            if(g->i >= 0 && g->i < g->h && g->j >= 0 && g->j < g->w)
                g->bd[g->i][g->j] = g->a;
        }
    }
}

int count_ne(t_g *g, int i, int j){
    int count = 0;
    for(int di = -1; di < 2; di++){
        for(int dj = -1; dj < 2; dj++){
           if( di == 0 && dj == 0)
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
    if(!tmp) return -1;
    for(int i = 0; i < g->h; i++){
        tmp[i] = (char *)malloc(g->w * sizeof(char));
        if(!tmp[i]){free_tab(tmp, i); return -1;}
        for(int j = 0; j < g->w; j++)
            tmp[i][j] = g->d;
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
    free_tab(g->bd, g->h);
    g->bd = tmp;
    return 0;
}


int main(int ac, char **av){
    if(ac != 4)
        return 1;
    t_g g;
    if(init_game(&g, av) == -1)
        return 1;
    fill_board(&g);
    for(int i = 0; i < g.it; i++){
        if(play(&g)==-1)
            return 1;
    }
    print_b(&g);
    free_tab(g.bd, g.h);
    return 0;
}
