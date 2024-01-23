#include "../inc/Server.hpp"
#include "../inc/webserv.hpp"
#define _XOPEN_SOURCE_EXTENDED 1
#include <sys/socket.h>
#include <netinet/in.h>
//#include <bits/stdc++.h>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>

Server::Server()
{
	ip = "127.0.0.1";
	port = 8888;
}

static int create_socket(int server_fd)
{	
	int	flags;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
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

static int bind_socket(int server_fd, sockaddr_in addr, int addr_len)
{
	if (bind(server_fd, (const sockaddr *) &addr, addr_len) < 0)
	{
		perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
	}
	return (server_fd);
}

static int listen_socket(int server_fd)
{
	if (listen(server_fd, 10) < 0)
	{
		perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
	}
	return (server_fd);
}

static void server_loop(int server_fd, sockaddr_in *addr, int *addr_len)
{
	int	new_socket;

	while (true)
	{	
		new_socket = accept(server_fd, (struct sockaddr *) addr, (socklen_t *) addr_len);
		if (new_socket < 0)
		{
            perror("Accept Failed");
            exit(EXIT_FAILURE);
        }
		handle_client(new_socket);
	}	
}

void Server::start()
{
	int server_fd = 0;
	struct sockaddr_in addr;
	int	addr_len = sizeof(addr);

	server_fd = create_socket(server_fd);
	set_sock_addr(&addr, port, ip);
	bind_socket(server_fd, addr, addr_len);
	listen_socket(server_fd);
	server_loop(server_fd, &addr, &addr_len);
}