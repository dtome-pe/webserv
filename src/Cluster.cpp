#include "../inc/Cluster.hpp"

Cluster::Cluster()
{

}

void Cluster::parseConfig(char *file)
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
}

void	Cluster::run()
{
	setSignals();
	while (true)
	{
		unsigned int size = _pollVec.size();
		checkPids(&size);
		int poll_count = poll(&_pollVec[0], size, POLL_TIMEOUT);
		if (poll_count == -1)
			throw std::runtime_error("poll error");
		if (poll_count == 0)
			continue ;
		for (unsigned int i = 0; i < size; i++)
		{
			if (_pollVec[i].revents == 0)
				continue ;
			if (_pollVec[i].revents & POLLIN)
			{
				if (checkIfListener(_pollVec[i].fd, _sockVec))
				{
					cout << "Add client. fd es: " << _pollVec[i].fd << endl;
					if (addClient(i) == 1)
						throw std::runtime_error("couldn't set the non-blocking mode on the file descriptor.");
					break ;
				}
				else
				{
					cout << "Pollin. fd es: " << _pollVec[i].fd  << endl;
					readFrom(i, &size, findSocket(_pollVec[i].fd, _sockVec));
				}
			}
			else if (_pollVec[i].revents & POLLOUT)
			{
				cout << "Pollout. fd es: " << _pollVec[i].fd  << endl;
				writeTo(i, size, findSocket(_pollVec[i].fd, _sockVec));
			}
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
			return (createNonBlockingClientSocketAndAddToPollAndSock(c_addr, _pollVec, i, c_fd, _sockVec));
	}
}

void	Cluster::readFrom(int i, unsigned int *size, Socket &client)
{	
	int		nbytes;
	std::string text = "";
	nbytes = receive(_pollVec[i].fd, text, _sockVec);
	//cout << "nbytes leidos: " << nbytes << endl;
	if (nbytes == 0 && client.getRequest() && client.getRequest()->getCgi())
	{	
		readNothing(client, _pollVec);
		return (closeConnection(i, _pollVec, _sockVec, size));
	}	
	else if (nbytes == -1 || nbytes == 0)
		return (closeConnection(i, _pollVec, _sockVec, size));
	else
	{
		if (!client.getRequest())
			client.setRequest(new Request(*this, _servVec, findListener(_sockVec, findSocket(_pollVec[i].fd, _sockVec)), findSocket(_pollVec[i].fd, _sockVec)));
		int ret = client.addToClient(text, client.getRequest()->getCgi(), POLLIN);
		if (ret == DONE || ret == DONE_ERROR)
		{
			//cout << "entra en DONE" << endl;
			readEnough(ret, _pollVec, client, i);
			client.getTextRead().clear();
		}
	}
}

void	Cluster::writeTo(int i, unsigned int size, Socket &client)
{
	Request &req = (*client.getRequest());

	//cout << "req en write to: " << req.makeRequest() << endl;
	if (!client.getResponse())
		client.setResponse(new Response());
	setResponse(*this, client, req, i, _pollVec, _sockVec, &size);

	int ret;
	if (str_to_int(client.getResponse()->getCode()) == CGI)
		ret = CGI;
	else
		ret = sendResponseAndReturnCode(client, req);

	clearClientAndSetPoll(client, _pollVec, i);

	if (ret == CONTINUE || ret == CGI) 
		client.bouncePrevious(req, ret);
	if (ret == CGI) // se inicia proceso cgi
	{
		cout << "proceso cgi" << endl;
		return (add_pollfd(_pollVec, _sockVec, req.getClient(), req.getClient().getCgiFd(), true));
	}
	if (req.getCgi()) // nos ha llegado el output del cgi
		removeCgiFdFromPollAndClose(_pollVec, _sockVec, req);
	if (!req.getKeepAlive())
		closeConnection(i, _pollVec, _sockVec, &size);
}

void	Cluster::closeConnection(int i, std::vector<pollfd>&_pollVec,
								std::vector<Socket>&_sockVec, unsigned int *size)
{
	cout << _pollVec[i].fd << " closed connection" << endl;
	close(_pollVec[i].fd);
	remove_pollfd(_pollVec, _sockVec, _pollVec[i].fd);
	_pollVec[i].fd = -1;
	(*size)--;
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

void	Cluster::checkPids(unsigned int *size)
{
	int		status;
	
	for (unsigned int i = 0; i < _pidVec.size(); i++)
	{
		if (0 == kill(_pidVec[i].pid, 0))
		{
			if (waitpid(_pidVec[i].pid, &status, WNOHANG) > 0) 
			{
				if (WIFEXITED(status))
					killZombieProcess(_pidVec, i);
			}
			else
			{
				if (timeEpoch() - _pidVec[i].time > CGI_TIMEOUT)
					killTimeoutProcessAndDisconnectClient(*this, _pidVec, i, _pollVec, _sockVec, size);
			}
		}
		else
			_pidVec.erase(_pidVec.begin() + i);
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
