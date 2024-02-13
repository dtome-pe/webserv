#include <webserv.hpp>
#include <string.h>

void	print_str(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == '\r')
		{
			std::cout << "\\r";
			continue ;
		}
		if (str[i] == '\n')
			std::cout << "\\n";
		std::cout << str[i];
	}
		std::cout << std::endl;
}

Request::Request(std::string buff)
{
	splitRequest(buff);
}

Request::~Request()
{
}

void	Request::splitRequest(std::string buff)
{
	//parse_Request(); //parsear antes de esto
	//int start = 0;
	int rec = 0;
	int finish = buff.find("\n");
	if (buff[finish - 1] == '\r')
		rec = 1;
	setStatusLine(buff.substr(0, finish - rec));
	//print_str(this->status_line);
	buff = buff.substr(finish + 1, buff.length());
	while (buff != "\r\n")
	{
		try
		{
			rec = 0;
			finish = buff.find("\n");
			if (buff[finish - 1] == '\r')
				rec = 1;
			this->headers.setHeader(buff.substr(0, finish - rec));
			buff = buff.substr(finish + 1, buff.length());
		}
		catch(const std::exception& e)
		{}
	}
	try
	{
		this->body = buff.substr(finish + 1, buff.length());
	}
	catch(const std::exception& e)
	{}
	print_str(this->makeRequest());
}

std::string Request::makeRequest()
{
	return (this->status_line + this->headers.makeHeader()
			+ "\r\n" + this->body);
}

void	Request::setStatusLine(std::string _status_line)
{
	this->status_line = _status_line + "\r\n";
	//char *s = 
	//std::cout << strtok(_status_line.c_str(), " /") << std::endl;
	//std::cout << strtok(NULL, " /") << std::endl;
	//for (size_t i = 0; s[i]; i++)
	//	std::cout << s[i] << std::endl;
}

void	Request::setHeader(std::string _header)
{
	this->headers.setHeader(_header);
}

void	Request::setBody(std::string _body)
{
	this->body = _body;
}

