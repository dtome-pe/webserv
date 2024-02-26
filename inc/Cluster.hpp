#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include <webserv.hpp>

class Cluster
{	
	private:
		ConfFile			_conf;
		std::vector<class Server>	_servVec;
		std::vector<class Socket>	_sockVec;
		std::vector<pollfd>			_pollVec;

	public:
		Cluster();
		
		void parseConfig(char *file);
		void setup();
		void run();

		int	handle_client(int new_socket, const std::vector<Server>&servVec, Socket &listener, std::string &text);

		std::vector<Server>&getServerVector();
		std::vector<Socket>&getSocketVector();
		std::vector<pollfd>&getPollVector();

		void	printVectors();
};

#endif
