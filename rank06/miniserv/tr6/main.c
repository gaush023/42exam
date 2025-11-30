#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

int count = 0, max_fd = 0, sockfd = 0;
int ids[6000];
char *msgs[6000];
fd_set afds, rfds, wfds;
char bufr[4242], bufw[4242]; 

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

void fatal_error(){
  write(2, "Fatal error\n", strlen("Fatal error\n"));
  exit(1);
}

void notify_all(int from, const char *str){
  for(int fd = 0; fd <= max_fd; fd++){
    if(FD_ISSET(fd, &afds) && fd != sockfd && fd != from){
      send(fd, str, strlen(str), 0);
    }
  }
}

void client_add(int fd){
  max_fd = (fd > max_fd ? fd : max_fd);
  ids[fd] = count++;
  msgs[fd] = NULL;
  FD_SET(fd, &afds);
  sprintf(bufw, "server: client %d just arrived\n", ids[fd]);
  notify_all(fd, bufw);
}

void remove_client(int fd){
  sprintf(bufw, "server: client %d just left\n", ids[fd]);
  notify_all(fd, bufw);
  FD_CLR(fd, &afds);
  if(msgs[fd])
    free(msgs[fd]);
  close(fd);
}

void send_pending_msg(int fd){
  char *msg = NULL; 
  while(extract_message(&msgs[fd], &msg)){
    sprintf(bufw, "client %d: %s", ids[fd], msg);
    notify_all(fd, bufw);
    free(msg);
  }
} 

int main(int ac, char **av) {

  if(ac != 2){
    write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments"));
    exit(1);
  }
  
  FD_ZERO(&afds);
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd < 0)
    fatal_error();
  FD_SET(sockfd, &afds);
  int opt = 1;
  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    fatal_error();
  max_fd = sockfd;
	struct sockaddr_in servaddr; 
	memset(&servaddr, 0, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1j
	servaddr.sin_port = htons(atoi(av[1])); 
  
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
    fatal_error();
	if (listen(sockfd, SOMAXCONN) != 0)
    fatal_error();
  while(1){
    rfds = afds;
    wfds = afds;
    if(select(max_fd + 1, &rfds, &wfds, NULL, NULL) < 0)
      fatal_error();
    for(int fd = 0; fd <= max_fd; fd++){
      if(FD_ISSET(fd, &rfds) && fd == sockfd){
        struct sockaddr_in clientAddr;
        socklen_t addrlen = sizeof(clientAddr);
        int clientfd = accept(sockfd, (struct sockaddr *)&clientAddr, &addrlen);
        if(clientfd >= 0)
          client_add(clientfd);
        continue;
      }
      if(FD_ISSET(fd, &rfds) && fd != sockfd){
        int bytes = recv(fd, bufr, sizeof(bufr) -1, 0);
        if(bytes <= 0){
          remove_client(fd);
          continue;
        }
        bufr[bytes] = '\0';
        char *tmp = str_join(msgs[fd], bufr);
        if(!tmp)
          fatal_error();
        msgs[fd] = tmp;
        send_pending_msg(fd);
      }
    }
  }
  return 0;
}
