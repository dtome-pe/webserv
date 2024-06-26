#include <webserv.hpp>

int	receive(int fd, std::string &text, std::vector<class Socket>&sock_vec)
{
	int	result;
	std::vector<unsigned char> buff(BUFF_SIZE);

	if (checkIfCgiFd(fd, sock_vec))
		result = read(fd, buff.data(), BUFF_SIZE);
	else
		result = recv(fd, buff.data(), BUFF_SIZE, 0);
	if (result != -1 && result != 0)
	{
		bounceBuff(text, buff, result);
		return (result);
	}
	else
		return (result);
}

bool	checkIfListener(int poll_fd, std::vector<class Socket>&sock_vec)
{
	for (unsigned int i = 0; i < sock_vec.size(); i++)
	{
		if (poll_fd == sock_vec[i].getFd() && sock_vec[i].listener)
			return (true);
	}
	return (false);
}

bool	checkIfCgiFd(int fd, std::vector<class Socket>&sock_vec)
{
	for (unsigned int i = 0; i < sock_vec.size(); i++)
	{
		if (fd == sock_vec[i].getCgiFd())
			return (true);
	}
	return (false);
}

int		findPoll(std::vector<pollfd>&pollVec, Socket &sock)
{
	for (unsigned int i = 0; i < pollVec.size(); i++)
	{
		if (pollVec[i].fd == sock.getFd())
			return (i);
	}
	return (0);
}

Socket &findSocket(int socket_to_find, std::vector<Socket>&sock_vec)
{
	for (unsigned int i = 0; i < sock_vec.size(); i++)
	{
		if (sock_vec[i].getFd() == socket_to_find || sock_vec[i].getCgiFd() == socket_to_find)
			return (sock_vec[i]);
	}
	return (sock_vec[0]);
}

Socket &findListener(std::vector<Socket>&sock_vec, Socket &client)
{
	for (unsigned int i = 0; i < sock_vec.size(); i++)
	{
		if (sock_vec[i].getFd() == client.pointingTo || sock_vec[i].getCgiFd() == client.pointingTo)
			return (sock_vec[i]);
	}
	return (sock_vec[0]);	
}

string &bounceBuff(string &text, vector<unsigned char>&buff, size_t nbytes)
{
	for (size_t i = 0; i < nbytes; i++)
		text += buff[i];
	return (text);
}

void	add_pollfd(std::vector<pollfd>&pollVec, std::vector<Socket>&sockVec, Socket &client, int fd, bool cgi)
{
	pollfd node;

	node.fd = fd;
	node.events = POLLIN;
	pollVec.push_back(node);
	if (!cgi)
		sockVec.push_back(client);
}

void	remove_pollfd(std::vector<pollfd> &pollVec, std::vector<Socket>&sockVec)
{	
	vector<pollfd>::iterator it = pollVec.begin();
	int count = 0;

	while(it != pollVec.end()) 
	{

		if(it->fd == -1) 
		{
			it = pollVec.erase(it);
			count++;
		}
		else ++it;
	}

	vector<Socket>::iterator it2 = sockVec.begin();

	while(it2 != sockVec.end()) 
	{

		if(it2->getFd() == -1) 
		{
			delete it2->getRequest();
			delete it2->getResponse();
			it2 = sockVec.erase(it2);
		}
		else ++it2;
	}
}