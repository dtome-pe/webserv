#ifndef CONFFILE_HPP
#define CONFFILE_HPP

#include "Colors.hpp"
#include "Locations.hpp"
#include <cctype>
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
		void	trimSpaces(std::string& str);
		void	parse_location(std::string line, Locations &loc);
		int		parse_element(std::string& content, int i);
		int		countServers(std::string content);
		std::string findInfo(std::string line, std::string tofind);
		void	findIp(Server& Serv, std::string newserv); // primero parseamos informacion en servers

		int		check_info();

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
};


#endif
