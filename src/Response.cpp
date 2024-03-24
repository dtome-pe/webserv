#include <webserv.hpp>

Response::Response()
{	

}

Response::~Response()
{

}

void	Response::handleRequest(Request &request, const Server *serv, const Locations *loc)
{	
	cout << "entra en handle request ";
	/*comprobamos el path del request y realizamos comprobaciones pertinentes*/
	std::string path = getPath(request, serv, loc); // tambien parseamos una posible question query, para conducir a archivo cgi de manera correcta
	cout << "con path: " << path << endl;
	if (path == "none") // no hay root directives, solo daremos una pagina de webserv si se accede al '/', si no 404
	{
		if (!check_method(request.getMethod(), NULL, serv)) // bloqueamos toda peticion que no sea GET, 405
		{
			setResponse(405, *this, "", serv, loc);
			return ;
		}
		/*Si la peticion es al root, damos pagina default, si no 404*/
		if (request.getTarget() == "/")
			setResponse(200, *this, readFileContents("default/default.html"), NULL, NULL);
		else
			setResponse(404, *this, "", serv, NULL);
	}
	else
	{
		if (!check_method(request.getMethod(), loc, serv)) // si metodo no permitido, 405
		{
			setResponse(405, *this, "", serv, loc);
			return ;
		}
		if (loc && loc->getRedirection().length() > 0) // comprobar si hay directive return
		{
			setResponse(loc->getRedirectionNumber(), *this, loc->getRedirection(), NULL, NULL); // pasaremos setResponse cuando tengamos map<int,string>
			return ;
		}
		if (!checkGood(path))  // si el path que ha resultado no existe, 404
		{
			setResponse(404, *this, "", serv, NULL);
			return ;
		}
		if (checkFileOrDir(path) == "dir" && !checkTrailingSlash(path))  // comprobamos si tiene o no trailing slash, nginx hace una redireccion 301 a URL con final slash
		{
			setResponse(301, *this, request.getTarget() + "/", NULL, NULL);
			return ;
		}
		if (checkFileOrDir(path) == "file")
		{
			//cout << "path is good and it's a file"  << endl; // si corresponde a un archivo, lo servimos con un 200
			if (checkCgi(path, loc)) // chequearemos si location tiene activado el cgi y para que extensiones
			{
				cgi(*this, request, path);
				return ;
			}
			else
				setResponse(200, *this, readFileContents(path), NULL, NULL); //sino servimos el recurso de manera normal
		}
		else // si corresponde a un directorio, primero miramos que no haya un index file
		{
			if (serv->getVIndex().size() > 0 || (loc && loc->getIndex().size() > 0)) // si hay index directive
			{
				std::string index_file = findIndex(path, serv, loc); // localizamos si el camino lleva a un archivo
				if (index_file != "")  // en caso afirmativo, se sirve
				{
					setResponse(200, *this, readFileContents(index_file), NULL, NULL);
					return ;
				}
				else // si no, damos un forbidden, como nginx
				{
					setResponse(403, *this, "", serv, NULL);
					return ;
				}
			}
			if (findIndexHtml(path)) // sino, buscamos un archivo index.html para servir.
			{
				path += "index.html";
				setResponse(200, *this, readFileContents(path), NULL, NULL);
				return ;
			}
			else // sino, comprobamos si tiene autoindex activado para mostrar listado directorio
			{
				if (!loc || !loc->getAutoindex()) // si no tiene autoindex (como solo lo puede tener un location, de momento), devolvemos 403 ya que no esta activado el directorylisting
											// y no tenemos permiso para coger ningun archivo de directorio
				{
					setResponse(403, *this, "", serv, NULL);
					return ;
				}
				else
				{
					std::string content = generateDirectoryListing(path);
					if (content == "Error 500") // por si da algun error interno el server.
						setResponse(200, *this, "", NULL, NULL);
					else
						setResponse(200, *this, content, NULL, NULL);
					return ;
				}
			}		
		}
	}
}

std::string Response::makeResponse()
{
	return (this->status_line.line + this->headers.makeHeader()
			+ "\r\n" + this->body);
}

void	Response::setStatusLine(std::string _status_line)
{
	this->status_line.line = _status_line + "\r\n";
	std::vector<std::string> split = HeaderHTTP::split(_status_line, " ");

	this->status_line.protocol = split[0];
	this->status_line.code = split[1];
	this->status_line.text = split[2];
}

void	Response::setHeader(std::string _header)
{
	this->headers.setHeader(_header);
}

void	Response::setBody(std::string _body)
{
	this->body = _body;
}
