#ifndef CONFFILE_HPP
#define CONFFILE_HPP

#include "Colors.hpp"
#include "Locations.hpp"
#include <webserv.hpp>


class ConfFile
{
	private:
		std::string file;
		std::vector<class Server>	serv_vec;
		std::vector<class Socket>	sock_vec;
		int		fd_count;

	public:
		pollfd 	*poll_ptr;
		int		fd_size;
		ConfFile(std::string _file);
		~ConfFile();
		void	parse_config(); 
		void	parse_location(std::string line, Locations &loc);
		int		parse_element(std::string& content, int i);
		int		countServers(std::string content);
		std::string findInfo(std::string line, std::string tofind, std::string found);
		void	findIp(Server& Serv, std::string newserv); // primero parseamos informacion en servers

		int		check_info(Socket S);

		void	print_servers();
		void	print_sockets();
		void	create_sockets();
		void	start_sockets();
		void	init_poll();
		void	poll_loop();
		const std::vector<class Server>& getServerVector() const {
			return (serv_vec);
		}
};


#endif
