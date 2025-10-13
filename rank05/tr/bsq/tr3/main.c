#include "bsq.h"

int main(int ac, char **av){
  if(ac == 1){
      if(execute_bsq(stdin) == -1)
          fprintf(stderr, "error map\n");
  }else if(ac == 2) {
      if(file_pointer(av[1]) == -1)
          fprintf(stderr, "error map\n");
  }else{
    int i = 1;
    while(i < ac){
      if(file_pointer(av[i]) == -1)
          fprintf(stderr, "error map\n");
      i++;
      if(i < ac -1)
          fprintf(stdout, "\n");
    }
  }
  return 0;
}

    
