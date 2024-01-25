#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <netinet/in.h>

	/*start server*/
	void	get_addr_info(struct addrinfo **s_addr);
	int 	create_s(int server_fd, struct addrinfo *s_addr);
	int 	bind_s(int server_fd, struct addrinfo *s_addr);
	int 	listen_s(int server_fd);

	int		handle_client(int new_socket);

	/*utils*/
	void	print_error(const char *str);

#endif