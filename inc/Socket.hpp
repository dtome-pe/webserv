#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <iostream>
# include <netinet/in.h>
# include "Request.hpp"
# include <poll.h>

class Socket
{
	public:

		std::string		ip;
		std::string		port;
		int				s_fd;
		struct addrinfo *s_addr;
		Request			*request_ptr;

		Socket();

		void	start();
		void	setPort(std::string port);
};

#endif
