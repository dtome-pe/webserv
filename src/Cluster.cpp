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
	{
		for (size_t j = 0; j < _servVec[i].host_port.size(); j++)
		{
			Socket s(_servVec[i].host_port[j], &_servVec[i]);		
			if (!look_for_same(s, _sockVec))	// buscamos socket creado con misma direccion:puerto
				_sockVec.push_back(s);
		}
	}
	for (unsigned int i = 0; i < _sockVec.size(); i++)
	{
		_sockVec[i].start(); // se crean, se hace bind, se hace listen. cada socket.
		pollfd node;
		node.fd = _sockVec[i].getFd();
		node.events = POLLIN; // esto es lo que determina que estamos interesados en eventos de lectura, al ser los sockets listeners
		_pollVec.push_back(node);
	}
}

void	Cluster::run()
{
	signal(SIGINT, SIG_DFL);
	signal(SIGPIPE, SIG_IGN);
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
					cout << "Read from. fd es: " << _pollVec[i].fd  << endl;
					readFrom(i, &size, POLLIN);
				}
			}
			else if (_pollVec[i].revents & POLLHUP)
			{
					cout << "Pollhup. fd es: " << _pollVec[i].fd << endl;
					readFrom(i, &size, POLLHUP);	
			}
			else if (_pollVec[i].revents & POLLOUT)
			{
				cout << "Write to. fd es: " << _pollVec[i].fd  << endl;
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
		{	
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				cout << "accept: " << strerror(errno) << endl;
				return 1;
			}
			else
				return 0;
		}
		else
		{
			Socket client(ip_to_str(&c_addr) + port_to_str(&c_addr), NULL);
			client.pointTo(_pollVec[i].fd);
			client.setFd(c_fd);
			if (client.setNonBlocking(c_fd) == 1)
			   return (1);
			add_pollfd(_pollVec, _sockVec, client, c_fd, false);
		}
	}
}

void	Cluster::readFrom(int i, unsigned int *size, int type)
{
	if (type == POLLHUP)
	{
		cout << "entra en pollhup" << endl;
		findSocket(_pollVec[i].fd, _sockVec).addToClient("", findSocket(_pollVec[i].fd, _sockVec).getRequest()->getCgi(), POLLHUP);
		_pollVec[findPoll(_pollVec, findSocket(_pollVec[i].fd, _sockVec))].events = POLLIN | POLLOUT; // vamos anadiendo a request, si request ha acabado, pondriamos fd en pollout
		findSocket(_pollVec[i].fd, _sockVec).getRequest()->otherInit();
	}
	int		nbytes;

	std::string text = "";
	std::vector<unsigned char> buff(BUFF_SIZE);
	nbytes = receive(_pollVec[i].fd, buff, _sockVec);
	//cout << "nbytes leidos: " << nbytes << endl;
	if (nbytes == -1)
	{	
		closeConnection(i, _pollVec, _sockVec, size);
		return ;
	}
	else if (nbytes == 0) // si 0, se ha cerrado conexion, tambien quitamos a cliente de vectores.
	{	
		cout << "entra en 0 bytes" << endl;
		closeConnection(i, _pollVec, _sockVec, size);
		return ;
	}
	else
	{
		if (!findSocket(_pollVec[i].fd, _sockVec).getRequest())
		{	
			//cout << "se crea request" << endl;
			findSocket(_pollVec[i].fd, _sockVec).setRequest(new Request(*this, _servVec, findListener(_sockVec, findSocket(_pollVec[i].fd, _sockVec)),
																			findSocket(_pollVec[i].fd, _sockVec)));
		}
		bounceBuff(text, buff);
		//cout << "text leido: " << text << endl;
		int ret = findSocket(_pollVec[i].fd, _sockVec).addToClient(text, findSocket(_pollVec[i].fd, _sockVec).getRequest()->getCgi(), POLLIN);
		if (ret == DONE)
		{
			cout << "DONE" << endl;
			_pollVec[i].events = POLLIN | POLLOUT; // vamos anadiendo a request, si request ha acabado, pondriamos fd en pollout
			findSocket(_pollVec[i].fd, _sockVec).getRequest()->otherInit();
		}
		else if (ret == DONE_ERROR)
		{
			cout << "DONE ERROR" << endl;
			close(_pollVec[i].fd);
			_pollVec[findPoll(_pollVec, findSocket(_pollVec[i].fd, _sockVec))].events = POLLIN | POLLOUT; // vamos anadiendo a request, si request ha acabado, pondriamos fd en pollout
			findSocket(_pollVec[i].fd, _sockVec).getRequest()->otherInit();
		}
	}
}

void	Cluster::writeTo(int i, unsigned int size, Socket &client)
{	
	int ret;
	Request &req = (*findSocket(_pollVec[i].fd, _sockVec).getRequest());
	cout << "entra en write to, req:"  << req.makeRequest() << endl;
	if (!client.getResponse())
		client.setResponse(new Response());
	if (client.getResponse()->getCode() != "") // si ya hay un code es que ha habido algun error previo
		client.getResponse()->setResponse(str_to_int(client.getResponse()->getCode()), req);
	else
	{
		if (req.getCgi())
			client.getResponse()->setResponse(cgi((*client.getResponse()), req, "", "output"), req);
		else
		{
			if (!req.good)   // comprobamos si ha habido algun fallo en el parseo para devolver error 400 y se cierra conexion
			{
				client.getResponse()->setResponse(400, req);
				closeConnection(i, _pollVec, _sockVec, &size);
			}
			/*si no es un metodo reconocido, devolvemos 501*/
			else if (req.getMethod() != "GET" && req.getMethod() != "PUT" && req.getMethod() != "DELETE" && req.getMethod() != "POST")
				client.getResponse()->setResponse(501, req);
			else
				/*iniciamos el flow para gestionar la peticion y ya seteamos respuesta segun el codigo*/
				client.getResponse()->setResponse(client.getResponse()->getResponseCode(req, req.getServer(), req.getLocation()), req);
		}	
	}
	if (str_to_int(client.getResponse()->getCode()) == CGI)
		ret = CGI;
	else
	{
		std::string response = client.getResponse()->makeResponse(); // hacemos respuesta con los valores del clase Response
		send(req.getClient().getFd(), response.c_str(), response.length(), 0);
		cout << "response sent: " << endl;
		ret = str_to_int(client.getResponse()->getCode()); // devolvemos codigo de respuesta para contemplar casos como el de 100 continue
	}
	_pollVec[i].events = POLLIN;
	client.setResponse(NULL);
	client.setRequest(NULL);
	/*si hemos respondido con continue o cgi, ponemos al socket de
	cliente continue true y copiamos headers, porque lo siguiente que enviara sera el cuerpo
	directamente, sin headers.*/
	if (ret == CONTINUE || ret == CGI) 
		findSocket(_pollVec[i].fd, _sockVec).bouncePrevious(req, ret);
	if (ret == CGI) // se ha iniciado proceso cgi
	{
		//cout << "proceso cgi" << endl;
		add_pollfd(_pollVec, _sockVec, req.getClient(), req.getClient().getCgiFd(), true); // anadimos proceso cgi al pollfd
		return ;
	}
	if (req.getCgi()) // ya se ha gestionado el retorno del cgi en la respuesta dada
	{
		remove_pollfd(_pollVec, _sockVec, req.getClient().getCgiFd()); // quitamos el cgi fd del poll y lo cerramos
		close(req.getClient().getCgiFd());
		req.getClient().setCgiFd(-1);
	}
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
		if (0 == kill(_pidVec[i].pid, 0)) // si pid esta activo, comprobamos timeout
		{
			if (waitpid(_pidVec[i].pid, &status, WNOHANG) > 0) 
			{
				if (WIFEXITED(status))
				{
					close(_pidVec[i].fd);
					kill(_pidVec[i].pid, SIGKILL);
					cout << "pid with fd: " << _pidVec[i].fd << endl;
					_pidVec.erase(_pidVec.begin() + i);
				}
			}
			else
			{
				if (timeEpoch() - _pidVec[i].time > CGI_TIMEOUT)
				{	
					cout << "Timeout!" << endl;
					for (unsigned int j = 0; j < _pollVec.size(); j++)
					{
						if (_pollVec[j].fd == _pidVec[i].fd)
							closeConnection(j, _pollVec, _sockVec, size);
						else if (_pollVec[j].fd == _pidVec[i].client->getFd())
							closeConnection(j, _pollVec, _sockVec, size);
					}
					close(_pidVec[i].fd);
					kill(_pidVec[i].pid, SIGKILL);
					waitpid(_pidVec[i].pid, NULL, 0);
					_pidVec.erase(_pidVec.begin() + i);
				}
			}
		}
		else							//si pid ya no existe, simplemente eliminamos nodo
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
