#include <webserv.hpp>

Response::Response()
{	
	body = "";
}

Response::~Response()
{

}

int		Response::getResponseCode(Request &request, const Server *serv, const Location *loc)
{
	/*comprobamos el path del request y realizamos comprobaciones pertinentes*/
	if (request.getHeader("Content-Length") != "not found")
	{
		if (str_to_int(request.getHeader("Content-Length")) > serv->getMaxBodySize())
			return (413);
	}
	std::string path = getPath(request, serv, loc); // tambien parseamos una posible question query, para conducir a archivo cgi de manera correcta
//	cout << "con path: " << path << " con method " << request.getMethod() << endl;
	if (path == "none") // no hay root directives, solo daremos una pagina de webserv si se accede al '/', si no 404
	{
		if (!check_method(request.getMethod(), NULL, serv)) // bloqueamos toda peticion que no sea GET, 405
			return (405);
		/*Si la peticion es al root, damos pagina default, si no 404*/
		if (request.getTarget() == "/")
		{
			setBody(readFileContents(request, "default/default.html"));
			return (200);
		}
		else
			return (404);
	}
	else
	{
		if (!check_method(request.getMethod(), loc, serv)) // si metodo no permitido, 405
		{
			return (405);
		}
		if (loc && loc->getRedirection().length() > 0) // comprobar si hay directive return
			return (loc->getRedirectionNumber());
		if (!checkGood(path))  // si el path que ha resultado no existe, comprobamos si es un put y se puede acceder a la carpeta previa
		{
			if (request.getMethod() == "PUT")
			{
				if (checkPutFile(path)) // si server puede acceder a la carpeta donde se quiere crear el archivo, hacemos put
					return (cgi(*this, request, path, request.getMethod()));
			}
			else
				return (404);
		}
		if (request.getMethod() == "GET" &&  checkFileOrDir(path) == "dir" && !checkTrailingSlash(path))  // comprobamos si tiene o no trailing slash, nginx hace una redireccion 301 a URL con final slash
		{	
			request.setTrailSlashRedir(true);
			return (301);
		}
		if (request.getMethod() == "DELETE")
			return (cgi(*this, request, path, request.getMethod()));
		if (request.getMethod() == "PUT")
		{	
			if (path[path.length() - 1] != '/')  // comprobamos que el put no tenga como target un directorio, entonces se devuelve 409
				return (cgi(*this, request, path, request.getMethod()));
			else
				return (409);
		}
		if (checkFileOrDir(path) == "file")
		{
			if (checkCgi(request, path, loc)) // chequearemos si location tiene activado el cgi y para que extensiones
				return (cgi(*this, request, path, request.getMethod()));
			else
			{
				setBody(readFileContents(request, path)); //sino servimos el recurso de manera normal
				return (200); 
			}
		}
		else // si corresponde a un directorio, primero miramos que no haya un index file
		{
			if (serv->getVIndex().size() > 0 || (loc && loc->getIndex().size() > 0)) // si hay index directive
			{
				std::string index_file = findIndex(path, serv, loc); // localizamos si el camino lleva a un archivo
				if (index_file != "")  // en caso afirmativo, se sirve
				{
					setBody(readFileContents(request, index_file));
					return (200);
				}
				else // si no, damos un forbidden, como nginx
					return (403);
			}
			if (findIndexHtml(path)) // sino, buscamos un archivo index.html para servir.
			{
				setBody(readFileContents(request, path + "index.html"));
				return (200);
			}
			else // sino, comprobamos si tiene autoindex activado para mostrar listado directorio
			{
				if (!loc || !loc->getAutoindex()) // si no tiene autoindex (como solo lo puede tener un location, de momento), devolvemos 403 ya que no esta activado el directorylisting
											// y no tenemos permiso para coger ningun archivo de directorio
					return (403);
				else
				{
					std::string content = generateDirectoryListing(path);
					if (content == "Error 500") // por si da algun error interno el server.
						return (500);
					else
					{
						setBody(content);
						return (200);
					}
				}
			}		
		}
	}
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
		setHeader("Location: http://" + request.getLocationDir());
	std::string lastMod = getLastModified(request.getPath());
	if (lastMod != "error" && request.getMethod() == "GET" && !request.getCgi())
		setHeader("Last-Modified: " + lastMod);
	setHeader("Connection: keep-alive");
	if (getHeader("Status") != "not found")
		headers.removeHeader("Status");
}

void	Response::setResponse(int code, Request &request)
{
	//cout << "code: " << code << endl;
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

void		Response::setCode(std::string _code)
{
	this->code = _code;
}

std::string	Response::getHeader(std::string _header)
{
	return (this->headers.getHeader(_header));
}

std::string Response::getCode()
{
	return (code);
}
