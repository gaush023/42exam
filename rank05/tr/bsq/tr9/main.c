#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  // ssize_t

typedef struct s_e{
    int l;
    char a;
    char b;
    char c;
} t_e;

typedef struct s_m{
    int w;
    int h;
    char **m;
} t_m;

typedef struct s_s{
    int size;
    int i;
    int j;
} t_s;

int l_e(FILE *file, t_e *e){
    int res = fscanf(file,"%d %c %c %c", &e->l, &e->a, &e->b, &e->c);
    if(res != 4) return -1;
    if(e->l < 1) return -1;
    if(e->a < 32 || e->a > 126) return -1;
    if(e->b < 32 || e->b > 126) return -1;
    if(e->c < 32 || e->c > 126) return -1;
    if(e->a == e->b || e->a == e->c || e->b == e->c) return -1;
    return 0;
}

void double_free(char *line, t_m *m){
    if(line) free(line);
    if(m && m->m){
        for(int i = 0; i < m->h; i++){
            free(m->m[i]);
        }
        free(m->m);
        m->m = NULL;
    }
}

int check_m(const t_m *m, char c1, char c2){
    for(int i = 0; i < m->h; i++){
        for(int j = 0; j < m->w; j++){
            char ch = m->m[i][j];
            if (ch != c1 && ch != c2)
                return -1;
        }
    }
    return 0;
}

char *copyline(const char *line, int res){
    char *tmp = (char *)malloc((res + 1) * sizeof(char));
    if(!tmp) return NULL;
    for(int i = 0; i < res; i++)
        tmp[i] = line[i];
    tmp[res] = '\0';
    return tmp;
}

int l_m(FILE *file, const t_e *e, t_m *m){
    m->h = e->l;
    m->m = (char **)malloc(sizeof(char*) * m->h);
    if(!m->m) return -1;

    // ヘッダ行の残り（終端の改行など）を捨てる
    int ch;
    while((ch = fgetc(file)) != '\n' && ch != EOF) { /* discard */ }

    char *line = NULL;
    size_t len = 0;
    for(int i = 0; i < m->h; i++){
        ssize_t res = getline(&line, &len, file);
        if(res == -1){ double_free(line, m); return -1; }
        if(res > 0 && line[res - 1] == '\n'){
            line[res - 1] = '\0';
            res--;
        }
        m->m[i] = copyline(line, (int)res);
        if(!m->m[i]){ double_free(line, m); return -1; }

        if(i == 0) m->w = (int)res;
        else if(m->w != (int)res){ double_free(line, m); return -1; }
    }
    free(line);

    if(check_m(m, e->a, e->b) == -1){ double_free(NULL, m); return -1; }
    return 0;
}

static inline int find_min(int a, int b, int c){
    int min = a;
    if(b < min) min = b;
    if(c < min) min = c;
    return min;
}

void print_m(const t_e *e, t_m *m, const t_s *s){
    for(int i = s->i; i < s->i + s->size; i++){
        for(int j = s->j; j < s->j + s->size; j++){
            m->m[i][j] = e->c;
        }
    }
    for(int i = 0; i < m->h; i++)
        printf("%s\n", m->m[i]);
}

void find_b(const t_e *e, t_m *m, t_s *s){
    // VLA（C99）。巨大マップなら動的確保を検討。
    int mx[m->h][m->w];
    for(int i = 0; i < m->h; i++)
        for(int j = 0; j < m->w; j++)
            mx[i][j] = 0;

    for(int i = 0; i < m->h; i++){
        for(int j = 0; j < m->w; j++){
            if(m->m[i][j] == e->b){
                mx[i][j] = 0;
            }else{
                if(i == 0 || j == 0)
                    mx[i][j] = 1;
                else
                    mx[i][j] = 1 + find_min(mx[i-1][j], mx[i-1][j-1], mx[i][j-1]);
                if(mx[i][j] > s->size){
                    s->size = mx[i][j];
                    s->i = i - s->size + 1;
                    s->j = j - s->size + 1;
                }
            }
        }
    }
    print_m(e, m, s);
}

int e_b(FILE *file){
    t_e e;
    if(l_e(file, &e) == -1){ fprintf(stderr, "map error\n"); return -1; }
    t_m m;
    if(l_m(file, &e, &m) == -1){ fprintf(stderr, "map error\n"); return -1; }
    t_s s = {0, 0, 0};
    find_b(&e, &m, &s);
    double_free(NULL, &m);
    return 0;
}

int r_f(const char *name){
    FILE *file = fopen(name, "r");
    if(!file){ fprintf(stderr, "map error\n"); return -1; }
    int res = e_b(file);
    fclose(file);
    return res;
}

int main(int ac, char **av){
    if(ac == 1){
        return e_b(stdin) == -1 ? 1 : 0;
    }else{
        for(int i = 1; i < ac; i++){
            if(r_f(av[i]) == -1) return 1;
            if(i < ac - 1) printf("\n");
        }
        return 0;
    }
}
