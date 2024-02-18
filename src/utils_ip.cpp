#include "../inc/webserv.hpp"

std::string ip_to_str(sockaddr_in *addr)
{
	std::ostringstream ipStream;
	uint32_t ipAddress = ntohl(addr->sin_addr.s_addr);

	ipStream << ((ipAddress >> 24) & 0xFF) << "."
             << ((ipAddress >> 16) & 0xFF) << "."
             << ((ipAddress >> 8) & 0xFF) << "."
             << (ipAddress & 0xFF);
	return (ipStream.str());	// volcamos ip de network byte order a string.
}