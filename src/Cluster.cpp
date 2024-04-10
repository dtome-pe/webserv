#include "../inc/Cluster.hpp"

Cluster::Cluster()
{

}

void Cluster::parseConfig(char *file)
{
	_conf.parse_config(*this, file);
	_conf.print_servers();
	MIME::initializeMIME();
	MIME::initializeMIME();
}

void Cluster::setup()
{	
	for (size_t i = 0; i < _servVec.size(); i++)
	{
		for (size_t j = 0; j < _servVec[i].host_port.size(); j++)
		{
			Socket s(_servVec[i].host_port[j], &_servVec[i]); // se crea socket, con host y puerto, se resuelve host a direccion ip 
												// con getaddr info en constructor de socket.
				
			if (!look_for_same(s, _sockVec))	// buscamos socket creado con misma direccion:puerto
			{
				_sockVec.push_back(s); // si devuelve falso, introducimos socket en vector para posterior bind.	
			}
			//es decir, si encuentra ya un socket en el mismo puerto y direccion, lo ignoramos, ya que daria un error
			// y nginx no da error, simplemente solo abre uno.
		}
	}
	for (unsigned int i = 0; i < _sockVec.size(); i++)
	{
		_sockVec[i].start(); // se crean, se hace bind, se hace listen. cada socket.

		pollfd node;  // se genera nuevo nodo en vector de pollfd, que son los fds que poll 
										// va investigando si tienen algo que leer o no

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
		//cout << "poll: " << poll_count << endl;
		if (poll_count == -1)
		{
			print_error("poll error");
			exit(EXIT_FAILURE);
		}
		if (poll_count == 0)
			continue ;
		for (unsigned int i = 0; i < size; i++)
		{
			if (_pollVec[i].revents == 0)
			{	
				//cout << "revents 0. fd: " <<  _pollVec[i].fd << endl;
				continue ;
			}
			if (_pollVec[i].revents & POLLIN)
			{
				if (checkIfListener(_pollVec[i].fd, _sockVec, size))
				{
					cout << "Add client. fd es: " << _pollVec[i].fd << endl;
					addClient(i);
					break ;
				}
				else
				{
					cout << "Read from. fd es: " << _pollVec[i].fd  << endl;
					readFrom(i, &size);
				}
			}
			else if (_pollVec[i].revents & POLLOUT)
			{
				cout << "Write to. fd es: " << _pollVec[i].fd  << endl;
				writeTo(i, size);
			}
		}
	}	
}

int		Cluster::addClient(int i)
{
	while (true)
	{
		/*datos para nuevo cliente*/
		int c_fd;
		struct sockaddr_in c_addr;
		socklen_t addrlen = sizeof (sockaddr_in);
		/*datos para nueva conexion*/
		c_fd = accept(_pollVec[i].fd, (struct sockaddr *) &c_addr, &addrlen);
		if (c_fd == -1)
		{	
			if (errno != EAGAIN && errno != EWOULDBLOCK) // EAGAIN y EWOULDBLOCK se comprueban aparte
							//ya que en situacion de sockets no blocking, esto es un retorno normal
							// y de hecho es el retorno para salir del loop y gestionar peticion
			{
				cout << "accept: " << strerror(errno) << endl;
				return 1;
			}
			else			// aqui se sale al recibir EGAIN/EWOULDBOCK, es decir, ya no hay mas clientes que aceptar, arriba se comprueba cualquier otro -1, es decir, error.
				return 0;
		}
		else // mientras accept no de error o EAGAIN, creamos socket cliente, y anadimos al pollfd
		{
			Socket client(ip_to_str(&c_addr) + port_to_str(&c_addr), NULL);  // se construye socket cliente
						// pero con menos pasos que los listeners.
			client.pointTo(_pollVec[i].fd); // solo queremos saber a que listener apunta, esta relacionado
			client.setFd(c_fd);			// ponemos su fd, que es el retorno de accept.
			client.setNonBlocking(c_fd); // lo ponemos non blocking
			add_pollfd(_pollVec, _sockVec, client, c_fd, false); // y lo anadimos tanto al vector de poll, como al de sockets.
		}
	}
}

void	Cluster::readFrom(int i, unsigned int *size)
{
	string	text;
	int		nbytes;

	std::vector<unsigned char> buff(BUFF_SIZE);
	while (true)
	{
		text = "";
		nbytes = receive(_pollVec[i].fd, &buff, _sockVec); // recibimos respuesta (recv) o (read) si es el retorno de un proceso cgi (fd no-socket)
		cout << "nbytes leidos: " << nbytes << endl;
		if (nbytes == -1)
		{	
			if (errno != EAGAIN && errno != EWOULDBLOCK) // igual, estos dos errores son normales en non blocking
			{
				closeConnection(i, _pollVec, _sockVec, size);
				return ;
			}
			cout << "sale EAGAIN" << endl;
			if (_pollVec[i].fd == findSocket(_pollVec[i].fd, _sockVec, *size).getCgiFd())
			{
				for (unsigned int j = 0; j < *size; j++)
				{
					if (_pollVec[j].fd == findSocket(_pollVec[i].fd, _sockVec, *size).getFd())
						_pollVec[j].events = POLLIN | POLLOUT;
				}
			}
			else
				_pollVec[i].events = POLLIN | POLLOUT;
			return ;	// asi que si aparecen, todo bien, salimos de funcion
		}
		else if (nbytes == 0) // si 0, se ha cerrado conexion, tambien quitamos a cliente de vectores.
		{	
			cout << "entra en 0 bytes" << endl;
			closeConnection(i, _pollVec, _sockVec, size);
			return ;
		}
		bounceBuff(text, buff);
		findSocket(_pollVec[i].fd, _sockVec, *size).appendTextRead(text);
	}
}

void	Cluster::writeTo(int i, unsigned int size)
{
	//se construye request con el texto de un pollin previo y con el socket listener, para que nos de informacion
	// de a que ip y puerto iba destinado esta peticion. tambien en el constructor se determinara
	// bloque de server y location cuya configuracion se aplicara, tambien veremos si esta request tendra que gestionar el output de un proceso cgi.
	Request req(*this, findSocket(_pollVec[i].fd, _sockVec, size).getTextRead(), _servVec,
	findListener(_sockVec, findSocket(_pollVec[i].fd, _sockVec, _sockVec.size()), size), findSocket(_pollVec[i].fd, _sockVec, _sockVec.size()));
	int ret = this->handleClient(req);
	_pollVec[i].events = POLLIN;
	/*si hemos respondido con continue o cgi, ponemos al socket de
	cliente continue true y copiamos headers, porque lo siguiente que enviara sera el cuerpo
	directamente, sin headers.*/
	if (ret == CONTINUE || ret == CGI) 
		findSocket(_pollVec[i].fd, _sockVec, _sockVec.size()).bouncePrevious(req, ret);
	if (ret == CGI) // se ha iniciado proceso cgi
	{
		cout << "proceso cgi" << endl;
		add_pollfd(_pollVec, _sockVec, req.getSocket(), req.getSocket().getCgiFd(), true); // anadimos proceso cgi al pollfd
		return ;
	}
	if (req.getCgi()) // ya se ha gestionado el retorno del cgi en la respuesta dada
	{
		remove_pollfd(_pollVec, _sockVec, req.getSocket().getCgiFd(), size); // quitamos el cgi fd del poll y lo cerramos
		close(req.getSocket().getCgiFd());
		req.getSocket().setCgiFd(-1);
	}
	if (!req.getKeepAlive())
	{	
		cout << "entra en close conn" << endl;
		closeConnection(i, _pollVec, _sockVec, &size);
	}
}

int	Cluster::handleClient(Request &request)
{	
	Response	rsp; // declaramos response
	if (request.getCgi())
		rsp.setResponse(cgi(rsp, request, "", "output"), request);
	else
	{
		if (!request.good)   // comprobamos si ha habido algun fallo en el parseo para devolver error 400
			rsp.setResponse(400, request);
		/*si no es un metodo reconocido, devolvemos 501*/
		else if (request.getMethod() != "GET" && request.getMethod() != "PUT" && request.getMethod() != "DELETE" && request.getMethod() != "POST")
			rsp.setResponse(501, request);
		else
			/*iniciamos el flow para gestionar la peticion y ya seteamos respuesta segun el codigo*/
			rsp.setResponse(handleRequest(request, rsp, request.getServer(), request.getLocation()), request);
	}
	if (str_to_int(rsp.getCode()) == CGI)
		return (CGI);
	else
	{
		std::string response = rsp.makeResponse(); // hacemos respuesta con los valores del clase Response
		send(request.getSocket().getFd(), response.c_str(), response.length(), 0);
		cout << "response sent: " << endl;
		return (str_to_int(rsp.getCode())); // devolvemos codigo de respuesta para contemplar casos como el de 100 continue
	}
}

int		Cluster::handleRequest(Request &request, Response &response, const Server *serv, const Location *loc)
{
	/*comprobamos el path del request y realizamos comprobaciones pertinentes*/
	if (request.getHeader("Content-Length") != "not found")
	{
		if (str_to_int(request.getHeader("Content-Length")) > serv->getMaxBodySize())
			return (413);
	}
	std::string path = getPath(request, serv, loc); // tambien parseamos una posible question query, para conducir a archivo cgi de manera correcta
//	cout << "con path: " << path << " con method " << request.getMethod() << endl;
	if (path == "none") // no hay root directives, solo daremos una pagina de webserv si se accede al '/', si no 404
	{
		if (!check_method(request.getMethod(), NULL, serv)) // bloqueamos toda peticion que no sea GET, 405
			return (405);
		/*Si la peticion es al root, damos pagina default, si no 404*/
		if (request.getTarget() == "/")
		{
			response.setBody(readFileContents(request, "default/default.html"));
			return (200);
		}
		else
			return (404);
	}
	else
	{
		if (!check_method(request.getMethod(), loc, serv)) // si metodo no permitido, 405
		{
			return (405);
		}
		if (loc && loc->getRedirection().length() > 0) // comprobar si hay directive return
			return (loc->getRedirectionNumber());
		if (!checkGood(path))  // si el path que ha resultado no existe, comprobamos si es un put y se puede acceder a la carpeta previa
		{
			if (request.getMethod() == "PUT")
			{
				if (checkPutFile(path)) // si server puede acceder a la carpeta donde se quiere crear el archivo, hacemos put
					return (cgi(response, request, path, request.getMethod()));
			}
			else
				return (404);
		}
		if (request.getMethod() == "GET" &&  checkFileOrDir(path) == "dir" && !checkTrailingSlash(path))  // comprobamos si tiene o no trailing slash, nginx hace una redireccion 301 a URL con final slash
		{	
			request.setTrailSlashRedir(true);
			return (301);
		}
		if (request.getMethod() == "DELETE")
			return (cgi(response, request, path, request.getMethod()));
		if (request.getMethod() == "PUT")
		{	
			if (path[path.length() - 1] != '/')  // comprobamos que el put no tenga como target un directorio, entonces se devuelve 409
				return (cgi(response, request, path, request.getMethod()));
			else
				return (409);
		}
		if (checkFileOrDir(path) == "file")
		{
			if (checkCgi(request, path, loc)) // chequearemos si location tiene activado el cgi y para que extensiones
				return (cgi(response, request, path, request.getMethod()));
			else
			{
				response.setBody(readFileContents(request, path)); //sino servimos el recurso de manera normal
				return (200); 
			}
		}
		else // si corresponde a un directorio, primero miramos que no haya un index file
		{
			if (serv->getVIndex().size() > 0 || (loc && loc->getIndex().size() > 0)) // si hay index directive
			{
				std::string index_file = findIndex(path, serv, loc); // localizamos si el camino lleva a un archivo
				if (index_file != "")  // en caso afirmativo, se sirve
				{
					response.setBody(readFileContents(request, index_file));
					return (200);
				}
				else // si no, damos un forbidden, como nginx
					return (403);
			}
			if (findIndexHtml(path)) // sino, buscamos un archivo index.html para servir.
			{
				response.setBody(readFileContents(request, path + "index.html"));
				return (200);
			}
			else // sino, comprobamos si tiene autoindex activado para mostrar listado directorio
			{
				if (!loc || !loc->getAutoindex()) // si no tiene autoindex (como solo lo puede tener un location, de momento), devolvemos 403 ya que no esta activado el directorylisting
											// y no tenemos permiso para coger ningun archivo de directorio
					return (403);
				else
				{
					std::string content = generateDirectoryListing(path);
					if (content == "Error 500") // por si da algun error interno el server.
						return (500);
					else
					{
						response.setBody(content);
						return (200);
					}
				}
			}		
		}
	}
}

void	Cluster::closeConnection(int i, std::vector<pollfd>&_pollVec,
								std::vector<Socket>&_sockVec, unsigned int *size)
{
	cout << _pollVec[i].fd << " closed connection" << endl;
	close(_pollVec[i].fd);
	_pollVec[i].fd = -1;
	remove_pollfd(_pollVec, _sockVec, _pollVec[i].fd, *size);
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
