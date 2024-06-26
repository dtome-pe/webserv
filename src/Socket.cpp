#include <webserv.hpp>

Socket::Socket(std::string host_port, Server *s_ptr, int serverId)
{
	_cgiFd = -1;
	_cgi = false;
	_continue = false;
	_textRead = "";
	_readAll = false;
	_request = NULL;
	_response = NULL;
	_ip = "";
	s_addr = NULL;
	_timeout = false;
	if (s_ptr)
	{
		_ip = host_port.substr(0, host_port.find(":"));
		_port = host_port.substr(host_port.find(":") + 1, host_port.length());
		if (this->get_addr_info(&s_addr, _port.c_str()) == 1)
		   throw std::runtime_error("Error obtaining address information for the specified host and port.");							
		serv = s_ptr;
		serv->setIpPort(_ip, _port);
		listener = 1;
		_serverId = serverId;
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
	s_addr = NULL;
}

int Socket::get_addr_info(struct addrinfo **s_addr, const char *port)
{
	int status;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	status = getaddrinfo(NULL, port, &hints, s_addr);
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
		cout << strerror(errno) << endl;
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
