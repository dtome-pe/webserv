#include "../inc/Cluster.hpp"

int stop = 0;

Cluster::Cluster()
{

}

void Cluster::parseConfig(std::string file)
{
	_conf.parse_config(*this, file);
	_conf.print_servers();
	MIME::initializeMIME();
}

void Cluster::setup()
{	
	for (size_t i = 0; i < _servVec.size(); i++)
		createSocketAndAddToSockVecIfDifferent(_servVec, _sockVec, i);
	for (unsigned int i = 0; i < _sockVec.size(); i++)
		startSocketAndAddToPollFd(_sockVec, _pollVec, i);
	reserveVectors();
}

void	Cluster::run()
{
	setSignals();
	while (true)
	{
		checkPids();
		remove_pollfd(_pollVec, _sockVec);
		int poll_count = poll(&_pollVec[0], _pollVec.size(), POLL_TIMEOUT);
		if (poll_count == -1)
		{
			if (errno == EINTR)
				return ;
			else
				throw std::runtime_error("poll error");
		}
		if (poll_count == 0)
			continue ;
		size_t size = _pollVec.size();
		for (unsigned int i = 0; i < size; i++)
		{
			if (_pollVec[i].revents == 0)
				continue ;
			if (_pollVec[i].revents & POLLIN || _pollVec[i].revents & POLLHUP)
			{
				if (checkIfListener(_pollVec[i].fd, _sockVec))
				{
					if (addClient(i) == 1)
						throw std::runtime_error("couldn't set the non-blocking mode on the file descriptor.");
					break ;
				}
				else
					readFrom(i, findSocket(_pollVec[i].fd, _sockVec));
			}
			else if (_pollVec[i].revents & POLLOUT)
				writeTo(i, findSocket(_pollVec[i].fd, _sockVec));
		}
	}	
}

int		Cluster::addClient(int i)
{
	while (true)
	{
		int c_fd;
		struct sockaddr_in c_addr;
		socklen_t addrlen = sizeof (sockaddr_in);

		c_fd = accept(_pollVec[i].fd, (struct sockaddr *) &c_addr, &addrlen);
		if (c_fd == -1)
			return (handleAcceptError());
		else
			createNonBlockingClientSocketAndAddToPollAndSock(c_addr, _pollVec, i, c_fd, _sockVec);
	}
}

void	Cluster::readFrom(unsigned int i, Socket &client)
{	
	cout << "i: " << i << " Pollin. fd es: " << _pollVec[i].fd << endl;
	int		nbytes;
	std::string text = "";
	nbytes = receive(_pollVec[i].fd, text, _sockVec);
	//cout << "nbytes leidos: " << nbytes << endl;
	if (nbytes == 0 && client.getRequest() && client.getRequest()->getCgi())
	{	
		readNothing(client, _pollVec);
		return (closeConnection(i, _pollVec, _sockVec));
	}	
	else if (nbytes == -1 || nbytes == 0)
		return (closeConnection(i, _pollVec, _sockVec));
	else
	{
		if (client.getRequest() == NULL)
			client.setRequest(new Request(*this, _servVec, findListener(_sockVec, client), client));
		int ret = client.addToClient(text, client.getRequest()->getCgi(), POLLIN);
		if (ret == DONE || ret == DONE_ERROR)
		{
			readEnough(ret, _pollVec, client, i);
			client.getTextRead().clear();
		}
	}
}

void	Cluster::writeTo(unsigned int i, Socket &client)
{
	cout << "i: " << i  << " Pollout. pollfd es: " << _pollVec[i].fd << ". client fd: " << client.getFd() << endl;

	//cout << "req en write to:  " << client.getRequest()->makeRequest() << endl;
	if (!client.getResponse())
		client.setResponse(new Response());
	setResponse(*this, client, *client.getRequest(), i);

	int ret;
	if (str_to_int(client.getResponse()->getCode()) == CGI)
		ret = CGI;
	else
	{
		ret = sendResponseAndReturnCode(client);
		cout << "response sent: " << endl;
	}
	if (ret == 0 || ret == -1)
	{
		cout << "error send" << endl;
		closeConnection(i, _pollVec, _sockVec);
		return ;
	}

	if (ret == CONTINUE || ret == CGI) 
		client.bouncePrevious(*client.getRequest(), ret);
	if (ret == CGI) // se inicia proceso cgi
	{
		cout << "proceso cgi" << endl;
		deleteRequestAndResponse(client.getRequest(), client.getResponse());
		clearClientAndSetPoll(client, _pollVec, i);
		return (add_pollfd(_pollVec, _sockVec, client, client.getCgiFd(), true));
	}
	if ((*client.getRequest()).getCgi()) // nos ha llegado el output del cgi
		closeCgiFd(i, _pollVec, client);
	if ((*client.getRequest()).getKeepAlive() == false)
		closeConnection(i, _pollVec, _sockVec);
	deleteRequestAndResponse(client.getRequest(), client.getResponse());
	clearClientAndSetPoll(client, _pollVec, i);
}

void	Cluster::closeConnection(unsigned int i, std::vector<pollfd>&pollVec, std::vector<Socket>&sockVec)
{
	cout << pollVec[i].fd << " closed connection" << endl;
	for (unsigned int j = 0; j < sockVec.size(); j++)
	{
		if (sockVec[j].getFd() == pollVec[i].fd)
		{
			sockVec[j].setFd(-1);
			break ;
		}

	}
	close(pollVec[i].fd);
	pollVec[i].fd = -1;
}

std::vector<Server>& Cluster::getServerVector()
{
	return (_servVec);
}

std::vector<Socket>& Cluster::getSocketVector()
{
	return (_sockVec);
}

std::vector<pollfd>& Cluster::getPollVector()
{
	return (_pollVec);
}

std::vector<pidStruct>& Cluster::getPidVector()
{
	return (_pidVec);
}

void Cluster::printVectors() 
{
    std::cout << "Server Vector:" << std::endl;
    for (unsigned int i = 0; i < _servVec.size(); ++i) 
	{
        std::cout << BGRED "Server " << i + 1 << ":" RESET << std::endl;
		_servVec[i].printHostPort();
		_servVec[i].printIpPort();
		_servVec[i].printServer_Names();
		_servVec[i].printRoot();
		_servVec[i].printErrorPages();
		_servVec[i].printindex();
		_servVec[i].printLocations();
    }

    std::cout << "Socket Vector:" << std::endl;
    for (unsigned int i = 0; i < _sockVec.size(); i++)
	{
		cout << "Socket " << i + 1 <<  " with fd " << _sockVec[i].getFd()  << ". Is listener? " << _sockVec[i].listener << endl
		<< "IP: " << _sockVec[i].getIp() << " Port: " << _sockVec[i].getPort() << endl;  
	}

    std::cout << "Poll Vector:" << std::endl;
	for (unsigned int i = 0; i < _pollVec.size(); i++)
	{
		std::cout << i << " socket fd: " << _pollVec[i].fd << std::endl;
	}
}

void    Cluster::reserveVectors()
{
	_sockVec.reserve(10000);
	_pollVec.reserve(10000);
	_pidVec.reserve(10000);
	_servVec.reserve(200);
}

void	Cluster::checkPids()
{
	int		status;
	
	std::vector<pidStruct>::iterator it = _pidVec.begin();

	while (it != _pidVec.end())
	{
		if (0 == kill(it->pid, 0))
		{
			if (waitpid(it->pid, &status, WNOHANG) > 0) 
			{
				if (WIFEXITED(status))
				{	
					killZombieProcess(it, _pidVec);
				}
			}
			else 
			{
				if (timeEpoch() - it->time > CGI_TIMEOUT)
				{
					killTimeoutProcessAndDisconnectClient(*this, _pollVec, _pidVec, _sockVec, it);
				}
			}		
		}
		else
			it = _pidVec.erase(it);
	}
}

void	Cluster::setPid(pid_t pid, unsigned int fd, Socket &client)
{
	struct pidStruct	node;
	node.pid = pid;
	node.fd = fd;
	node.time = timeEpoch();
	node.client = &client;

	_pidVec.push_back(node);
}

void	Cluster::clean()
{
	cout << "entra en clean" << endl;

	for (unsigned int i = 0; i < _sockVec.size(); i++)
	{
		freeaddrinfo(_sockVec[i].s_addr);
		delete _sockVec[i].getResponse();
		delete _sockVec[i].getRequest();
	}

}