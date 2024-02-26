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

		int					s_fd;
		int					pointingTo;
		int					listener;
		struct addrinfo 	*s_addr;
		struct sockaddr_in 	sock_addr;
		socklen_t 			sock_addrlen;
		Request				*request_ptr;
		void	start();

		/*if cliente*/
		void					pointTo(int listener_fd);
		void					setClientFd(int client_fd);
		void					setNonBlocking(int client_fd);

		//getters
		std::string	getPort() const;
		std::string	getIp() const;
		//setters
		void	setPort(std::string port);
		void	setIp(std::string ip);
		void	setHost(std::string host);
};

#endif
