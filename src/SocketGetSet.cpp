#include <webserv.hpp>

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

std::string &Socket::getTextRead()
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

pollfd		*Socket::getPoll()
{
	return (_poll);
}

pollfd		*Socket::getCgiPoll()
{
	return (_cgiPoll);
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

void	Socket::setPoll(pollfd *node)
{
	_poll = node;
}

void	Socket::setCgiPoll(pollfd *node)
{
	_cgiPoll = node;
}