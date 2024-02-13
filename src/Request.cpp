#include <webserv.hpp>

Request::Request(std::string buff)
{
	//parse_Request(); //parsear antes de esto
	int start = 0;
	int finish = buff.find("\r");
	this->status_line = buff.substr(start, finish + 2);
	buff = buff.substr(finish + 2, buff.length());
	while (buff != "\r\n")
	{
		try
		{
			finish = buff.find("\r");
			this->headers.setHeader(buff.substr(start, finish));
			buff = buff.substr(finish + 2, buff.length());
		}
		catch(const std::exception& e)
		{}
	}
	try
	{
		this->body = buff.substr(finish + 2, buff.length());
	}
	catch(const std::exception& e)
	{}
	std::cout << this->makeRequest() << std::endl;
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

