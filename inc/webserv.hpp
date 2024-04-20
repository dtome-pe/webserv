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

#define NOT_DONE 0
#define DONE 1
#define DONE_ERROR 2

#define BUFF_SIZE 100

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

/*Cluster utils*/

void    		createSocketAndAddToSockVecIfDifferent(std::vector<Server> &servVec,  std::vector<Socket> &sockVec, int i);
void 			startSocketAndAddToPollFd(std::vector<Socket> &sockVec, std::vector<pollfd> &pollVec, int i);
void    		setSignals();
int        		handleAcceptError();
int     		createNonBlockingClientSocketAndAddToPollAndSock(struct sockaddr_in c_addr, std::vector<pollfd> &pollVec, int i, int c_fd, std::vector<Socket> &sockVec);
void        	readFromPollhup(Socket &client, std::vector<pollfd> &pollVec);
void        	readEnough(int ret, std::vector<pollfd> &pollVec, Socket &client, int i);
void           	set400AndCloseConnection(Cluster &cluster, Socket &client, Request &req, int i, std::vector<pollfd> &pollVec,  std::vector<Socket> &sockVec, unsigned int *size);
void            setResponse(Cluster &cluster, Socket &client, Request &req, int i, std::vector<pollfd> &pollVec,  std::vector<Socket> &sockVec, unsigned int *size);
int             sendResponseAndReturnCode(Socket &client, Request &req);
void            clearClientAndSetPoll(Socket &client, std::vector<pollfd> &pollVec, int i);
void            removeCgiFdFromPollAndClose(std::vector<pollfd> &pollVec, std::vector<Socket> &sockVec, Request &req);

/*response utils*/

std::string 	getPath(Request &request, const Server *serv, const Location *loc);
std::string 	removeDoubleSlashes(const std::string &input);
std::string 	readFileContents(Request &request, const std::string &filename);
std::string 	getLengthAsString(std::string &content);
bool 			findIndexHtml(std::string &path);
std::string 	findIndex(std::string &path, const Server *serv, const Location *loc);
std::string 	generateDirectoryListing(const std::string &path);
std::string 	checkReturn(const Location *loc);


bool 			checkDefaultPath();
std::string 	getDefaultPath();
std::string 	getDefaultFile(const std::string &file);

bool			entityTooLarge(Request &request, const Server *serv);
int				noRoot(Response &response, Request &request, const Server *serv);
bool 			methodNotAllowed(std::string method, const Location *loc, const Server *serv);
bool 			pathIsGood(const std::string &path);
bool 			previousDirIsGood(std::string &path);
int				putOr404(Response &response, Request &request, std::string &path);
bool			thereIsTrailingSlash(std::string &path);
int				trailSlashRedir(Request &request);
bool			needTrailSlashRedir(Request &request, std::string &path);
int 			putOrDel(Response &response, Request &request, std::string &path);
int 			cgiOr200(Response &response, Request &request, std::string &path, const Location *loc);;
int				indexDirectiveOrIndexOrAutoIndex(Response &response, Request &request, std::string &path, const Server *serv, const Location *loc);

/*addToClient utils*/

int 			donePollhup(Socket &client, std::string &textRead);
int 			doneNoBody(Socket &client);
int 			contentLengthRequestDone(Socket &client, Request *request, std::string &textRead);
int 			contentLengthCgiOutputDone(Socket &client, Response *response, std::string &textRead, int *flag);
void 			parseRequestTillBody(Socket &client, std::string &textRead);
int 			parseCgiOutputTillBody(Socket &client, std::string &textRead, std::string text, int *flag);

/*cgi*/

bool 			checkCgi(Request &request, std::string &path, const Location *loc);
int 			cgi(Response &response, Request &request, std::string path, std::string method);
bool			checkPut(std::string &path);
bool 			checkPutFile(std::string &path);

char *const 	*setEnvp(Request &request, std::string &path, std::string &method);
char *const 	*setArgv(Request &request, std::string &path, std::string &method);
std::string 	bounceContent(int fd);
std::string 	parseCgiHeader(Response &response, std::string &content);
bool 			checkCgi(Request &request, std::string &path, const Location *loc);
int 			cgi(Response &response, Request &request, std::string path, std::string method);
bool 			checkPut(std::string &path);


char *const 	*setEnvp(Request &request, std::string &path, std::string &method);
char *const 	*setArgv(Request &request, std::string &path, std::string &method);
std::string 	bounceContent(int fd);
std::string 	parseCgiHeader(Response &response, std::string &content);

/*poll utils*/
int 			receive(int new_socket, std::string &text, std::vector<class Socket> &sock_vec);
bool 			checkIfListener(int poll_fd, std::vector<class Socket> &sock_vec);
bool 			checkIfCgiFd(int socket, std::vector<class Socket> &sock_vec);
bool 			checkIfCgiFd(int socket, std::vector<class Socket> &sock_vec);
Socket 			&findSocket(int socket_to_find, std::vector<Socket> &sock_vec);
Socket 			&findListener(std::vector<Socket> &sock_vec, Socket &client);
int				findPoll(std::vector<pollfd>&pollVec, Socket &sock);
string &		bounceBuff(string &text, vector<unsigned char> &buff);
void 			add_pollfd(std::vector<pollfd> &pollVec, std::vector<Socket> &sockVec, Socket &client, int fd, bool cgi);
void 			remove_pollfd(std::vector<pollfd> &pollVec, std::vector<Socket> &sockVec, int fd);

/*check GET path*/

std::string 	checkFileOrDir(const std::string &path);

/*utils*/

bool			checkIfHeader(std::string text);
void 			print_error(const char *str);
std::string 	ip_to_str(sockaddr_in *addr);
std::string 	port_to_str(sockaddr_in *addr);
std::string 	int_to_str(int n);
unsigned int 	str_to_int(std::string str);
std::string 	getCurrentTime();
std::string 	getLastModified(std::string path);
uint64_t 		timeEpoch();
bool 			look_for_same(Socket &sock, std::vector<Socket> &sock_vec);
uint64_t		hexStringToDecimalUint(const std::string& hexDigit);

#endif