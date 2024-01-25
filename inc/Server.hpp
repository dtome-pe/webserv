#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <netinet/in.h>

class Server
{

	public:

		std::string		ip;
		unsigned int	port;
		int				s_fd;
		struct addrinfo *s_addr;

		Server();

		void	start();
		void	loop();
};

#endif