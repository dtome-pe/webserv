#include <webserv.hpp>

Response::Response(Request &request, const Server *serv, const Locations *loc)
{	
	(void) request;
	(void) serv;
	(void) loc;
	
	std::cout << "response: " <<  request.getMethod() << std::endl;
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
	return (this->status_line.line + this->headers.makeHeader()
			+ "\r\n" + this->body);
}

void	Response::setStatusLine(std::string _status_line)
{
	this->status_line.line = _status_line + "\r\n";
	std::vector<std::string> split = HeaderHTTP::split(_status_line, " ");
//	for (size_t i = 0; i < split.size(); i++)
//		std::cout << split[i] << std::endl;

	this->status_line.protocol = split[0];
	this->status_line.code = split[1];
	this->status_line.text = split[2];

/* 	std::cout << "Response line elements:" << std::endl;
	std::cout << this->status_line.protocl << std::endl;
	std::cout << this->status_line.code << std::endl;
	std::cout << this->status_line.text << std::endl; */
}

void	Response::setHeader(std::string _header)
{
	this->headers.setHeader(_header);
}

void	Response::setBody(std::string _body)
{
	this->body = _body;
}
