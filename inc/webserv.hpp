#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "Server.hpp"
# include "Socket.hpp"
# include <netinet/in.h>
using namespace std;

class Server;

	typedef struct socket_list
	{
		Socket					*sock;
		struct socket_list		*next;
	}			t_sock;


	typedef struct	server_list
	{
		Server					*serv;
		struct server_list		*next;
	}			t_serv;

	typedef struct data
	{
		t_serv	*serv_list;
		pollfd 	*poll;
		int		fd_size;
		int		fd_count;
	} t_data;

	/*parse config*/
	int		parse_config(const std::string& file, t_data *data);

	int		parse_listen(t_sock **list, std::string &line);
	
	/*start server*/
	void	get_addr_info(struct addrinfo **s_addr, const char *port);
	int 	create_s(int server_fd, struct addrinfo *s_addr);
	int 	bind_s(int server_fd, struct addrinfo *s_addr);
	int 	listen_s(int server_fd);

	int		handle_client(int new_socket);

	/*poll*/
	void	poll_loop(t_data *data);

	/*print utils*/
	void	print_error(const char *str);
	void	print_servers(t_serv *list);

	/*socket list utils*/
	t_sock		*sock_new(std::string &port);
	int			sock_back(t_sock **lst, t_sock *new_s);
	t_sock		*sock_last(t_sock *lst);
	int			sock_count(t_sock *lst);

	/*server list utils*/

	t_serv		*serv_new();
	int			serv_back(t_serv **lst, t_serv *new_s);
	t_serv		*serv_last(t_serv *lst);

#endif