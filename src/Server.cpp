#include "../inc/Server.hpp"
#define _XOPEN_SOURCE_EXTENDED 1
#include <sys/socket.h>
#include <netinet/in.h>
#include <bits/stdc++.h>
#include <string>
#include <cstring>

Server::Server()
{
	ip = "127.0.0.1";
	port = 8888;
}

/* in_addr_t Server::ip_to_net_byte_order(const std::string& ip_str) 
{
    // Split the IP address into octets
    std::istringstream iss(ip_str);
    std::string octet_str;
    in_addr_t ip = 0;

    for (int i = 0; i < 4; ++i) {
        std::getline(iss, octet_str, '.');

        int octet_int;
		std::istringstream(octet_str) >> octet_int;
		uint8_t octet = static_cast<uint8_t>(octet_int);
      
	    ip = (ip << 8) | octet;
    }

    return (in_addr_t) htonl(ip); // Convert to network byte order
} */

sockaddr_in *Server::set_sock_addr(sockaddr_in *addr)
{
	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = htonl(INADDR_ANY);
	return (addr);
}

void Server::start()
{
	int server_fd, new_socket;
	struct sockaddr_in addr;
	int	addr_len = sizeof(addr);
	char buff[1024];

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		perror("Socket failed");
        close(server_fd);
        exit(EXIT_FAILURE);
	}
	set_sock_addr(&addr);
	if (bind(server_fd, (const sockaddr *) &addr, addr_len) < 0)
	{
		perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 10) < 0)
	{
		perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
	}
	while (true)
	{	
		new_socket = accept(server_fd, (struct sockaddr *) &addr, (socklen_t *) &addr_len);
		if (new_socket < 0)
		{
            perror("In accept");
            exit(EXIT_FAILURE);
        }
		if (recv(new_socket, buff, 1024, 0) < 0)
			printf("No bytes there to read\n");
		printf("%s\n", buff);
		send(new_socket, buff, 1024, 0);
		close(new_socket);
	}
}