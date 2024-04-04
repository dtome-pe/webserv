#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include <webserv.hpp>

class Cluster
{	
	private:
		ConfFile					_conf;
		std::vector<class Server>	_servVec;
		std::vector<class Socket>	_sockVec;
		std::vector<pollfd>			_pollVec;

	public:
		Cluster();
		
		void 						parseConfig(char *file);
		void 						setup();
		void 						run();

		int							handleClient(Request &request);
		int							handleRequest(Request &request, Response &response, const Server *serv, const Location *loc);
		void						closeConnection(int i, std::vector<pollfd>&_pollVec,
													std::vector<Socket>&_sockVec, unsigned int *size, int *flag);
		std::vector<Server>			&getServerVector();
		std::vector<Socket>			&getSocketVector();
		std::vector<pollfd>			&getPollVector();

		void						printVectors();
};

#endif
