#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <iostream>
# include <netinet/in.h>
# include "Request.hpp"
# include <poll.h>

class Socket
{
	private:
		std::string ip;
		std::string port;
	public:

//		std::string		ip;
//		std::string		port;

		int				s_fd;
		struct addrinfo *s_addr;
		Request			*request_ptr;

		Socket();

		void	start();
		std::string	getPort();
		std::string	getIp();
		void	setPort(std::string port);
		void	setIp(std::string ip);
};

#endif
