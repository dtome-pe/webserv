#include "../inc/Server.hpp"
#include "../inc/webserv.hpp"
#define _XOPEN_SOURCE_EXTENDED 1
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <bits/stdc++.h>
#include <string>
#include <cstring>
#include <fcntl.h>

Server::Server()
{
	ip = "127.0.0.1";
	port = 8888;
}

static void	get_addr_info(struct addrinfo **s_addr)
{	
	int				status;
	struct addrinfo hints;

	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	status = getaddrinfo(NULL, "8888", &hints, s_addr);
	if (status != 0)
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    	exit(1);
	}
}

static int create_s(int server_fd, struct addrinfo *s_addr)
{	
	int flags;
	
	server_fd = socket(s_addr->ai_family, s_addr->ai_socktype, s_addr->ai_protocol);
	if (server_fd < 0)
	{
		perror("Socket failed");
        close(server_fd);
        exit(EXIT_FAILURE);
	}
	flags = fcntl(server_fd, F_GETFD, 0);
	if (flags == -1)
	{
		perror("Error getting socket flags");
        close(server_fd);
        exit(EXIT_FAILURE);
	}
	if (fcntl(server_fd, F_SETFD, flags | O_NONBLOCK) == -1)
	{
		perror("Error setting socket to non blocking");
        close(server_fd);
        exit(EXIT_FAILURE);
	}
	return (server_fd);
}

static int bind_s(int server_fd, struct addrinfo *s_addr)
{	
	int	yes = 1;

	if (bind(server_fd, s_addr->ai_addr, s_addr->ai_addrlen) < 0)
	{
		perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
	}
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes)) == -1) 
	{
    	perror("setsockopt");
   		exit(1);
	} 
	return (server_fd);
}

static int listen_s(int server_fd)
{
	if (listen(server_fd, 10) < 0)
	{
		perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
	}
	return (server_fd);
}

static void server_loop(int server_fd)
{
	int	new_socket;
	struct sockaddr_in c_addr;
    socklen_t c_addr_size = sizeof (c_addr);

	while (true)
	{	
		new_socket = accept(server_fd, (struct sockaddr *) &c_addr, &c_addr_size);
		if (new_socket < 0)
		{
            perror("Accept Failed");
            exit(EXIT_FAILURE);
        }
		if (!fork())
		{	
			close(server_fd);
			handle_client(new_socket);
			close(new_socket);
			exit(0);
		}
		close(new_socket);
	}	
}

void Server::start()
{
	struct addrinfo *s_addr;
	int s_fd = 0;

	get_addr_info(&s_addr);
	s_fd = create_s(s_fd, s_addr);
	bind_s(s_fd, s_addr);
	listen_s(s_fd);
	server_loop(s_fd);

	freeaddrinfo(s_addr);
}