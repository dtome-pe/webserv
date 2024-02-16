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
		int				s_fd;
		struct addrinfo *s_addr;
		Request			*request_ptr;
		Socket();
		~Socket();
		void	start();

		//getters
		std::string	getPort() const;
		std::string	getIp() const;
		//setters
		void	setPort(std::string port);
		void	setIp(std::string ip);
};

#endif
