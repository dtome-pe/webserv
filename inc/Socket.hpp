#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <iostream>
# include <netinet/in.h>
# include "Request.hpp"
# include <poll.h>

class Socket
{
	private:
		std::string host;
		std::string ip;
		std::string port;
	
	public:

		class Server 			*serv;
		Socket(std::string host_port, Server *s_ptr);

		int				s_fd;
		struct addrinfo *s_addr;
		Request			*request_ptr;
		void	start();

		//getters
		std::string	getPort() const;
		std::string	getIp() const;
		//setters
		void	setPort(std::string port);
		void	setIp(std::string ip);
		void	setHost(std::string host);
};

#endif
