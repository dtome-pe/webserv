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
		std::string server_name;
		std::string error_page;
		std::string methods;
	public:
		int				s_fd;
		struct addrinfo *s_addr;
		Request			*request_ptr;
		Socket();
		~Socket();
		void	start();

		//getters
		std::string	getPort();
		std::string	getIp();
		std::string getServerName();
		std::string getErrorPage();
		std::string getAllowMethods();
		//setters
		void	setPort(std::string port);
		void	setIp(std::string ip);
		void	setServerName(std::string serverName);
		void	setErrorPage(std::string errorPage);
		void	setAllowMethods(std::string allow);
};

#endif
