#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/select.h>

int count = 0, max_fd = 0, sockfd = 0;
int ids[60000];
char *msgs[60000];
fd_set active_fds, read_fds, write_fds;
char buf_read[4096], buf_write[4096];

void fatal_error()
{
	write(2, "Fatal error\n", 12);
	exit(1);
}

int extract_message(char **buf, char **msg)
{
	char *newbuf;
	int i = 0;

	*msg = NULL;
	if (*buf == NULL)
		return (0);
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, strlen(*buf + i + 1) + 1);
			if (!newbuf)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			(*buf)[i + 1] = '\0';
			*msg = *buf;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char *newbuf;
	int len = 0;

	if (buf)
		len = strlen(buf);
	newbuf = malloc(len + strlen(add) + 1);
	if (!newbuf)
		return (NULL);
	newbuf[0] = '\0';
	if (buf)
		strcat(newbuf, buf);
	strcat(newbuf, add);
	free(buf);
	return (newbuf);
}

void notify_all(int from, const char *str)
{
	for (int fd = 0; fd <= max_fd; fd++)
	{
		if (FD_ISSET(fd, &active_fds) && fd != sockfd && fd != from)
			send(fd, str, strlen(str), 0);
	}
}

void add_client(int fd)
{
	max_fd = (fd > max_fd ? fd : max_fd);
	ids[fd] = count++;
	msgs[fd] = NULL;
	FD_SET(fd, &active_fds);

	sprintf(buf_write, "server: client %d just arrived\n", ids[fd]);
	notify_all(fd, buf_write);
}

void remove_client(int fd)
{
	sprintf(buf_write, "server: client %d just left\n", ids[fd]);
	notify_all(fd, buf_write);

	if (msgs[fd])
		free(msgs[fd]);
	msgs[fd] = NULL;

	FD_CLR(fd, &active_fds);
	close(fd);
}

void send_pending_messages(int fd)
{
	char *msg = NULL;

	while (extract_message(&msgs[fd], &msg) == 1)
	{
		sprintf(buf_write, "client %d: %s", ids[fd], msg);
		notify_all(fd, buf_write);
		free(msg);
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}

	FD_ZERO(&active_fds);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		fatal_error();

	max_fd = sockfd;
	FD_SET(sockfd, &active_fds);

	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		fatal_error();

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(2130706433);
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
		fatal_error();
	if (listen(sockfd, SOMAXCONN) != 0)
		fatal_error();

	while (1)
	{
		read_fds = active_fds;
		write_fds = active_fds;

		if (select(max_fd + 1, &read_fds, &write_fds, NULL, NULL) < 0)
			fatal_error();

		for (int fd = 0; fd <= max_fd; fd++)
		{
			if (FD_ISSET(fd, &read_fds) && fd == sockfd)
			{
				socklen_t addr_len = sizeof(server_addr);
				int client_fd = accept(sockfd, (struct sockaddr *)&server_addr, &addr_len);
				if (client_fd >= 0)
					add_client(client_fd);
				continue;
			}

			if (FD_ISSET(fd, &read_fds) && fd != sockfd)
			{
				int bytes = recv(fd, buf_read, sizeof(buf_read) - 1, 0);
				if (bytes <= 0)
				{
					remove_client(fd);
					continue;
				}
				buf_read[bytes] = '\0';

				char *tmp = str_join(msgs[fd], buf_read);
				if (!tmp)
					fatal_error();
				msgs[fd] = tmp;

				send_pending_messages(fd);
			}
		}
	}

	return (0);
}
