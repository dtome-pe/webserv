#include <webserv.hpp>

Socket::Socket(std::string host_port, Server *s_ptr)
{
	_cgiFd = -1;
	_cgi = false;
	_textRead = "";
	_readAll = false;
	if (s_ptr) // listener
	{
		/*trocemos host y port para meterlas en funcion get_addr_info*/
		_host = host_port.substr(0, host_port.find(":"));
		_port = host_port.substr(host_port.find(":") + 1, host_port.length());

		this->get_addr_info(&s_addr, _host.c_str(), _port.c_str()); // obtenemos datos y se resuelve dominio y se introduce ip y puerto.
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
		_continue = false;
	}
}

void Socket::start()
{
	create_s(s_addr, sock_addr, sock_addrlen); // creamos el fd del socket
	bind_s(s_addr);							   // bindeamos
	listen_s();
	freeaddrinfo(s_addr);
}

void Socket::get_addr_info(struct addrinfo **s_addr, const char *host, const char *port)
{
	int status;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints)); // damos valor NULL a todo para luego inicializar
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	status = getaddrinfo(host, port, &hints, s_addr);
	if (status != 0)
	{
		print_error(gai_strerror(status));
		exit(1);
	}
}

void Socket::create_s(struct addrinfo *s_addr, struct sockaddr_in sock_addr, socklen_t sock_addrlen)
{
	_fd = socket(s_addr->ai_family, s_addr->ai_socktype, s_addr->ai_protocol);
	if (_fd < 0)
	{
		print_error(strerror(errno));
		close(_fd);
		exit(EXIT_FAILURE);
	}
	if (fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
	{
		print_error(strerror(errno));
		close(_fd);
		exit(EXIT_FAILURE);
	}
	sock_addrlen = sizeof(sock_addr);
	if (getsockname(_fd, (struct sockaddr *)&sock_addr, &sock_addrlen) == -1)
	{
		print_error("error getting sock name");
		exit(1);
	}
}

void Socket::bind_s(struct addrinfo *s_addr)
{
	int yes = 1;

	struct sockaddr_in *addr = (sockaddr_in *)s_addr->ai_addr;

	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		print_error(strerror(errno));
		exit(1);
	}
	if (bind(_fd, (const sockaddr *)addr, s_addr->ai_addrlen) < 0) // hacemos bind
	{
		print_error(strerror(errno));
		close(_fd);
		exit(EXIT_FAILURE);
	}
}

void Socket::listen_s()
{
	if (listen(_fd, 10) < 0)
	{
		print_error(strerror(errno));
		close(_fd);
		exit(EXIT_FAILURE);
	}
}

void Socket::pointTo(int fd)
{
	pointingTo = fd;
}

void Socket::setNonBlocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
	{
		print_error(strerror(errno));
		close(fd);
		exit(EXIT_FAILURE);
	}
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

HeaderHTTP &Socket::getPreviousHeaders()
{
	return (_previousHeaders);
}

bool Socket::getCgi()
{
	return (_cgi);
}

int Socket::getCgiFd()
{
	return (_cgiFd);
}

std::string Socket::getTextRead()
{
	return (_textRead);
}

bool Socket::getReadAll()
{
	return (_readAll);
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
