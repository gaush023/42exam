#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


typedef struct s_e{
    int ml;
    char e;
    char o;
    char f;
} t_e;

typedef struct s_m{
    char **cm;
    int h;
    int w;
} t_m;

typedef struct s_s{
    int size;
    int i;
    int j;
} t_s;

int load_e(FILE *file, t_e *e){
    int ret = fscanf(file, "%d %c %c %c", &(e->ml), &(e->e), &(e->o), &(e->f));

    if(ret != 4)
        return -1;
    if(e->ml <= 0)
        return -1;
    if(e->ml < 32 || e->ml > 126)
        return -1;
    if(e->e < 32 || e->e > 126)
        return -1;
    if(e->o < 32 || e->o > 126)
        return -1;
    if(e->f < 32 || e->f >126)
        return -1;
    if(e->e == e->o || e->e == e->f || e->e == e->f)
        return -1;
    return 0;
}


void free_map(char **m){
    if(m){
        for(int i = 0;m[i] != NULL;i++)
            free(m[i]);
        free(m);
    }
}

size_t strlen(char *str){
    size_t len = 0;
    while (str[len] != '\0')
        len++;
    return len;
}

char *ft_substr(char *s, int start, int len){

    char *dst;
    if(!s)
        return NULL;
    if (start + len > strlen(s))
        len = strlen(s) - start;
    if ((dst = malloc(sizeof(char) * (len + 1))) == NULL)
        return (NULL);
    int i = 0;
    while (++i - 1 < len)
        *(dst + i - 1) = *(s + start + i - 1);
    *(dst + i - 1) = '\0';
    return (dst);
}

int check_el(char **tmp, char c1, char c2){
    for(int i = 0; tmp[i]; i++){
        for(int j = 0; tmp[i][j]; j++){
            if(tmp[i][j] != c1 && tmp[i][j] != c2)
                return -1;
        }
    }
    return 0;
}

int load_m(FILE *file, t_m *m, t_e *e){
    m->h = e->ml;
    m->cm = (char **)malloc((m->h +1 )* sizeof(char*));
    m->cm[m->h] = NULL;
    
    char *line = NULL;
    size_t n = 0;

    if(getline(&line, &n, file) == -1){
        free_map(m->cm);
        return -1;
    }
    for(int i = 0; i < m->h; i++){
        int res = getline(&line, &n, file);
        if(res == -1) { free(line); free_map(m->cm); return -1;}
        if(line[res-1] == '\n')
            res--;
        else { free(line); free_map(m->cm); return -1;}
        m->cm[i] = ft_substr(line, 0, res);
        if(m->cm[i] == NULL){ free(line); free_map(m->cm);return -1;}
        if( i == 0){ m->w = res; }
        else {
            if ( m->w != res){ free(line); free_map(m->cm);return -1;}
        }
    }
    if(check_el(m->cm, e->e, e->o) == -1) { free(line); free_map(m->cm);return -1;}
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
    int res[m->h][m->w];
    for(int i = 0; i < m->h; i++){
        for(int j = 0; j < m->w; j++)
            res[i][j] = 0;
    }

    for(int i = 0; i < m->h; i++){
        for(int j = 0; j < m->w; j++){
            if(m->cm[i][j] == e->o)
                res[i][j] = 0;
            else if( i == 0 || j == 0)
                res[i][j] = 1;
            else{
                int min = find_min(res[i - 1][j], res[i - 1][j-1], res[i][j-1]);
                res[i][j] = min + 1;
            }
            if(res[i][j] > s->size){
                s->size = res[i][j];
                s->i = i - s->size + 1;
                s->j = j - s->size + 1;
            }
        }
    }
}

void print_fmap(t_m *m, t_e *e, t_s *s){
    for(int i = s->i; i < s->i + s->size; i++){
        for(int j = s->j; j < s->j + s->size; j++){
            if(i < m->h && j < m->w)
                m->cm[i][j] = e->f;
        }
    }
    for(int i = 0; i < m->h; i++)
        fprintf(stdout, "%s\n", m->cm[i]);
}

int execute_bsq(FILE *file){
    t_e e;
    load_e(file, &e);
    t_m m;
    load_m(file, &m, &e); 
    t_s s;
    s.size = 0;
    s.i = 0;
    s.j = 0;
    find_big_sq(&e, &m, &s);
    print_fmap(&m, &e, &s);
    free_map(m.cm);
    return 0;
}

int file_pointer(char *name){
    FILE *file = fopen(name, "r");
    if (!file)
        return -1;
    int res = execute_bsq(file);
    fclose(file);
    return res;
}

int main(int ac, char **av){
    if(ac == 1){
        if(execute_bsq(stdin) == -1)
            fprintf(stderr, "Error\n");
    }
    else if(ac == 2){
        if(file_pointer(av[1]) == -1)
            fprintf(stderr, "Error\n");
    } else {
        int i = 1;
        while( i < ac){
            if(file_pointer(av[i]) == -1)
                fprintf(stderr, "Error\n");
            i++;
         if(i < ac - 1)
            fprintf(stdout, "\n");            
        }
    }
    return 0;
}



