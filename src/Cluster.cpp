#include "../inc/Cluster.hpp"

Cluster::Cluster()
{

}

void Cluster::parseConfig(char *file)
{
	_conf.parse_config(*this, file);
	_conf.print_servers();
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
		node.events = POLLIN; // esto es lo que determina que estamos interesados en eventos de lectura. recv.
		_pollVec.push_back(node);
	}
}

static void	handler(int sig)
{
	if (sig == SIGINT)
	{	
		cout << "closing webserv..." << endl;
		exit(0);
	}
}

void	Cluster::run()
{
	signal(SIGINT, handler);
	signal(SIGPIPE, SIG_IGN);
	while (1)
	{
		unsigned int size = _pollVec.size();
		int poll_count = poll(&_pollVec[0], size, -1); 
		/*se hace el poll. el retorno es cuantos fds se han mostrado dispuestos a recibir informacion.
		es decir, o una nueva conexion (si es un fd listener, server) o algo a recibir si es un fd de un cliente conectado.*/
		if (poll_count == -1)
		{
			print_error("poll error");
			exit(EXIT_FAILURE);
		}
		for (unsigned int i = 0; i < size; i++) // iteramos a todo el vector
		{
			if (_pollVec[i].revents == 0) // buscamos que socket esta listo para recibir cliente
				continue ;
			if (_pollVec[i].revents & POLLIN) // hay alguien listo para leer = hay un intento de conexion
			{
				if (checkIfListener(_pollVec[i].fd, _sockVec, size)) // si se trata de un listener, aceptamos conexion
				{
				//	cout << "se entra en pollin listener fd:" << pollVec[i].fd << " i es: " << i << endl;
					while (true) 
					{
						/*datos para nueva conexion*/
						int c_fd;
						struct sockaddr_in c_addr;
						socklen_t addrlen = sizeof (sockaddr_in);
						/*datos para nueva conexion*/
						c_fd = accept(_pollVec[i].fd, (struct sockaddr *) &c_addr, &addrlen); // se acepta nuevo cliente
						if (c_fd == -1)
						{	
							if (errno != EAGAIN && errno != EWOULDBLOCK) // EAGAIN y EWOULDBLOCK se comprueban aparte
											//ya que en situacion de sockets no blocking, esto es un retorno normal
											// y de hecho es el retorno para salir del loop y gestionar peticion
							{
								cout << "accept: " << strerror(errno) << endl;
								exit(1);
							}
							else			// aqui se sale al recibir EGAIN, arriba se comprueba cualquier otro -1, es decir, error.
								break ;
						}
						else
						{
							Socket client(ip_to_str(&c_addr) + port_to_str(&c_addr), NULL);  // se construye socket cliente
										// pero con menos pasos que los listeners.
							client.pointTo(_pollVec[i].fd); // solo queremos saber a que listener apunta, esta relacionado
							client.setFd(c_fd);			// ponemos su fd, que es el retorno de accept.
							client.setNonBlocking(c_fd); // lo ponemos non blocking
							add_pollfd(_pollVec, _sockVec, client, c_fd); // y lo anadimos tanto al vector de poll, como al de sockets.
						}
					}
				}
				else // si no es listener el evento activado, es que es un cliente listo para transmitir informacion
				{	
					//cout << "se entra en pollin cliente fd:" << pollVec[i].fd << " i es" << i << endl;
					string	text = "";
					int		nbytes;
					int		flag = 1;
					while (true)
					{
						std::vector<unsigned char> buff(5000);
						nbytes = receive_response(_pollVec[i].fd, &buff); // recibimos respuesta (recv)
						//cout << "nbytes result: " << nbytes << endl;
						if (nbytes == -1)
						{	
							if (errno != EAGAIN && errno != EWOULDBLOCK) // igual, estos dos errores son normales en non blocking
							{
								closeConnection(i, _pollVec, _sockVec, &size, &flag);
								break ;
							}
							//cout << "EAGAIN / EWOULDBLOCK" << endl; salimos en caso EAGAIN para gestionar informacion
							break ;	
						}
						else if (nbytes == 0) // si 0, se ha cerrado conexion, tambien quitamos a cliente de vectores.
						{	
							closeConnection(i, _pollVec, _sockVec, &size, &flag);
							break ;
						}
						bounceBuff(text, buff); // vamos bounceando el buffer en una string, mientras siga el loop.
					}
					if (flag) //  si no ha habido un break por error o cierre de conexion, gestionamos peticion.
					{	
						//se construye request con el texto y con el socket listener, para que nos de informacion
						// de a que ip y puerto iba destinado esta peticion
						Request req(text, findListener(_sockVec, findSocket(_pollVec[i].fd, _sockVec, _sockVec.size()), size));
						this->handleClient(req, _pollVec[i].fd, _servVec);
						if (!req.getKeepAlive())
							closeConnection(i, _pollVec, _sockVec, &size, &flag);
					}
				}
			}
		}
	}	
}

int	Cluster::handleClient(Request &request, int new_socket, const std::vector<Server>&servVec)
{
	Response	rsp; // declaramos response
	/*determinamos a que server block y que location block son los responsables de aplicar
	sus diferentes reglas y configuraciones para gestionar dicha peticion*/
	const Server *serv = find_serv_block(servVec, request);
	const Location *loc = find_loc_block(serv, request);
	if (!request.good)   // comprobamos si ha habido algun fallo en el parseo para devolver error 400
		setResponse(400, rsp, "", serv, NULL);
	else
		/*iniciamos el flow para gestionar la peticion*/
		handleRequest(request, rsp, serv, loc);
	std::string response = rsp.makeResponse(); // hacemos respuesta con los valores del clase Response
	send(new_socket, response.c_str(), response.length(), 0);
	
	return (0);
}

void	Cluster::handleRequest(Request &request, Response &response, const Server *serv, const Location *loc)
{
	cout << "entra en handle request:  ";
	/*comprobamos el path del request y realizamos comprobaciones pertinentes*/
	std::string path = getPath(request, serv, loc); // tambien parseamos una posible question query, para conducir a archivo cgi de manera correcta
	cout << "con path: " << path << " con method " << request.getMethod() << endl;
	if (path == "none") // no hay root directives, solo daremos una pagina de webserv si se accede al '/', si no 404
	{
		if (!check_method(request.getMethod(), NULL, serv)) // bloqueamos toda peticion que no sea GET, 405
		{
			response.setResponse(405, "", serv, loc);
			return ;
		}
		/*Si la peticion es al root, damos pagina default, si no 404*/
		if (request.getTarget() == "/")
			response.setResponse(200, readFileContents("default/default.html"), NULL, NULL);
		else
			response.setResponse(404, "", serv, NULL);
	}
	else
	{
		if (!check_method(request.getMethod(), loc, serv)) // si metodo no permitido, 405
		{
			response.setResponse(405, "", serv, loc);
			return ;
		}
		if (loc && loc->getRedirection().length() > 0) // comprobar si hay directive return
		{
			response.setResponse(loc->getRedirectionNumber(), loc->getRedirection(), NULL, NULL); // pasaremos setResponse cuando tengamos map<int,string>
			return ;
		}
		if (!checkGood(path))  // si el path que ha resultado no existe, comprobamos si es un put y se puede acceder a la carpeta previa
		{
			cout << "tras primer checkgood" << endl;
			if (request.getMethod() == "PUT")
			{
				if (checkPutFile(path))
				{
					setPut(response, request, path, request.getMethod());
					return ;
				}
			}
			else
			{
				response.setResponse(404, "", serv, NULL);
				return ;
			}
		}
		if (request.getMethod() == "GET" &&  checkFileOrDir(path) == "dir" && !checkTrailingSlash(path))  // comprobamos si tiene o no trailing slash, nginx hace una redireccion 301 a URL con final slash
		{
			response.setResponse(301, request.getTarget() + "/", NULL, NULL);
			return ;
		}
		if (request.getMethod() == "DELETE")
		{
			setDel(response, request, path, request.getMethod());
			return ;
		}
		if (request.getMethod() == "PUT")
		{	
			if (path[path.length() - 1] != '/') 
			{
				setPut(response, request, path, request.getMethod());
				return ;
			}
			else
			{
				response.setResponse(409, "", NULL, NULL);
				return ;
			}
		}
		if (checkFileOrDir(path) == "file")
		{
			if (checkCgi(request, path, loc)) // chequearemos si location tiene activado el cgi y para que extensiones
			{
				cgi(response, request, path, request.getMethod());
				return ;
			}
			else
				response.setResponse(200, readFileContents(path), NULL, NULL); //sino servimos el recurso de manera normal
		}
		else // si corresponde a un directorio, primero miramos que no haya un index file
		{
			if (serv->getVIndex().size() > 0 || (loc && loc->getIndex().size() > 0)) // si hay index directive
			{
				std::string index_file = findIndex(path, serv, loc); // localizamos si el camino lleva a un archivo
				if (index_file != "")  // en caso afirmativo, se sirve
				{
					response.setResponse(200, readFileContents(index_file), NULL, NULL);
					return ;
				}
				else // si no, damos un forbidden, como nginx
				{
					response.setResponse(403, "", serv, NULL);
					return ;
				}
			}
			if (findIndexHtml(path)) // sino, buscamos un archivo index.html para servir.
			{
				path += "index.html";
				response.setResponse(200, readFileContents(path), NULL, NULL);
				return ;
			}
			else // sino, comprobamos si tiene autoindex activado para mostrar listado directorio
			{
				if (!loc || !loc->getAutoindex()) // si no tiene autoindex (como solo lo puede tener un location, de momento), devolvemos 403 ya que no esta activado el directorylisting
											// y no tenemos permiso para coger ningun archivo de directorio
				{
					response.setResponse(403, "", serv, NULL);
					return ;
				}
				else
				{
					std::string content = generateDirectoryListing(path);
					if (content == "Error 500") // por si da algun error interno el server.
						response.setResponse(200,  "", NULL, NULL);
					else
						response.setResponse(200, content, NULL, NULL);
					return ;
				}
			}		
		}
	}
}

void	Cluster::closeConnection(int i, std::vector<pollfd>&_pollVec,
								std::vector<Socket>&_sockVec, unsigned int *size, int *flag)
{
	close(_pollVec[i].fd);
						//	cout << "connexion with fd: " << pollVec[i].fd << " was closed with client. socket config is: " << endl;
	remove_pollfd(_pollVec, _sockVec, _pollVec[i].fd, *size);
	(*size)--; // si se quita un elemento del vector, reducimos size para buen funcionamiento
	*flag = 0;	
}

void Cluster::clean()
{

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
		cout << "Socket " << i + 1 <<  " with fd " <<_sockVec[i].getFd()  << ". Is listener? " << _sockVec[i].listener << endl
		<< "IP: " << _sockVec[i].getIp() << " Port: " << _sockVec[i].getPort() << endl;  
	}

    std::cout << "Poll Vector:" << std::endl;
	for (unsigned int i = 0; i < _pollVec.size(); i++)
	{
		std::cout << i << " socket fd: " << _pollVec[i].fd << std::endl;
	}
} 

