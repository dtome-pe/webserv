#include <webserv.hpp>

Response::Response(Request &request)
{	
	(void) request;
	
	/*if (request.method == "GET")
		this->do_get();
	else if (request.method == "POST")
		this->do_post();
	else if (request.method == "DELETE")
		this->do_delete();	*/
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
	return (this->status_line.whole_line + this->headers.makeHeader()
			+ "\r\n" + this->body);
}

void	Response::setStatusLine(std::string _status_line)
{
	this->status_line.whole_line = _status_line + "\r\n";
}

void	Response::setHeader(std::string _header)
{
	this->headers.setHeader(_header);
}

void	Response::setBody(std::string _body)
{
	this->body = _body;
}
