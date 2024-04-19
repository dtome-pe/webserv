#include <webserv.hpp>

Socket::Socket(std::string host_port, Server *s_ptr)
{
	_cgiFd = -1;
	_cgi = false;
	_continue = false;
	_textRead = "";
	_readAll = false;
	_request = NULL;
	_response = NULL;
	if (s_ptr) // listener
	{
		/*trocemos host y port para meterlas en funcion get_addr_info*/
		_host = host_port.substr(0, host_port.find(":"));
		_port = host_port.substr(host_port.find(":") + 1, host_port.length());

		if (this->get_addr_info(&s_addr, _host.c_str(), _port.c_str()) == 1)
		   throw std::runtime_error("Error obtaining address information for the specified host and port.");	// obtenemos datos y se resuelve dominio y se introduce ip y puerto.
		struct sockaddr_in *addr = (sockaddr_in *)s_addr->ai_addr;	// se castea para poder obtener ip
		_ip = ip_to_str(addr);										// volcamos ip de network byte order a string, para luego chequear sockets duplicados
							   // y posteriormente pasar informacion a server correspondiente
		serv = s_ptr; // apuntamos a server correspondiente
		/*en este momento le seteamos en el vector ip_port al server que ha generado este socket, la ip
		ya resuelta y puerto, para en find_serv_block poder buscar que server la toca gestionar
		la conexion recibida*/
		serv->setIpPort(_ip, _port);
		listener = 1;
	}
	else
	{
		_host = host_port.substr(0, host_port.find(":"));
		listener = 0;
		_continue = false;
	}
}

void Socket::start()
{
	if (create_s(s_addr, sock_addr, sock_addrlen) == 1)
	   throw std::runtime_error("error creating socket");	// creamos el fd del socket
	if (bind_s(s_addr) == 1)
		throw std::runtime_error("error executing the bind.");		// bindeamos
	if (listen_s() == 1)
		throw std::runtime_error("error setting up the socket to listen for incoming connections.");
	freeaddrinfo(s_addr);
}

int Socket::get_addr_info(struct addrinfo **s_addr, const char *host, const char *port)
{
	int status;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints)); // damos valor NULL a todo para luego inicializar
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	status = getaddrinfo(host, port, &hints, s_addr);
	if (status != 0)
		return (1);
	return (0);
}

int Socket::create_s(struct addrinfo *s_addr, struct sockaddr_in sock_addr, socklen_t sock_addrlen)
{
	_fd = socket(s_addr->ai_family, s_addr->ai_socktype, s_addr->ai_protocol);
	if (_fd < 0)
	{
		close(_fd);
		return (1);
	}
	if (fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
	{
		close(_fd);
		return (1);
	}
	sock_addrlen = sizeof(sock_addr);
	if (getsockname(_fd, (struct sockaddr *)&sock_addr, &sock_addrlen) == -1)
		return (1);
	return (0);
}

int Socket::bind_s(struct addrinfo *s_addr)
{
	int yes = 1;

	struct sockaddr_in *addr = (sockaddr_in *)s_addr->ai_addr;

	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		return (1);
	}
	if (bind(_fd, (const sockaddr *)addr, s_addr->ai_addrlen) < 0) // hacemos bind
	{
		close(_fd);
		return (1);
	}
	return (0);
}

int Socket::listen_s()
{
	if (listen(_fd, 10) < 0)
	{
		close(_fd);
		return (1);
	}
	return (0);
}

int	Socket::addToClient(std::string text, bool cgi, int type)
{
	appendTextRead(text);

	static int 	flag;
	size_t i = 0;

	if (cgi && type == POLLHUP)
	{
		getResponse()->setBody(_textRead);
		setTextRead("");
		getResponse()->waitingForBody = false;
		return (DONE);
	}
	//cout << "add to client: cgi: " << cgi << endl;
	if (!cgi)
	{
		while (i < _textRead.length())
		{
			if (!getRequest()->getWaitingForBody())
			{
				if (_textRead[i] == '\r' && (i + 1) < _textRead.length() && _textRead[i + 1] == '\n')
				{
					getRequest()->parseRequest(_textRead.substr(0, i + 2), cgi);
					_textRead = _textRead.substr(i + 2, _textRead.length());
					i = 0;
					continue ;
				}
			}
			else
				break ;
			i++;
		}
		if (getRequest()->getWaitingForBody())
		{
			if (_request->getHeader("Content-Length") == "not found" && _request->getHeader("Transfer-Encoding") == "not found")
			{	
				cout << "waiting for body but no body to receive, request done" << endl;
				setTextRead("");
				getRequest()->setWaitingForBody(false);
				return (DONE);
			}
			else if (_request->getHeader("Content-Length") != "not found")
			{
				cout << "content-length: " << str_to_int(_request->getHeader("Content-Length")) << endl << "textRead len: " << _textRead.length() << endl;
				if (_textRead.length() >= str_to_int(_request->getHeader("Content-Length")))
				{
					_request->setBody(_textRead.substr(0, str_to_int(_request->getHeader("Content-Length"))));
					setTextRead("");
					getRequest()->setWaitingForBody(false);
					return (DONE);
				}
			}
			else if (_request->getHeader("Transfer-Encoding") == "chunked")
			{
				if (_request->parseChunked(_textRead) == DONE)
					return (DONE);
			}
		}
		return (NOT_DONE);
	}
	else
	{
		if (!getResponse())
			setResponse(new Response());
		while (i < _textRead.length())
		{
			if (!getResponse()->waitingForBody)
			{
				if (_textRead[i] == '\n')
				{
					if (!flag)
					{
						if (!checkIfHeader(text)) // si la primera linea del output no es ninguno de los tres headers esenciales, damos 500. nos aseguramos de que haya algun header.
						{
							getResponse()->setCode("500");
							return (DONE_ERROR);
						}
						flag = 1;
					}
					getResponse()->parseCgi(_textRead.substr(0, i + 1));
					_textRead = _textRead.substr(i + 1, _textRead.length());
					i = 0;
					continue ;
				}
			}
			else
				break ;
			i++;
		}
		if (getResponse()->waitingForBody)
		{
			cout << "body: " << _textRead << endl;
			if (_response->getCgiHeader("Content-Length") != "not found")
			{
				if (_textRead.length() >= str_to_int(_response->getCgiHeader("Content-Length")))
				{
					_response->setBody(_textRead.substr(0, str_to_int(_response->getCgiHeader("Content-Length"))));
					setTextRead("");
					getResponse()->waitingForBody = false;
					flag = 0;
					return (DONE);
				}
			}
		}
		return (NOT_DONE);
	}
}

void Socket::pointTo(int fd)
{
	pointingTo = fd;
}

int Socket::setNonBlocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
		return (1);
	return (0);
}

std::string Socket::getPort() const
{
	return (_port);
}

std::string Socket::getIp() const
{
	return (_ip);
}

int Socket::getFd() const
{
	return (_fd);
}

bool Socket::getContinue()
{
	return (_continue);
}

std::string Socket::getPreviousRequestLine()
{
	return (_previousRequestLine);
}

HeaderHTTP 	&Socket::getPreviousHeaders()
{
	return (_previousHeaders);
}

bool 		Socket::getCgi()
{
	return (_cgi);
}

int 		Socket::getCgiFd()
{
	return (_cgiFd);
}

std::string Socket::getTextRead()
{
	return (_textRead);
}

bool 		Socket::getReadAll()
{
	return (_readAll);
}

Request		*Socket::getRequest()
{
	return (_request);
}

Response	*Socket::getResponse()
{
	return (_response);
}


void Socket::setHost(std::string host)
{
	_host = host;
}

void Socket::setPort(std::string port)
{
	_port = port;
}

void Socket::setIp(std::string ip)
{
	_ip = ip;
}

void Socket::setFd(int fd)
{
	_fd = fd;
}

void Socket::setContinue(bool c)
{
	_continue = c;
}

void Socket::setPreviousRequestLine(std::string requestLine)
{
	_previousRequestLine = requestLine;
}

void Socket::setPreviousHeaders(HeaderHTTP headers)
{
	_previousHeaders = headers;
}

void Socket::setCgi(bool cgi)
{
	_cgi = cgi;
}

void Socket::setCgiFd(int fd)
{
	_cgiFd = fd;
}

void Socket::setReadAll(bool readAll)
{
	_readAll = readAll;
}

void Socket::appendTextRead(std::string text)
{
	_textRead += text;
}

void Socket::setTextRead(std::string text)
{
	_textRead = text;
}

void	Socket::setRequest(Request *request)
{
	_request = request;
}

void	Socket::setResponse(Response *response)
{
	_response = response;
}

void	Socket::setBodyType(int type)
{
	_bodyType = type;
}

void Socket::bouncePrevious(Request &request, int type)
{
	if (type == CONTINUE)
		setContinue(true);
	else if (type == CGI)
		setCgi(true);
	setPreviousRequestLine(request.getRequestLine());
	setPreviousHeaders(request.getHeaders());
	_previousHeaders.removeHeader("Expect");
}
