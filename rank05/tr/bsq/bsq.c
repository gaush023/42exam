#include "bsq.h"

int load_el(FILE* file, t_el* els)
{
  int ret = fscanf(file, "%d %c %c %c", &(els->lines), &(els->empty), &(els->ob), &(els->full));
  
  if((ret != 4))
    return -1;

  if(els->lines <= 0)
    return -1;
  if(els->empty == els->ob || els->empty == els->full || els->ob == els->full)
    return -1;
  if(els->empty < 32 || els->empty > 126)
    return -1;
  if(els->ob < 32 || els->ob > 126)
    return -1;
  if(els->full < 32 || els->full > 126)
    return -1;

  return 0;
}

int find_min(int n1, int n2, int n3)
{
  int min = n1;

  if (n2 < min)
    min = n2;
  if (n3 < min)
    min = n3;
  return min;
}

char *ft_substr(char *arr, int start, int len)
{
  char *str = (char *)malloc(len + 1);
  if(!str)
    return NULL;
  int i = 0;
  int j = 0;
  while(arr[i])
  {
    if(( i >= start) && (j < len))
    {
      str[j] = arr[i];
      j++;
    }
    i++;
  }
  str[j] = '\0';
  return str;
}

void free_map(char **map)
{
  int i = 0;
  if(map)
  {
    while(map[i] != NULL)
    {
        if(map[i])
          free(map[i]);
        i++;
    }
    free(map);
  }
}

int ele_con(char **map, char c1, char c2)
{
  int i = 0;
  while(map[i])
  {
    int j = 0;
    while(map[i][j] != '\0')
    {
      if((map[i][j] != c1) && (map[i][j] != c2))
        return -1;
      j++;
    }
    i++;
  }
  return (0);
}

int loadMap(FILE* file, t_map *map, t_el* els)
{
  map->height = els->lines;
  map->grid = (char **)malloc((map->height + 1) * (sizeof(char *)));
  map->grid[map->height] = NULL;

  char *line = NULL;
  size_t len = 0;

  if(getline(&line, &len, file) == -1)
  {
    free_map(map->grid);
    return(-1);
  }
  
  for(int i = 0; i < map->height; i++)
  {
    int read = getline(&line, &len, file);
    if (read == -1)
    {
      free(line);
      free_map(map->grid);
      return -1;
    }
    if(line[read - 1] == '\n')
      read--;
    else {
      free(line);
      free_map(map->grid);
      return(-1);
    }
    map->grid[i] = ft_substr(line, 0, read);
    if(!(map->grid[i]))
    {
      free(line);
      free_map(map->grid);
      return(-1);
    }

    if(i == 0)
      map->width = read;
    else {
      if(map->width != read)
      {
        free(line);
        free_map(map->grid);
        return -1;
      }
    }
  }

  if(ele_con(map->grid, els->empty, els->ob) == -1)
  {
    free(line);
    free_map(map->grid);
    return -1;
  }
  free(line);
  return 0;
}

void find_biq_sq(t_map *map, t_sq *sq, t_el *els)
{
  int matrix[map->height][map->width];
  for(int i = 0; i < map->height; i++)
  {
    for(int j = 0; j < map->width; j++)
      matrix[i][j] = 0;
  }

  for(int i = 0; i < map->height; i++)
  {
    for(int j = 0; j < map->width; j++)
    {
      if(map->grid[i][j] == els->ob)
        matrix[i][j] = 0;
      else if(i == 0 || j == 0)
        matrix[i][j] = 1;
      else {
        int min = find_min(matrix[i -1][j], matrix[i - 1][j - 1], matrix[i][j - 1]);
        matrix[i][j] = min + 1;
      }

      if(matrix[i][j] > sq->size)
      {
        sq->size = matrix[i][j];
        sq->i = i - matrix[i][j] + 1;
        sq->j = j - matrix[i][j] + 1;
      }
    }
  }

	for(int i = 0; i < map->height; i++)
	{
		for(int j = 0; j < map->width; j++)
		{
			printf("%d", matrix[i][j]);
		}
		printf("\n");
	}
} 

void print_filled_square(t_map* map, t_sq* square, t_el* elements)
{

	for(int i = square->i; i < square->i + square->size; i++)
	{
		for(int j = square->j; j < square->j + square->size; j++)
		{
			if((i < map->height) && (j < map->width))
				map->grid[i][j] = elements->full;
		}
	}

	for(int i = 0; i < map->height; i++)
	{
		fprintf(stdout, "%s\n", map->grid[i]);
	}
}

int execute_bsq(FILE* file)
{
  t_el els;
  if(load_el(file, &els) == -1)
    return -1;
  
  t_map map;
  if(loadMap(file, &map, &els)== -1)
    return -1;

  t_sq sq;
  sq.size = 0;
  sq.i = 0;
  sq.j = 0;

  find_biq_sq(&map, &sq, &els);

	print_filled_square(&map, &sq, &els);
	free_map(map.grid);
	return(0);
}

int convert_file_pointer(char* name)
{
	FILE* file = fopen(name, "r");
	if(!file)
		return(-1);
	int ret = 0;
	ret = execute_bsq(file);
	fclose(file);
	return(ret);
}
