#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"
# include <iostream>
# include <netinet/in.h>
# include "Request.hpp"
# include "Socket.hpp"
# include <poll.h>

class Server
{

	public:


		struct socket_list	*sock_list;

		Server();

		void	start();
};

#endif