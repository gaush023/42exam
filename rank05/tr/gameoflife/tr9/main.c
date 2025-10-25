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
	char **bd;
}t_g;

int init_game(t_g *g, char **av){
	g->w = atoi(av[1]);
 	g->h = atoi(av[2]);
	g->it = atoi(av[3]);
	g->a = 'O';
	g->d = ' ';
	g->i = 0;
	g->j = 0;
	g->flag = 0;
	c

}

int main(int ac, char **av){
	if(ac != 4)
		return 1;
	t_g g;
	if(init_game(&g) == -1)

}





