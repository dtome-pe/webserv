#include <webserv.hpp>

Response::Response()
{	
	body = "";
}

Response::~Response()
{

}

void	Response::setBasicHeaders(int code, Request &request)
{
	setHeader("Server: Webserv");
	setHeader("Date: " + getCurrentTime());
	if (body != "")
	{
		if (getHeader("Content-type") == "not found") // si no esta seteado, (por cgi), se pone setea.
			setHeader("Content-type: " + MIME::getMIMEType(request.getExtension()));
		setHeader("Content-Length: " + getLengthAsString(body));
	}
	if (body == "" && code == 200)
	{
		setHeader("Content-type: text/plain");
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
		setHeader("Location: http://" + request.getHeader("Host") + request.getTarget());
	setHeader("Connection: keep-alive");
	if (getHeader("Status") != "not found")
		headers.removeHeader("Status");
}

void	Response::setResponse(int code, Request &request)
{
	cout << "code: " << code << endl;
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
	/*comprobariamos si serv tiene un error page establecido en conf file*/
	std::string	content = "";
	std::map<int, std::string>::const_iterator it = serv->getErrorPage().find(code);
	if (it != serv->getErrorPage().end())
	{	
		cout << "entra en el if" << endl;
		std::string path = serv->getRoot() + it->second;
		if (checkGood(path))
			content = readFileContents(request, path);
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

void		Response::setBody(std::string _body)
{
	this->body = _body;
}

std::string	Response::getHeader(std::string _header)
{
	return (this->headers.getHeader(_header));
}

std::string Response::getCode()
{
	return (code);
}
