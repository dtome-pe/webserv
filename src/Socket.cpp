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
	_ip = "";
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
		cout << _ip << endl;
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
	   throw std::runtime_error("error creating socket");
	if (bind_s(s_addr) == 1)
		throw std::runtime_error("error executing the bind.");
	if (listen_s() == 1)
		throw std::runtime_error("error setting up the socket to listen for incoming connections.");
	freeaddrinfo(s_addr);
}

int Socket::get_addr_info(struct addrinfo **s_addr, const char *host, const char *port)
{
	int status;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
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
	if (bind(_fd, (const sockaddr *)addr, s_addr->ai_addrlen) < 0)
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

	if (cgi && type == 0)
		return (doneNothing(*this, _textRead));

	if (!_request->getCgi())
	{
		parseRequestTillBody(*this, _textRead);
		if (getRequest()->getWaitingForBody())
		{
			if (_request->getHeader("Content-Length") == "not found" && _request->getHeader("Transfer-Encoding") == "not found")
				return (doneNoBody(*this));
			else if (_request->getHeader("Content-Length") != "not found" && contentLengthRequestDone(*this, _request, _textRead) == DONE)
				return (DONE);
			else if (_request->getHeader("Transfer-Encoding") == "chunked" && _request->parseChunked(_textRead) == DONE)
				return (DONE);
		}
		return (NOT_DONE);
	}
	else
	{
		cout << "entramos en parse cgi" << endl;
		if (!getResponse())
			setResponse(new Response());
		int ret = parseCgiOutputTillBody(*this, _textRead, text, &flag);
		if (ret == DONE_ERROR)
			return (DONE_ERROR);
		if (getResponse()->waitingForBody && _response->getCgiHeader("Content-Length") != "not found" && contentLengthCgiOutputDone(*this, _response, _textRead, &flag) == DONE)
			return (DONE);
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
	{	
		cout << "error fcntl" << endl;
		return (1);
	}
	return (0);
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
