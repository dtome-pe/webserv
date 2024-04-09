#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <lib.hpp>

using namespace std;

#define CONTINUE 100
#define CGI 42

#define GET 0
#define POST 1
#define DELETE 2
#define PUT 3

#define BUFF_SIZE 2000

class Server;

typedef struct socket_list
{
	Socket *sock;
	struct socket_list *next;
} t_sock;

typedef struct server_list
{
	Server *serv;
	struct server_list *next;
} t_serv;

typedef struct data
{
	t_serv *serv_list;
	// std::vector<Server>	serv_vec;
	pollfd *poll;
	int fd_size;
	int fd_count;
} t_data;

/*response utils*/
bool check_method(std::string method, const Location *loc, const Server *serv);
std::string getPath(Request &request, const Server *serv, const Location *loc);
bool check_method(std::string method, const Location *loc, const Server *serv);
std::string getPath(Request &request, const Server *serv, const Location *loc);
std::string removeDoubleSlashes(const std::string &input);
std::string readFileContents(Request &request, const std::string &filename);
std::string readFileContents(Request &request, const std::string &filename);
std::string getLengthAsString(std::string &content);
bool findIndexHtml(std::string &path);
std::string findIndex(std::string &path, const Server *serv, const Location *loc);
std::string findIndex(std::string &path, const Server *serv, const Location *loc);
std::string generateDirectoryListing(const std::string &path);
bool checkTrailingSlash(std::string &path);
std::string checkReturn(const Location *loc);
std::string checkReturn(const Location *loc);

bool checkDefaultPath();
std::string getDefaultPath();
std::string getDefaultFile(const std::string &file);

/*cgi*/

bool checkCgi(Request &request, std::string &path, const Location *loc);
int cgi(Response &response, Request &request, std::string path, std::string method);
int setDel(Request &request, std::string &path, std::string method);
int setPut(Response &response, Request &request, std::string &path, std::string method);
bool checkPut(std::string &path);
bool checkPutFile(std::string &path);

char *const *setEnvp(Request &request, std::string &path, std::string &method);
char *const *setArgv(Request &request, std::string &path, std::string &method);
std::string bounceContent(int fd);
std::string parseCgiHeader(Response &response, std::string &content);
bool checkCgi(Request &request, std::string &path, const Location *loc);
int cgi(Response &response, Request &request, std::string path, std::string method);
int setDel(Request &request, std::string &path, std::string method);
int setPut(Response &response, Request &request, std::string &path, std::string method);
bool checkPut(std::string &path);
bool checkPutFile(std::string &path);

char *const *setEnvp(Request &request, std::string &path, std::string &method);
char *const *setArgv(Request &request, std::string &path, std::string &method);
std::string bounceContent(int fd);
std::string parseCgiHeader(Response &response, std::string &content);

/*poll utils*/
int receive(int new_socket, std::vector<unsigned char> *buff, std::vector<class Socket> &sock_vec);
bool checkIfListener(int poll_fd, std::vector<class Socket> &sock_vec, unsigned int size);
bool checkIfCgiFd(int socket, std::vector<class Socket> &sock_vec);
bool checkIfCgiFd(int socket, std::vector<class Socket> &sock_vec);
Socket &findSocket(int socket_to_find, std::vector<Socket> &sock_vec, unsigned int size);
Socket &findListener(std::vector<Socket> &sock_vec, Socket &client, unsigned int size);
string &bounceBuff(string &text, vector<unsigned char> &buff);
void add_pollfd(std::vector<pollfd> &pollVec, std::vector<Socket> &sockVec, Socket &client, int fd, bool cgi);
void add_pollfd(std::vector<pollfd> &pollVec, std::vector<Socket> &sockVec, Socket &client, int fd, bool cgi);
void remove_pollfd(std::vector<pollfd> &pollVec, std::vector<Socket> &sockVec, int fd, unsigned int size);

/*check GET path*/
bool checkGood(const std::string &path);
std::string checkFileOrDir(const std::string &path);

/*utils*/
/*utils*/
void print_error(const char *str);
std::string ip_to_str(sockaddr_in *addr);
std::string port_to_str(sockaddr_in *addr);
std::string int_to_str(int n);
unsigned int str_to_int(std::string str);
std::string getCurrentTime();
std::string getLastModified(std::string path);
uint64_t timeEpoch();
bool look_for_same(Socket &sock, std::vector<Socket> &sock_vec);
std::string int_to_str(int n);
unsigned int str_to_int(std::string str);
std::string getCurrentTime();
std::string getLastModified(std::string path);
uint64_t timeEpoch();
bool look_for_same(Socket &sock, std::vector<Socket> &sock_vec);

#endif