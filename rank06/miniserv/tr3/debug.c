#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

int sockfd = 0, max_fd = 0, count = 0;
int ids[6000];
char *msgs[6000];
fd_set active_fds, read_fds, write_fds;
char buf_read[4242], buf_write[4242];

void fatal_error(){
  write(2, "Fatal error\n", 12);
  exit(1);
}

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

void notify_all(int from, const char *str){
  fprintf(stderr, "DEBUG: notify_all from fd=%d, msg=%s", from, str);
  for(int fd = 0; fd <= max_fd; fd++){
    if(FD_ISSET(fd, &active_fds) && fd != sockfd && fd != from) {
      fprintf(stderr, "DEBUG: sending to fd=%d\n", fd);
      int ret = send(fd, str, strlen(str), 0);
      fprintf(stderr, "DEBUG: send returned %d\n", ret);
    }
  }
}

void add_client(int fd){
  fprintf(stderr, "DEBUG: add_client fd=%d\n", fd);
  max_fd = (fd > max_fd ? fd : max_fd);
  ids[fd] = count++;
  msgs[fd] = NULL;
  FD_SET(fd, &active_fds);
  sprintf(buf_write, "server: client %d just arrived\n", ids[fd]);
  notify_all(fd, buf_write);
}

void remove_client(int fd){
  sprintf(buf_write, "server: client %d just left\n", ids[fd]);
  notify_all(fd, buf_write);
  if(msgs[fd])
    free(msgs[fd]);
  msgs[fd] = NULL;
  FD_CLR(fd, &active_fds);
  close(fd);
}

void send_pending_message(int fd){
  char *msg = NULL;
  while(extract_message(&msgs[fd], &msg) == 1){
    sprintf(buf_write, "client %d: %s", ids[fd], msg);
    notify_all(fd, buf_write);
    free(msg);
  }
}

int main(int ac, char **av) {

  if(ac != 2){
    write(2, "Wrong number of arguments\n", 27);
    exit(1);
  }

  FD_ZERO(&active_fds);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
      fatal_error();

  fprintf(stderr, "DEBUG: sockfd=%d\n", sockfd);

  int opt = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
      fatal_error();
  FD_SET(sockfd, &active_fds);
  max_fd = sockfd;

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(av[1]));

	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    fatal_error();
  if(listen(sockfd, SOMAXCONN) != 0)
    fatal_error();

  fprintf(stderr, "DEBUG: Server listening on port %s\n", av[1]);

  while(1){
    read_fds = active_fds;
    write_fds = active_fds;

    if(select(max_fd + 1, &read_fds, &write_fds, NULL, NULL) < 0)
      fatal_error();
    for(int fd = 0; fd <= max_fd; fd++){
      if(FD_ISSET(fd, &read_fds) && fd == sockfd){
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
        fprintf(stderr, "DEBUG: accept returned fd=%d\n", client_fd);
        if(client_fd >= 0)
          add_client(client_fd);
        continue;
      }
      if(FD_ISSET(fd, &read_fds) && fd != sockfd){
        int bytes = recv(fd, buf_read, sizeof(buf_read) -1, 0);
        fprintf(stderr, "DEBUG: recv from fd=%d, bytes=%d\n", fd, bytes);
        if(bytes <= 0){
          remove_client(fd);
          continue;
        }
        buf_read[bytes] = '\0';
        char *tmp = str_join(msgs[fd], buf_read);
        if(!tmp)
          fatal_error();
        msgs[fd] = tmp;
        send_pending_message(fd);
      }
    }
  }
  return 0;
}
