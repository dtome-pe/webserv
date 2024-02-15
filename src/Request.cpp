#include <webserv.hpp>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>



/* 
int main() {
    std::string input = "Hola,este;es.un|ejemplo";
    std::string delimiters = ",;.|";

    std::vector<std::string> tokens = split(input, delimiters);

    // Imprimir los tokens
    for (const auto& t : tokens) {
        std::cout << t << std::endl;
    }

    return 0;
}
 */

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
	//headers.printHeaders();
	//setIpPort();
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
	setRequestLine(buff.substr(0, finish - rec));
	print_str(this->request_line.line);
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
	//print_str(this->makeRequest());
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
//	for (size_t i = 0; i < split.size(); i++)
//		std::cout << split[i] << std::endl;
	this->request_line.method = split[0];
	this->request_line.target = split[1];
	this->request_line.version = split[2];
/* 	std::cout << "Request line elements:" << std::endl;
	std::cout << "ins = " << this->request_line.method << std::endl;
	std::cout << "model = " << this->request_line.protocl << std::endl; */
}

void	Request::setHeader(std::string _header)
{
	this->headers.setHeader(_header);
}

void	Request::setBody(std::string _body)
{
	this->body = _body;
}

void	Request::setIpPort()
{
	std::cout << headers.getHeader("Host") << std::endl;
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
