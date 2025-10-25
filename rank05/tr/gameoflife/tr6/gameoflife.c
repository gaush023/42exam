#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


typedef struct s_g{
    int h;
    int w;
    int it;
    char a;
    char d;
    int flag;
    int i;
    int j;
    char **bd;
} t_g;

void print_b(t_g *g){
    for(int i = 0; i < g->h; i++){
        for(int j = 0; j < g->w; j++){
            putchar(g->bd[i][j]);
        }
            putchar('\n');
    }
}

void double_free(char **tmp, int h){
    if(tmp){
        for(int i = 0; i < h; i++)
            free(tmp[i]);
        free(tmp);
    }
}


char *fill_line(int size){
    char *tmp = (char *)malloc((size + 1)* sizeof(char));
    if(!tmp)
        return NULL;
    for(int i = 0; i < size; i++)
        tmp[i] = ' ';
    tmp[size] = '\0';
    return tmp;
}


int init_g(t_g *g, char **av){
    g->w = atoi(av[1]);
    g->h = atoi(av[2]);
    g->it = atoi(av[3]);
    g->a = 'O';
    g->d = ' ';
    g->flag = 0;
    g->i = 0;
    g->j = 0;
    g->bd = (char **)malloc(g->h * sizeof(char *));
    if(!g->bd)
        return -1;
    for(int i = 0; i < g->h; i++){
        g->bd[i] = fill_line(g->w); 
        if(!g->bd[i]){ double_free(g->bd, g->h); return -1;}
    }
    return 0;
}

void fill_board(t_g *g){
    char bu;
    while(read(STDIN_FILENO, &bu, 1) == 1){
        int f = 1;
        switch(bu){
            case 'w': if(g->i > 0) g->i--; break;
            case 's': if(g->i < g->h - 1) g->i++; break;
            case 'a': if(g->j > 0) g->j--; break;
            case 'd': if(g->j < g->w - 1) g->j++; break;
            case 'x': g->flag = !g->flag; break;
            default: f = 0; break;
        }
        if(f && g->flag){
            if(g->i >= 0 && g->i < g->h && g->j >= 0 && g->j < g->w)
                g->bd[g->i][g->j] = g->a;
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
    if(!tmp)
        return -1;
    for(int i = 0; i < g->h; i++){
        tmp[i] = fill_line(g->w);
        if(!tmp[i]){ double_free(tmp, i); return -1;}
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
    double_free(g->bd, g->h);
    g->bd = tmp;
    return 0;
}


int main(int ac, char **av){
    if(ac != 4)
        return 1;
    t_g g;
    if(init_g(&g, av) == -1){ return 1; }
    fill_board(&g);
    for(int i = 0; i < g.it; i++){
        if(play(&g) == -1)
            return 1;
    }
    print_b(&g);
    double_free(g.bd, g.h);
}


