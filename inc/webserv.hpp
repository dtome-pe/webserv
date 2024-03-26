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
	
	/*find blocks*/
	bool						look_for_same(Socket &sock, std::vector<Socket>&sock_vec);
	const Server 				*find_serv_block(const std::vector<class Server> &serv, Request &request);
	const Location 				*find_loc_block(const Server *serv, Request &req);

	/*response utils*/
	bool 						check_method(std::string method, const Location *loc, const Server *serv);
	std::string 				getPath(Request &request, const Server *serv, const Location *loc);
	std::string 				removeDoubleSlashes(const std::string& input);
	std::string 				readFileContents(const std::string& filename);
	std::string 				getLengthAsString(std::string &content);
	bool 						findIndexHtml(std::string &path);
	std::string					findIndex(std::string &path, const Server *serv, const Location *loc);
	std::string 				generateDirectoryListing(const std::string& path);
	bool						checkTrailingSlash(std::string &path);
	std::string 				checkReturn(const Location *loc);

	void						setResponse(int code, Response &response, std::string arg, const Server *serv, const Location *loc);

	bool 						checkDefaultPath();
	std::string 				getDefaultPath();
	std::string 				getDefaultFile(const std::string &file);
	void						makeDefault(int code, Response &response, const std::string &file, const Server *serv);

	/*cgi*/

	bool						checkCgi(Request &request, std::string &path, const Location *loc);
	int							cgi(Response &response, Request &request, std::string &path, std::string method);
	int							setDel(Request &request, std::string &path, std::string method);
	int							setPut(Response &response, Request &request, std::string &path, std::string method);
	bool						checkPut(std::string &path);
	bool						checkPutFile(std::string &path);

	char* const* 				setEnvp(Request &request, std::string &path, std::string &method);
	char* const*				setArgv(Request &request, std::string &path, std::string &method);
	std::string 				bounceContent(int fd);
	std::string	 				parseCgiHeader(Response &response, const std::string& content);

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

	/*utils*/
	void						print_error(const char *str);
	std::string					ip_to_str(sockaddr_in *addr);
	std::string 				port_to_str(sockaddr_in *addr);
	std::string					int_to_str(int n);
	std::string					getCurrentTime();

#endif