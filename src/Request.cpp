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
//	print_str(this->request_line.line);
	buff = buff.substr(finish + 1, buff.length());
	while (buff != "\r\n")
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
	if (this->headers.header_map.find("Host") == this->headers.header_map.end())
		good = false;
	setIpPortHost(listener);
}

std::string Request::makeRequest()
{
	return (this->request_line.line + this->headers.makeHeader()
			+ "\r\n" + this->body);
}

void	Request::setRequestLine(std::string _status_line)
{
	this->request_line.line = _status_line + "\r\n";
	std::vector<std::string> split = HeaderHTTP::split(_status_line, " ");

	if (split.size() != 3)
		good = false;

	this->request_line.method = split[0];
	this->request_line.target = split[1];
	this->request_line.version = split[2];

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

	//std::cout << "client ip: " << ip << "server port: " << port << "host header: " << host << std::endl;
}

std::string Request::getMethod()
{
	return (request_line.method);
}

std::string Request::getTarget()
{
	return (request_line.target);
}

std::string Request::getVersion()
{
	return (request_line.version);
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
