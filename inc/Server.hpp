#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <netinet/in.h>
# include "Request.hpp"
# include <poll.h>

class Server
{

	public:

		std::string		ip;
		std::string		port;
		struct addrinfo *s_addr;
		Request			*request_ptr;

		struct pollfd	poll;


		Server();

		void	start();
		void	loop();
};

#endif