#include <sys/socket.h>
#include <netinet/in.h>
#include <bits/stdc++.h>
#include <string>
#include <cstring>

in_addr_t ip_to_net_byte_order(const std::string& ip_str) 
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

sockaddr_in *set_sock_addr(sockaddr_in *addr, unsigned int port, std::string &ip)
{
	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = ip_to_net_byte_order(ip);
	return (addr);
}