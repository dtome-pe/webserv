#include <webserv.hpp>

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
		setExtension(path.substr(path.find_last_of(".") + 1, path.length()));
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

void	Request::setCgi(bool cgi)
{
	this->cgi = cgi;
}

void	Request::setCgiOutput(std::string cgiOutput)
{
	this->cgiOutput = cgiOutput;
}

void	Request::setWaitingForBody(bool waiting)
{
		waitingForBody = waiting;
}

std::string Request::getMethod()
{
	return (method);
	return (method);
}

std::string Request::getTarget()
{
	return (target);
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

bool	Request::getWaitingForBody()
{
		return (waitingForBody);
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

bool		Request::getCgi()
{
	return (cgi);
}

std::string	&Request::getCgiOutput()
{
	return(cgiOutput);
}

Socket		&Request::getClient()
{
	return (client);
}

Cluster		&Request::getCluster()
{
	return (cluster);
}