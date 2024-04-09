#include <webserv.hpp>

int	receive(int fd, std::vector<unsigned char>*buff, std::vector<class Socket>&sock_vec)
{
	int	result;

	if (checkIfCgiFd(fd, sock_vec))
		result = read(fd, buff->data(), BUFF_SIZE);
	else
		result = recv(fd, buff->data(), BUFF_SIZE, 0);
	if (result != -1)
	{
		(*buff).resize(result);
		return (result);
	}
	else
		return (result);
}

bool	checkIfListener(int poll_fd, std::vector<class Socket>&sock_vec, unsigned int size)
{
	for (unsigned int i = 0; i < size; i++)
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

Socket &findSocket(int socket_to_find, std::vector<Socket>&sock_vec, unsigned int size)
{	
	(void) size;

	for (unsigned int i = 0; i < sock_vec.size(); i++)
	{
		if (sock_vec[i].getFd() == socket_to_find || sock_vec[i].getCgiFd() == socket_to_find)
		if (sock_vec[i].getFd() == socket_to_find || sock_vec[i].getCgiFd() == socket_to_find)
			return (sock_vec[i]);
	}
	return (sock_vec[0]);
}

Socket &findListener(std::vector<Socket>&sock_vec, Socket &client, unsigned int size)
{	
	(void) size;

	for (unsigned int i = 0; i < sock_vec.size(); i++)
	{
		if (sock_vec[i].getFd() == client.pointingTo || sock_vec[i].getCgiFd() == client.pointingTo)
		if (sock_vec[i].getFd() == client.pointingTo || sock_vec[i].getCgiFd() == client.pointingTo)
			return (sock_vec[i]);
	}
	return (sock_vec[0]);	
}

string &bounceBuff(string &text, vector<unsigned char>&buff)
{
	for (size_t i = 0; i < buff.size(); i++)
		text += buff[i];
	return (text);
}

void	add_pollfd(std::vector<pollfd>&pollVec, std::vector<Socket>&sockVec, Socket &client, int fd, bool cgi)
{
	pollfd node;

	node.fd = fd;
	//cout << "fd added: " << fd << endl;
	node.events = POLLIN | POLLOUT;
	pollVec.push_back(node);
	if (!cgi)
		sockVec.push_back(client);
	if (!cgi)
		sockVec.push_back(client);
}

void	remove_pollfd(std::vector<pollfd> &pollVec, std::vector<Socket>&sockVec, int fd, unsigned int size)
{
	(void) size;

	for (unsigned int i = 0; i < pollVec.size(); i++)
	{
		if (pollVec[i].fd == fd)
		{
			pollVec.erase(pollVec.begin() + i);
			break ;
		}
	}
	for (unsigned int i = 0; i < sockVec.size(); i++)
	{
		if (sockVec[i].getFd() == fd)
		{
			sockVec.erase(sockVec.begin() + i);
			break ;
		}
	}
}