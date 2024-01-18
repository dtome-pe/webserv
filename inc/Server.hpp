#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <netinet/in.h>

class Server
{

	private:
		sockaddr_in *set_sock_addr(sockaddr_in *addr);
		in_addr_t ip_to_net_byte_order(const std::string& ip_str);

	public:

		std::string		ip;
		unsigned int	port;

		Server();

		void	start();
};

#endif