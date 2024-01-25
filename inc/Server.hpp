#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <netinet/in.h>
# include "Request.hpp"

class Server
{

	public:

		std::string		ip;
		std::string		port;
		int				s_fd;
		struct addrinfo *s_addr;
		Request			*request_ptr;


		Server();

		void	start();
		void	loop();
};

#endif