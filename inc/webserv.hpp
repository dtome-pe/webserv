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



	void						init_servers(t_data *data, t_serv *lst);
	void						init_poll(t_data *data, t_serv *lst);
	void						init_data(t_data *data);

	/*parse config*/
	int							parse_config(const std::string& file, t_data *data);

	int							parse_listen(t_sock **list, std::string &line);
	
	/*start server*/
	void						get_addr_info(struct addrinfo **s_addr, const char *host, const char *port);
	int 						create_s(int server_fd, struct addrinfo *s_addr, struct sockaddr_in sock_addr, socklen_t sock_addrlen);
	int 						bind_s(int server_fd, struct addrinfo *s_addr, std::string ip);
	int 						listen_s(int server_fd);

	bool						look_for_same(Socket &sock, std::vector<Socket>&sock_vec);

	const Server 				*find_serv_block(const std::vector<class Server> &serv, Request &request);

	const Locations 			*find_loc_block(const Server *serv, Request &req);

	/*response utils*/
	bool 						check_method(std::string method, const Locations *loc);
	std::string 				getPath(Request &request, const Server *serv, const Locations *loc);
	std::string 				removeDoubleSlashes(const std::string& input);
	std::string 				readFileContents(const std::string& filename);
	std::string 				getLengthAsString(std::string &content);
	bool 						findIndexHtml(std::string &path);
	std::string					findIndex(std::string &path, const Server *serv, const Locations *loc);
	std::string 				generateDirectoryListing(const std::string& path);
	bool						checkTrailingSlash(std::string &path);
	std::string 				checkReturn(const Locations *loc);

	void						setResponse(int code, Response &response, std::string arg, const Server *serv, const Locations *loc);

	bool 						checkDefaultPath();
	std::string 				getDefaultPath();
	std::string 				getDefaultFile(const std::string &file);
	void						makeDefault(Response &response, const std::string &file, const Server *serv);

	bool						checkCgi(std::string &path);

	/*cgi utils*/
	char* const* 				setEnvp(Request &request, std::string &path);
	char* const*				setArgv(std::string &path);
	std::string 				bounceContent(int *pipe_fd);
	std::string 				getCgiHeader(const std::string& content, const std::string &header);
	std::string					removeHeaders(std::string &content);
	void						removeHeaderLine(std::string& content);

	/*poll utils*/
	int							receive_response(int new_socket, std::vector<unsigned char> *buff);
	bool						checkIfListener(int poll_fd, std::vector<class Socket>&sock_vec, unsigned int size);
	Socket						&findSocket(int socket_to_find, std::vector<Socket>&sock_vec,  unsigned int size);
	Socket						&findListener(std::vector<Socket>&sock_vec, Socket &client, unsigned int size);
	string						&bounceBuff(string &text, vector<unsigned char>&buff);
	void						add_pollfd(std::vector<pollfd>&pollVec, std::vector<Socket>&sockVec, Socket &client, int fd);
	void						remove_pollfd(std::vector<pollfd> &pollVec, std::vector<Socket>&sockVec, int fd, unsigned int size);
	
	/*check GET path*/
	bool						checkGood(const std::string &path);
	std::string 				checkFileOrDir(const std::string &path);

	/*poll*/
	//void	poll_loop(t_data *data);
	void						poll_loop(std::vector<pollfd> &pollVec, ConfFile &conf);

	/*print utils*/
	void						print_error(const char *str);

	/*ip utils*/
	std::string					ip_to_str(sockaddr_in *addr);
	std::string 				port_to_str(sockaddr_in *addr);

#endif