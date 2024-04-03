#ifndef CONFFILE_HPP
#define CONFFILE_HPP

#include "Colors.hpp"
#include <cctype>
#include <webserv.hpp>

class Cluster;

class ConfFile
{
	private:
		std::string file;
		std::vector<class Server>	serv_vec;
		std::vector<class Socket>	sock_vec;
		std::vector<pollfd>			pollVec;

	public:
		ConfFile();
		ConfFile(std::string _file);
		~ConfFile();
		void	trimSpaces(std::string& str);
		void	parse_config(Cluster &cluster, char *file); 
		void	parse_location(std::string line, Location &loc);
		int		parse_element(std::string& content, int i);
		int		countServers(std::string content);
		std::string findInfo(std::string line, std::string tofind);
		void	findIp(Server& Serv, std::string newserv);
		int		check_info();
		void	copyInfo(Cluster &cluster);
		void	print_servers();
		void	print_sockets();
		void	create_sockets();
		void	start_sockets();
		void	init_poll();
		void	poll_loop();
		std::vector<std::string>	splitString(std::string& line);
		const std::vector<class Server>& getServerVector() const {
			return (serv_vec);
		}
		std::vector<class Socket>& getSocketVector() {
			return (sock_vec);
		}
		std::vector<pollfd>& getPollVector() {
			return (pollVec);
		}
		std::string checkPath(std::string path);
};


#endif
