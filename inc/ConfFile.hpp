#ifndef CONFFILE_HPP
#define CONFFILE_HPP

#include<webserv.hpp>

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
		int		parse_element(std::string &line);
		void	print_servers();
		void	init_serv();
		void	init_poll();
		void	poll_loop();
};

#endif
