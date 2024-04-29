#include <webserv.hpp>

Response::Response()
{	
	body = "";
	waitingForBody = false;
	autoindex = false;
	code = "";
}

Response::~Response()
{

}

int		Response::getResponseCode(Request &request, const Server *serv, const Location *loc)
{
	if (entityTooLarge(request, serv))
		return (413);

	std::string path = getPath(request, serv, loc);

	if (path == "none")
		return (noRoot(*this, request, serv));
	else
	{
		if (methodNotAllowed(request.getMethod(), loc, serv))
			return (405);

		if (loc && loc->getRedirection().length() > 0)
			return (loc->getRedirectionNumber());

		if (!pathIsGood(path))
			return (putOr404(*this, request, path));

		if (needTrailSlashRedir(request, path))  // comprobamos si tiene o no trailing slash, nginx hace una redireccion 301 a URL con final slash
			return (trailSlashRedir(request));

		if (request.getMethod() == "DELETE" || request.getMethod() == "PUT")
			return (putOrDel(*this, request, path));

		if (checkFileOrDir(path) == "file")
			return (cgiOr200(*this, request, path, loc));

		else // si corresponde a un directorio, primero miramos que no haya un index file
			return (indexDirectiveOrIndexOrAutoIndex(*this, request, path, serv, loc));
	}
}

void	Response::setBasicHeaders(int code, Request &request)
{
	setHeader("Server: Webserv");
	setHeader("Date: " + getCurrentTime());
	if (body != "")
	{	
		if (getCgiHeader("Content-Type") != "not found")
			setHeader("Content-Type: " + getCgiHeader("Content-Type"));
		if (getHeader("Content-Type") == "not found") // si no esta seteado, (por cgi), se pone setea.
		{
			if (this->autoindex)
				setHeader("Content-Type: " + MIME::getMIMEType("html"));
			else
				setHeader("Content-Type: " + MIME::getMIMEType(request.getExtension()));
		}
		if (getCgiHeader("Content-Length") != "not found")
			setHeader("Content-Length: " + getCgiHeader("Content-Length"));
		if (getHeader("Content-Length") == "not found" && body.length() > 0)
			setHeader("Content-Length: " + getLengthAsString(body));
	}
	if (body == "" && code == 200)
	{
		setHeader("Content-Type: text/plain");
		setHeader("Content-Length: 0");
	}
	if (code == 201)
		setHeader("Content-Length: 0");

	if (code == 301 || code == 302 || code == 303)
	{	if (request.getTrailSlashRedir())
			setHeader("Location: " + request.getTarget() + "/");
		else
			setHeader("Location: " + request.getLocation()->getRedirection());
	}
	if (code == 201)
		setHeader("Location: http://" + request.getLocationDir());
	std::string lastMod = getLastModified(request.getPath());
	if (lastMod != "error" && request.getMethod() == "GET" && !request.getCgi())
		setHeader("Last-Modified: " + lastMod);
	if (request.getKeepAlive())
		setHeader("Connection: keep-alive");
}

void	Response::setResponse(int code, Request &request)
{
	if (code == CGI)
	{
		setCode("42");
		return ;
	}
	switch (code)
	{
		case 100:
		{
			setStatusLine("HTTP/1.1 100 Continue");
			return;
		}
		case 200:
		{
			setStatusLine("HTTP/1.1 200 OK");
			break;
		}
		case 201:
		{
			setStatusLine("HTTP/1.1 201 Created");
			break;
		}
		case 204:
		{
			setStatusLine("HTTP/1.1 204 No Content");
			break;
		}
		case 301:
		{
			setStatusLine("HTTP/1.1 301 Moved Permanently");
			break;
		}
		case 302:
		{
			setStatusLine("HTTP/1.1 302 Found");
			break;
		}
		case 303:
		{
			setStatusLine("HTTP/1.1 303 See Other");
			break;
		}
		case 400:
		{
			setStatusLine("HTTP/1.1 400 Bad Request");
			makeDefault(400, request, *this, "/400.html", request.getServer());
			break;
		}
		case 403:
		{
			setStatusLine("HTTP/1.1 403 Forbidden");
			makeDefault(403, request, *this, "/403.html", request.getServer());
			break;
		}
		case 404:
		{
			setStatusLine("HTTP/1.1 404 Not Found");
			makeDefault(404, request, *this, "/404.html", request.getServer());
			break;
		}
		case 405:
		{
			setStatusLine("HTTP/1.1 405 Method Not Allowed");
			std::string allow_header = "Allow: ";			/*allow header obligatorio en caso de mensaje 405 method not allowed
															donde se informan de metodos aceptados en URL*/
			if (request.getLocation())
			{
				if (request.getLocation()->getMethods()[0] == 1)
					allow_header += "GET, ";
				if (request.getLocation()->getMethods()[1] == 1)
					allow_header += "POST, ";
				if (request.getLocation()->getMethods()[2] == 1)
					allow_header += "DELETE";
				if (request.getLocation()->getMethods()[3] == 1)
					allow_header += "PUT";
			}
			else
			{
				allow_header += "GET, ";
			}
			setHeader(allow_header);
			makeDefault(405, request, *this, "/405.html", request.getServer());
			break;
		}
		case 409:
		{
			setStatusLine("HTTP/1.1 409 Conflict");
			makeDefault(409, request, *this, "/409.html", request.getServer());
			break;
		}
		case 413:
		{
			setStatusLine("HTTP/1.1 413 Request Entity Too Large");
			makeDefault(413, request, *this, "/413.html", request.getServer());
			break;
		}
		case 414:
		{
			setStatusLine("HTTP/1.1 414 Request-URI Too Long");
			makeDefault(413, request, *this, "/414.html", request.getServer());
			break;
		}
		case 500:
		{
			setStatusLine("HTTP/1.1 500 Internal Server Error");
			makeDefault(500, request, *this, "/500.html", request.getServer());
			break;
		}
		case 501:
		{
			setStatusLine("HTTP/1.1 501 Not Implemented");
			makeDefault(500, request, *this, "/501.html", request.getServer());
			break;
		}
	}
	setBasicHeaders(code, request);
}

void	Response::makeDefault(int code, Request &request, Response &response, const std::string &file, const Server *serv)
{
	std::string	content = "";
	std::map<int, std::string>::const_iterator it = serv->getErrorPage().find(code);
	if (it != serv->getErrorPage().end())
	{	
		cout << it->second << endl;
		std::string path = serv->getRoot() + it->second;
		if (pathIsGood(path))
		{	
			cout << "path is good" << endl;
			content = readFileContents(request, path);
		}
		else
			content = readFileContents(request, "default" + file);
	}
	else
		 content = readFileContents(request, "default" + file);
	response.setBody(content);
}

std::string Response::makeResponse()
{
	return (line + this->headers.makeHeader()
			+ "\r\n" + this->body);
}

void	Response::parseCgi(std::string text)
{
	if (text == "\n")
		waitingForBody = true;
	else
		setCgiHeader(text.substr(0, text.length() - 1));
}

void		Response::setStatusLine(std::string _status_line)
{
	this->line = _status_line + "\r\n";
	std::vector<std::string> split = HeaderHTTP::split(_status_line, " ");

	this->protocol = split[0];
	this->code = split[1];
	this->text = split[2];
}

void		Response::setHeader(std::string _header)
{
	this->headers.setHeader(_header);
}

void		Response::setCgiHeader(std::string _cgiHeader)
{
	this->cgiHeaders.setHeader(_cgiHeader);
}

void		Response::setBody(std::string _body)
{
	this->body = _body;
}

void		Response::setCode(std::string _code)
{
	this->code = _code;
}

std::string	Response::getHeader(std::string _header)
{
	return (this->headers.getHeader(_header));
}

std::string	Response::getCgiHeader(std::string _cgiHeader)
{
	return (this->cgiHeaders.getHeader(_cgiHeader));
}

std::string Response::getCode()
{
	return (code);
}
