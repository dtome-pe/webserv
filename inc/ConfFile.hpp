#ifndef CONFFILE_HPP
#define CONFFILE_HPP

#include<webserv.hpp>

class ConfFile
{
	private:
		std::string file;
		std::vector<class Server>	serv_vec;
		int		fd_count;
		std::string _ip;
		std::string _port;
		std::string _server_name;
	public:
		pollfd 	*poll_ptr;
		int		fd_size;
		ConfFile(std::string _file);
		ConfFile();
		~ConfFile();
		void	parse_config(); 
		int		parse_element(std::string& line);
		int		countServers(std::string content);
		std::string findInfo(std::string line, std::string tofind);
		void	print_servers();
		void	init_serv();
		void	init_poll();
		void	poll_loop();
};

#endif
