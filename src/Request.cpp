#include <webserv.hpp>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include <lib.hpp>
#include <map>

Request::Request(std::string buff, Socket &listener)
{
	good = true;
	splitRequest(buff, listener);
}

Request::~Request()
{
}

void	Request::splitRequest(std::string buff, Socket &listener)
{
	int rec = 0;
	int finish = buff.find("\n");
	if (buff[finish - 1] == '\r')
		rec = 1;
	setRequestLine(buff.substr(0, finish - rec));
	buff = buff.substr(finish + 1, buff.length());
	while (buff.substr(0, 2) != "\r\n")
	{
		try
		{
			rec = 0;
			finish = buff.find("\n");
			if (buff[finish - 1] == '\r')
				rec = 1;
			if (this->headers.setHeader(buff.substr(0, finish - rec)) == 1)
				good = false;
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
	{	}
	if (this->headers.map.find("Host") == this->headers.map.end())
		good = false;
	setIpPortHost(listener);
}


void	Request::setRequestLine(std::string reqLine)
{
	this->request_line = reqLine + "\r\n";
	std::vector<std::string> split = HeaderHTTP::split(reqLine, " ");

	if (split.size() != 3)
		good = false;

	this->method = split[0];
	this->target = split[1];
	this->version = split[2];

}

void	Request::setHeader(std::string _header)
{
	this->headers.setHeader(_header);
}

void	Request::setBody(std::string _body)
{
	this->body = _body;
}

void	Request::setIpPortHost(Socket &listener)
{
	ip = listener.getIp();
	port = listener.getPort();
}

void	Request::setCgiExtension(std::string &extension)
{
	cgiExtension = extension;
}

void	Request::setCgiBinary(std::string &binary)
{
	cgiBinary = binary;
}

std::string Request::getMethod()
{
	return (method);
}

std::string Request::getTarget()
{
	return (target);
}

std::string Request::getVersion()
{
	return (version);
}

std::string Request::getRequestLine()
{
	return (request_line);
}

std::string	Request::getHeader(std::string header)
{
	return (headers.getHeader(header));
}

HeaderHTTP	Request::getHeaders()
{
	return (headers);
}

std::string Request::getBody()
{
	return (body);
}

std::string Request::getCgiExtension()
{
	return (cgiExtension);
}

std::string Request::getCgiBinary()
{
	return (cgiBinary);
}

std::string Request::makeRequest()
{
	return (this->request_line + this->headers.makeHeader()
			+ "\r\n" + this->body);
}

void	Request::printRequest()
{	
	std::string str = this->makeRequest();
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
