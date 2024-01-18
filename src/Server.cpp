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

in_addr_t Server::ip_to_net_byte_order(const std::string& ip_str) 
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
}

sockaddr_in *Server::set_sock_addr(sockaddr_in *addr)
{
	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = ip_to_net_byte_order(ip);
	return (addr);
}

void Server::start()
{
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1)
	{
		perror("Error binding socket");
        close(sock_fd);
        exit(EXIT_FAILURE);
	}
	sockaddr_in addr;
	set_sock_addr(&addr);
	if (bind(sock_fd, (const sockaddr *) &addr, (socklen_t) sizeof(addr)) == -1)
	{
		perror("Error binding socket");
        close(sock_fd);
        exit(EXIT_FAILURE);
	}
	if (listen(sock_fd, 5) == -1)
	{
		perror("Error listening");
        close(sock_fd);
        exit(EXIT_FAILURE);
	}	
	std::cout << "Listening at " << getsockname(sock_fd, (sockaddr *) &addr, (socklen_t *) sizeof(addr)) << std::endl;
}