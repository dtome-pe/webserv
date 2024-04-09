#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include <webserv.hpp>

#define TIMEOUT 5

struct pidStruct
{
	pid_t pid;
	int fd;
	unsigned int time;
	Socket *client;
};

#define TIMEOUT 5

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
	ConfFile _conf;
	std::vector<class Server> _servVec;
	std::vector<class Socket> _sockVec;
	std::vector<pollfd> _pollVec;
	std::vector<struct pidStruct> _pidVec;

public:
	Cluster();

	void parseConfig(char *file);
	void setup();
	void run();

	int handleClient(Request &request);
	int handleRequest(Request &request, Response &response, const Server *serv, const Location *loc);
	void closeConnection(int i, std::vector<pollfd> &_pollVec, std::vector<Socket> &_sockVec, unsigned int *size);

	std::vector<Server> &getServerVector();
	std::vector<Socket> &getSocketVector();
	std::vector<pollfd> &getPollVector();

	void printVectors();

	/*time out utils*/
	void checkPids(unsigned int *size);
	void setPid(pid_t pid, unsigned int fd, Socket &client);

	int addConnection(int i);
	void readFromConnection(int i, unsigned int *size);
};

#endif
