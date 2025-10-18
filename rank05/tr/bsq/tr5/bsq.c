#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_e{
    int h;
    char e;
    char o;
    char m;
} t_e;

typedef struct s_m{
    char **m;
    int w;
    int h;
} t_m;

typedef struct s_s{
    int size;
    int i;
    int j;
} t_s;


int load_e(FILE *file, t_e *e){
    int res = fscanf(file, "%d %c %c %c", &(e->h), &(e->e), &(e->o), &(e->m));
    if (res != 4)
        return -1;
    if (e->e == e->o || e->e == e->m || e->e == e->m)
        return -1;
    if (e->e < 32 || 126 < e->e)
        return -1;
    if (e->o < 32 || 126 < e->o)
        return -1;
    if (e->m < 32 || 126 < e->m)
        return -1;

}

void free_map(char **m){
    if(m){
        for(int i = 0; m[i]; i++){
            free(m[i]);
        }
        free(m);
    }
}

char *ft_strdup(char *line, int n){
    char *tmp = (char *)malloc((n + 1) * (sizeof(char)));
    if (!tmp)
        return NULL;
    for(int i = 0; i < n; i++)
        tmp[i] = line[i];
    tmp[n] = '\0';
    return tmp;
}


int check_el(char **m, char c1, char c2){
    for(int i = 0; m[i]; i++){
        for(int j = 0; m[i][j]; j++){
            if(m[i][j] != c1 && m[i][j] != c2)
                return -1;
        }   
    }
    return 0;
}


int load_m(FILE *file, t_e *e, t_m *m){
    m->m = (char **)malloc((e->h + 1) * sizeof(char *));
    m->h = e->h;
    m->m[e->h] = NULL;
    
    char *line;
    size_t n = 0;
    if(getline(&line, &n, file) == -1) {free(line); free_map(m->m); return -1;}
    for(int i = 0; i < m->h; i++){
        int res = getline(&line, &n, file);
        if(res == -1) {free(line); free_map(m->m); return -1;}
        if(line[res - 1] == '\n')
            res--;
        else{free(line); free_map(m->m); return -1;}
        m->m[i] = ft_strdup(line, res);
        if(!m->m[i]) {free(line); free_map(m->m); return -1;}    
        if( i == 0)
            m->w = res;
        else {
            if(res != m->w) {free(line); free_map(m->m); return -1;}
        }
    }
    if(check_el(m->m, e->e, e->o) == -1){free(line); free_map(m->m); return -1;}
    free(line);
    return 0;
}

int find_min(int n1, int n2, int n3){
    int min = n1;
    if(min > n2)
        min = n2;
    if(min > n3)
        min = n3;
    return min;
}

void find_big_sq(t_e *e, t_m *m, t_s *s){
    int mx[m->h][m->w];
    for(int i = 0; i < m->h; i++){
        for(int j = 0; j < m->w; j++)
            mx[i][j] = 0;
    }

    for(int i = 0; i < m->h; i++){
        for(int j = 0; j < m->w; j++){
            if(m->m[i][j] == e->o)
                mx[i][j] = 0;
            else if(i == 0 || j == 0)
                mx[i][j] = 1;
            else{
                int min = find_min(mx[i - 1][j], mx[i -1][j -1], mx[i][j-1]);
                mx[i][j] = min + 1;
            }
            if(mx[i][j] > s->size){
                s->size = mx[i][j];
                s->i = i - s->size + 1;
                s->j = j - s->size + 1;
            }
        }
    }
}

void print_fmap(t_e *e, t_m *m, t_s *s){
    for(int i = s->i; i < s->i + s->size; i++){
        for(int j = s->j; j < s->j + s->size; j++){
            m->m[i][j] = e->m;
        }
    }
    for(int i = 0; i < m->h; i++)
        fprintf(stdout, "%s\n", m->m[i]);
}



int execute_bsq(FILE *file){
    t_e e;
    if (load_e(file, &e)== -1)
        return -1;
    t_m m;
    if (load_m(file, &e, &m) == -1)
        return -1;
    t_s s;
    s.size = 0;
    s.i = 0;
    s.j = 0;
    find_big_sq(&e, &m, &s);
    print_fmap(&e, &m, &s);
    free_map(m.m);
    return 0;
}

int file_pointer(char *n){
    FILE *file = fopen(n, "r");
    if(!file)
        return -1;
    int res = execute_bsq(file);
    fclose(file);
    return res;
}

int main(int ac, char **av){
    if(ac == 1){
        if(execute_bsq(stdin) == -1){
            fprintf(stderr, "error\n");
            return -1;
        }
    } else if (ac == 2){
        if(file_pointer(av[1]) == -1){
            fprintf(stderr, "error\n");
            return -1;
        }
    }else{
        for(int i = 1; i < ac; i++){
            if(file_pointer(av[i]) == -1){
                fprintf(stderr, "error\n");
                return -1;
            }
            if(i < ac -1)
                fprintf(stdout, "\n");
        }
    }
    return 0;
}
