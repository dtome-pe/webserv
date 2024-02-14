#include <webserv.hpp>

Response::Response(Request &request)
{	
	(void) request;
	
	this->setStatusLine("HTTP/1.1 200 OK");
	this->setHeader("Server: apache");
	this->setBody("Hi");
	this->setHeader("Content-Length: 2");
}

Response::~Response()
{
}

std::string Response::makeResponse()
{
	return (this->status_line + this->headers.makeHeader()
			+ "\r\n" + this->body);
}

void	Response::setStatusLine(std::string _status_line)
{
	this->status_line = _status_line + "\r\n";
}

void	Response::setHeader(std::string _header)
{
	this->headers.setHeader(_header);
}

void	Response::setBody(std::string _body)
{
	this->body = _body;
}
