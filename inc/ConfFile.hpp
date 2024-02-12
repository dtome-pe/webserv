#ifndef CONFFILE_HPP
#define CONFFILE_HPP

#include<iostream>
#include<fstream>
#include<vector>
#include<Server.hpp>
#include<Socket.hpp>

class ConfFile
{
	private:
		std::string file;
		std::vector<class Server>	serv_vec;
		pollfd 	*poll;
		int		fd_size;
		int		fd_count;
	public:
		ConfFile(std::string _file);
		~ConfFile();
		void	parse_config(); 
		int		parse_element(std::string &line);
		void	print_servers();
		void	init_serv();
		void	init_poll();
		void	poll_loop();
};

#endif
