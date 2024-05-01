#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include <webserv.hpp>

#define CGI_TIMEOUT 5
#define POLL_TIMEOUT 10000

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

	void 							parseConfig(std::string	file);
	void 							setup();
	void 							run();
	void							clean();

	int 							addClient(int i);
	void 							readFrom(unsigned int i, Socket &client);
	void							writeTo(unsigned int i, Socket &client);
	
	void 							closeConnection(unsigned int i, std::vector<pollfd> &_pollVec, std::vector<Socket>&_sockVec);

	void							reserveVectors();

	std::vector<Server> 			&getServerVector();
	std::vector<Socket> 			&getSocketVector();
	std::vector<pollfd> 			&getPollVector();
	std::vector<pidStruct>			&getPidVector();

	void printVectors();

	/*time out utils*/
	void 							checkPids();
	void 							setPid(pid_t pid, unsigned int fd, Socket &client);
};

#endif
