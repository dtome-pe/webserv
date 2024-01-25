#include "../inc/Server.hpp"
#include "../inc/webserv.hpp"
#define _XOPEN_SOURCE_EXTENDED 1
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <cstdio>

Server::Server()
{
	ip = "127.0.0.1";
	port = "8888";
}

void	Server::parse_config()
{

}

void	Server::start()
{
	get_addr_info(&s_addr, port.c_str());
	s_fd = create_s(s_fd, s_addr);
	bind_s(s_fd, s_addr);
	listen_s(s_fd);
	freeaddrinfo(s_addr);
}

void Server::loop()
{
	int	new_socket;
	struct sockaddr_in c_addr;
    socklen_t c_addr_size = sizeof (c_addr);

	while (true)
	{	
		new_socket = accept(s_fd, (struct sockaddr *) &c_addr, &c_addr_size);
		if (new_socket < 0)
		{
            print_error(strerror(errno));
            exit(EXIT_FAILURE);
        }
		if (!fork())
		{	
			close(s_fd);
			handle_client(new_socket);
			close(new_socket);
			exit(0);
		}
		close(new_socket);
	}	
}