#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include <webserv.hpp>

#define CGI_TIMEOUT 5
#define POLL_TIMEOUT 1000

struct pidStruct
{
	pid_t pid;
	int fd;
	unsigned int time;
	Socket *client;
};

class Cluster
{
private:
	ConfFile _conf;
	std::vector<class Server> _servVec;
	std::vector<class Socket> _sockVec;
	std::vector<pollfd> _pollVec;
	std::vector<struct pidStruct> _pidVec;

public:
	Cluster();

	void 							parseConfig(char *file);
	void 							setup();
	void 							run();

	int 							addClient(int i);
	void 							readFrom(int i, unsigned int *size, int type, Socket &client);
	void							writeTo(int i, unsigned int size, Socket &client);
	
	void 							closeConnection(int i, std::vector<pollfd> &_pollVec, std::vector<Socket> &_sockVec, unsigned int *size);

	std::vector<Server> 			&getServerVector();
	std::vector<Socket> 			&getSocketVector();
	std::vector<pollfd> 			&getPollVector();

	void printVectors();

	/*time out utils*/
	void 							checkPids(unsigned int *size);
	void 							setPid(pid_t pid, unsigned int fd, Socket &client);
};

#endif
