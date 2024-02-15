#ifndef CONFFILE_HPP
#define CONFFILE_HPP

#include "Colors.hpp"
#include <webserv.hpp>


class ConfFile
{
	private:
		std::string file;
		std::vector<class Server>	serv_vec;
		int		fd_count;
	public:
		pollfd 	*poll_ptr;
		int		fd_size;
		ConfFile(std::string _file);
		~ConfFile();
		void	parse_config(); 
		int		parse_element(std::string& content, int i);
		int		countServers(std::string content);
		std::string findInfo(std::string line, std::string tofind, std::string found);
		void	findIp(Socket& S, std::string newserv);

		int		check_info(Socket S);

		void	print_servers();
		void	init_serv();
		void	init_poll();
		void	poll_loop();
		const std::vector<class Server>& getServerVector() const {
			return (serv_vec);
		}
};
/*
const std::vector<class Server>& ConfFile::getServerVector() const {
    return (serv_vec);
}*/

#endif
