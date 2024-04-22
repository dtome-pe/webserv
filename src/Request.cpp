#include <webserv.hpp>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include <lib.hpp>
#include <map>

Request::Request(Cluster &cluster, const std::vector<class Server> &server, Socket &listener, Socket &client) : cluster(cluster), client(client), listener(listener), server(server)
{
	good = true;
	cgi = false;
	keepAlive = true;
	trailSlashRedir = false;
	uploadStore = "";
	request_line = "";
	waitingForBody = false;

	if (client.getContinue() || client.getCgi())   
	{
		//cout << "entra en get continue | cgi" << endl;										
		setRequestLine(client.getPreviousRequestLine());
		setHeaders(client.getPreviousHeaders());
		if (client.getCgi())
			headers.removeHeader("Content-Length");
		if (client.getContinue())
			client.setContinue(false);
		if (client.getCgi())
		{
			client.setCgi(false);
			setCgi(true);
		}
		client.setPreviousRequestLine("");
		client.getPreviousHeaders().clear();
	}
}

void Request::otherInit()
{
	setIpPortHost(listener);
	if (getHeader("Connection") == "close") // cambiamos keepAlive si explicitamente se solicita la finalizacion de la conexion
		keepAlive = false;
	/*determinamos block server y location relevantes para request*/
	setServer(server);
	setLocation(getServer());
	if (loc && loc->getUploadStore() != "")   // seteamos uploadStore si toca
		setUploadStore(loc->getUploadStore());
	client.setTextRead(""); // limpiamos buffer
	client.setReadAll(false);
}

Request::~Request()
{

}

void	Request::parseRequest(std::string text)
{
	if (request_line == "")
		setRequestLine(text);
	else
	{
		if (text == "\r\n")
			setWaitingForBody(true);
		else
			setHeader(text.substr(0, text.length() - 2));
	}
}

int	Request::parseChunked(std::string &textRead)
{
	static uint64_t		actualChunkSize = 0;
	static std::string	body = "";
	unsigned int		i = 0;

	while (i < textRead.size())
	{
		if (!actualChunkSize)
		{
			if (textRead[i] == ';' || (textRead[i] == '\r' && textRead.length() > i + 1 && textRead[i + 1] == '\n'))
			{
				actualChunkSize = hexStringToDecimalUint(textRead.substr(0, i));
				if (actualChunkSize == 0)
				{
					setBody(body);
					setHeader("Content-Length: " + int_to_str(body.length()));
					headers.removeHeader("Transfer-Encoding");
					textRead = "";
					body = "";
					return (DONE);
				}
				if (textRead[i] == ';')
					textRead = textRead.substr(i + 1, textRead.length());
				else
					textRead = textRead.substr(i + 2, textRead.length());
				i = 0;
				continue ;
			}
		}
		else
		{
			if (textRead.size() >= actualChunkSize)
			{
				body.append(textRead.substr(0, actualChunkSize));
				textRead = textRead.substr(actualChunkSize, textRead.length());
				actualChunkSize = 0;
			}
		}
		i++;
	}
	return (NOT_DONE);
}

void	Request::setRequestLine(std::string reqLine)
{
	this->request_line = reqLine;
	std::vector<std::string> split = HeaderHTTP::split(reqLine, " ");

	if (split.size() != 3)
		good = false;

	this->method = split[0];
	this->target = split[1];
	this->version = split[2];
}

static const Server *ip_port(const std::vector<class Server> &serv, Request &request)
{
	const Server *	ret = NULL;

	for(size_t i = 0; i < serv.size(); i++)  
	{	
		for (size_t j = 0; j < serv[i].ip_port.size(); j++)
		{
			if ((request.getIp() == serv[i].getIp(serv[i].ip_port[j]) ||
					serv[i].getIp(serv[i].ip_port[j]) == "") && request.getPort() == serv[i].getPort(serv[i].ip_port[j]))
			{
				if (!ret)
				{
					ret = &serv[i];
					break ;
				}
				if (ret)
					return (NULL);
			}
		}
	}
	return (ret);
}

static const Server *serverName(const std::vector<class Server> &serv, Request &request)
{
	const Server *ret;

	for(size_t i = 0; i < serv.size(); i++)  
	{	
		const std::vector<std::string> &serv_name = serv[i].getVServerName();
		for (size_t j = 0; j < serv_name.size(); j++)
		{
			if (serv_name[j] == request.getHost())
			{	
				ret = &serv[i];
				return (ret);
			}
		}
	}
	return (NULL);
}

static const Server *getFirstBlock(const std::vector<class Server> &serv, Request &request)
{
	for(size_t i = 0; i < serv.size(); i++)  
	{	
		for (size_t j = 0; j < serv[i].ip_port.size(); j++)
		{
			if ((request.getIp() == serv[i].getIp(serv[i].ip_port[j]) ||
					serv[i].getIp(serv[i].ip_port[j]) == "") && request.getPort() == serv[i].getPort(serv[i].ip_port[j]))
			{	
				return (&serv[i]);
			}
		}
	}
	return (NULL);
}


void Request::setServer(const std::vector<class Server> &server)
{	
	const Server *block;

	block = ip_port(server, *this);
	if (!block)
	{
		block = serverName(server, *this);
		if (!block)
		{
			serv = getFirstBlock(server, *this);
			return ;
		}
	}
	serv = block;
}

static std::string decode(const std::string& encoded) // decodifica caracteres escapados con hexadecimales en URL encoding
{  
	std::ostringstream decoded;

    for (std::size_t i = 0; i < encoded.length(); ++i) 
	{
        if (encoded[i] == '%' && i + 2 < encoded.length()) 
		{
            char hex1 = encoded[i + 1];
            char hex2 = encoded[i + 2];

            std::istringstream hexStream(std::string("0x") + hex1 + hex2);
            int decodedChar;
            hexStream >> std::hex >> decodedChar;

            decoded << static_cast<char>(decodedChar);

            i += 2;
        } 
		else 
            decoded << encoded[i];
    }
    return decoded.str();
}


void Request::setLocation(const Server *serv)
{
	const Location *ret = NULL;

	std::string decoded_str = decode(getTarget());
	
	const std::vector<Location>&locations = serv->getLocations();
	
	std::vector<const Location*>matches;

	for (unsigned int i = 0; i < locations.size(); i++)
	{
		int len = locations[i].getLocation().length();
		if (!getTarget().compare(0, len, locations[i].getLocation()))
		{
			matches.push_back(&locations[i]);
		}
	}
	for (unsigned int i = 0; i < matches.size(); i++)
	{
		if (!ret)
			ret = matches[i];
		else
		{
			if (matches[i]->getLocation().length() > ret->getLocation().length())
				ret = matches[i];
		}
	}		
	loc = ret;
}

std::string Request::makeRequest()
{
	return (this->request_line + this->headers.makeHeader()
			+ "\r\n" + this->body);
}

std::string Request::getLocationDir()
{
	return (getHeader("Host") + uploadStore.substr(uploadStore.find_last_of('/'), uploadStore.length())
			+ target.substr(target.find_last_of('/'), target.length()));
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
