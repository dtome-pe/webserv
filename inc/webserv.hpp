#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "Server.hpp"
# include <netinet/in.h>
using namespace std;

	typedef struct server_list
	{
		Server					*s;
		struct server_list		*next;
	}			t_s;

	typedef struct data
	{
		t_s		*s_list;
		pollfd 	*poll;
		int		fd_size;
		int		fd_count;
	} t_data;

	/*parse config*/
	int		parse_config(const std::string& file, t_data *data);

	int		parse_listen(t_s *list, std::string &line);
	
	/*start server*/
	void	get_addr_info(struct addrinfo **s_addr, const char *port);
	int 	create_s(int server_fd, struct addrinfo *s_addr);
	int 	bind_s(int server_fd, struct addrinfo *s_addr);
	int 	listen_s(int server_fd);

	int		handle_client(int new_socket);

	/*poll*/
	void	poll_loop(t_data *data, t_s *list);

	/*print utils*/
	void	print_error(const char *str);
	void	print_servers(t_data *data);

	/*server list utils*/
	t_s		*s_new();
	int		s_back(t_s **lst, t_s *new_s);
	t_s		*s_last(t_s *lst);

#endif