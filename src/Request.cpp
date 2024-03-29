#include <webserv.hpp>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include <lib.hpp>
#include <map>

Request::Request(std::string buff, const std::vector<class Server> &server, Socket &listener, Socket &client)
{
	good = true; // por defecto, request correcta en parseo
	keepAlive = true; // por defecto
	trailSlashRedir = false;
	uploadStore = "";
	if (client.getContinueBool())   						//si tenemos el continue
	{														// solo volcamos request line y headers guardados en socket de cliente
		setRequestLine(client.getContinueRequestLine());	// que envio el expect y recibio el 100 continue, pero no
		setHeaders(client.getContinueHeaders());			// el header de expect
		client.setContinueBool(false);
		client.setContinueRequestLine("");
		client.getContinueHeaders().clear();
		setBody(buff);
	}
	else
		splitRequest(buff); // sino es continue se parsea de manera normal
	setIpPortHost(listener);
	if (getHeader("Connection") == "close") // cambiamos keepAlive si explicitamente se solicita la finalizacion de la conexion
		keepAlive = false;
	/*determinamos block server y location relevantes para request*/
	setServer(server);
	setLocation(getServer());
	if (loc && loc->getUploadStore() != "")   // seteamos uploadStore si toca
		setUploadStore(loc->getUploadStore());
}

Request::~Request()
{
}

void	Request::splitRequest(std::string buff)
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
		this->body = buff.substr(2, buff.length()); // nos quedamos el cuerpo sin el /r/n previo.
	}
	catch(const std::exception& e)
	{	}
	if (this->headers.map.find("Host") == this->headers.map.end())
		good = false;
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
			if (serv_name[j] == request.getHost()) // comparamos con lo que hemos cogido del Host header para determinar
											// por server_name
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
	// a cada server
	for(size_t i = 0; i < serv.size(); i++)  
	{	
		for (size_t j = 0; j < serv[i].ip_port.size(); j++)
		{	
			//si es el primer match, lo retornamos y se acabo
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
	
	//cout << "request ip: " << request.ip << "request port: " << request.port << endl;

	block = ip_port(server, *this);  //primero buscamos si solo hay un match por direccion y puerto
	if (!block) // si ha devuelto nulo, es que hay mas de un server block con ip:puerto y hay que buscar ahora por server_name
	{
		block = serverName(server, *this);
		if (!block) // si ha devuelto nulo, es que no ha encontrado match con server_name
		{
			serv = getFirstBlock(server, *this); // asi que hay que devolver primer server que encaje con ip:puerto
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
            // Extract the two characters following '%'
            char hex1 = encoded[i + 1];
            char hex2 = encoded[i + 2];

            // Convert hex characters to an integer
            std::istringstream hexStream(std::string("0x") + hex1 + hex2);
            int decodedChar;
            hexStream >> std::hex >> decodedChar;

            // Append the decoded character to the result
            decoded << static_cast<char>(decodedChar);

            // Move the index two characters forward
            i += 2;
        } 
		else 
		{
            // Append non-% characters directly to the result
            decoded << encoded[i];
        }
    }
    return decoded.str();
}


void Request::setLocation(const Server *serv)
{
	const Location *ret = NULL;

	std::string decoded_str = decode(getTarget()); // decodificamos posibles %xx de URI
	
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

void Request::setHost(std::string &host)
{
	this->host = host;
}

void Request::setIp(std::string &ip)
{
	this->ip = ip;
}

void Request::setPort(std::string &port)
{
	this->port = port;
}

void	Request::setTrailSlashRedir(bool redir)
{
	trailSlashRedir = redir;
}

void	Request::setPath(std::string path)
{
	this->path = path;
	if (path.find_last_of(".") != std::string::npos)
		setExtension(path.substr(path.find(".") + 1, path.length()));
}

void	Request::setExtension(std::string extension)
{
	this->extension = extension;
}

void	Request::setHeaders(HeaderHTTP headers)
{
	this->headers = headers;
}

void	Request::setUploadStore(std::string path)
{
	this->uploadStore = path;
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

std::string Request::getHost()
{
	return (host);
}

std::string Request::getIp()
{
	return (ip);
}

std::string Request::getPort()
{
	return (port);
}

bool		Request::getKeepAlive()
{
	return (keepAlive);
}

const Server		*Request::getServer()
{
	return(serv);
}

const Location		*Request::getLocation()
{
	return(loc);
}

bool				Request::getTrailSlashRedir()
{
	return(trailSlashRedir);
}

std::string			Request::getPath()
{
	return (path);
}

std::string			Request::getExtension()
{
	return (extension);
}

std::string	Request::getUploadStore()
{
	return (uploadStore);
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
