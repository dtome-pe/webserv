#include <webserv.hpp>

Request::Request(std::string buff)
{
	//status_line = "HTTP/1.1 200 OK\r\n";
	//headers.setHeader("Content-Length: 2");//\r\n");
	//body = "Hi";

	//parse_Request(); //parsear antes de esto
	int start = 0;
	int finish = buff.find("\n");
	this->body = buff.substr(start, finish + 1);
	buff = buff.substr(finish + 1, buff.length());
	//std::cout << "body = <" << body << ">";// << std::endl;
	std::cout << this->body;// << std::endl;
	while (buff == "\r\n")
	{
		start = finish;
		finish = buff.find("\n");
		this->headers.setHeader(buff.substr(start, finish));
		buff = buff.substr(finish + 1, buff.length());
		std::cout << this->headers.header.back();// << std::endl;
	}
}

Request::~Request()
{
}

std::string Request::makeRequest()
{
	return (this->status_line + this->headers.makeHeader()
			+ "\r\n" + this->body);
}

void	Request::setStatusLine(std::string _status_line)
{
	this->status_line = _status_line + "\r\n";
}

void	Request::setHeader(std::string _header)
{
	this->headers.setHeader(_header);
}

void	Request::setBody(std::string _body)
{
	this->body = _body;
}

