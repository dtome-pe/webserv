#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <netinet/in.h>

class Server
{

	public:

		std::string		ip;
		unsigned int	port;

		Server();

		void	start();
};

#endif