#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include<lib.hpp>

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
		//std::vector<Server>	serv_vec;
		pollfd 	*poll;
		int		fd_size;
		int		fd_count;
	} t_data;



	void	init_servers(t_data *data, t_serv *lst);
	void	init_poll(t_data *data, t_serv *lst);
	void	init_data(t_data *data);

	/*parse config*/
	int		parse_config(const std::string& file, t_data *data);

	int		parse_listen(t_sock **list, std::string &line);
	
	/*start server*/
	void	get_addr_info(struct addrinfo **s_addr, const char *host, const char *port);
	int 	create_s(int server_fd, struct addrinfo *s_addr);
	int 	bind_s(int server_fd, struct addrinfo *s_addr, std::string ip);
	int 	listen_s(int server_fd);

	int		handle_client(int new_socket, ConfFile &conf, sockaddr_in &c_addr, sockaddr_in &sock_addr);

	const Server *find_serv_block(const std::vector<class Server> &serv, Request &request);

	const Locations *find_loc_block(const Server *serv, Request &req);

	/*response utils*/
	bool check_method(std::string method, const Locations *loc);
	std::string get_path(Request &request, const Server *serv, const Locations *loc);
	std::string removeDoubleSlashes(const std::string& input);

	/*poll*/
	//void	poll_loop(t_data *data);
	void	poll_loop(pollfd *poll, int size, ConfFile &conf);

	/*print utils*/
	void	print_error(const char *str);

	/*ip utils*/
	std::string ip_to_str(sockaddr_in *addr);

#endif