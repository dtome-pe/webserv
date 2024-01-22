#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <netinet/in.h>

	sockaddr_in *set_sock_addr(sockaddr_in *addr, unsigned int port, std::string &ip);
	in_addr_t ip_to_net_byte_order(const std::string& ip_str);

	void	*handle_client(void *arg);

#endif