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
	private:

	public:
		int		active;
		struct socket_list	*sock_list;
		std::vector<class Socket>	sock_vec;
		Server();

		void	start();
};

#endif
