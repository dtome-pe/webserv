#include <webserv.hpp>

void Response::do_default() // damos default.html si se accede al root del server pero no hay root directive
{
	cout << "entra en do default" << endl;

	this->setStatusLine("HTTP/1.1 200 OK");

	const char *env_path = std::getenv("DEFAULT_DIR");
	std::string default_path;

	if (env_path)
		default_path = std::string(env_path) + "/default.html";
	else
	{
		cerr << "env not set, getting default path at /home/theonewhoknew/repos/CURSUS/webserv/default" << endl;
		default_path = "/home/theonewhoknew/repos/CURSUS/webserv/default/default.html";
	}
	std::string content = readFileContents(default_path);
	this->setHeader("Content-Length: " + getLengthAsString(content));
	this->setBody(content);
}

void Response::do_redirection(Request &request, std::string return_str)
{
	cout << "entra en redirection. return str is " << return_str << endl;

	std::string code = return_str.substr(0, return_str.find(" "));
	std::string location = return_str.substr(return_str.find(" "), return_str.length());

	if (code == "301")
		do_301(request.ip + ":" + request.port + location);
}


void Response::do_200_get_path(std::string &path)
{
	this->setStatusLine("HTTP/1.1 200 OK");

	std::string content = readFileContents(path);

	this->setHeader("Content-Length: " + getLengthAsString(content));
	this->setBody(content);
}

void Response::do_200_get_content(std::string &content)
{
	this->setStatusLine("HTTP/1.1 200 OK");

	this->setHeader("Content-Length: " + getLengthAsString(content));
	this->setBody(content);
}

void Response::do_301(std::string location)
{
	cout << "entra en 301" << endl;

	this->setStatusLine("HTTP/1.1 301 Moved Permanently");

	this->setHeader("Location: http://" + location);
	this->setHeader("Connection: keep-alive");
}

void Response::do_404()
{
	cout << "entra en 404 " << endl;
	
	this->setStatusLine("HTTP/1.1 404 Not Found");
	this->setHeader("Content-Length: 52");
	this->setBody("The resource you were looking for could not be found");
}

void Response::do_405(const Locations *loc)
{
	this->setStatusLine("HTTP/1.1 405 Method Not Allowed");
	
	std::string allow_header = "Allow: ";			/*allow header obligatorio en caso de mensaje 405 method not allowed
													donde se informan de metodos aceptados en URL*/
	if (loc->getMethods()[0] == 1)
		allow_header += "GET, ";
	if (loc->getMethods()[1] == 1)
		allow_header += "POST, ";
	if (loc->getMethods()[1] == 1)
		allow_header += "DELETE";
	this->setHeader(allow_header);
	this->setBody("Method not allowed");
}

void Response::do_500()
{
	cout << "entra en 500 " << endl;
	
	this->setStatusLine("HTTP/1.1 500 Internal Server Error");
}

Response::Response(Request &request, const Server *serv, const Locations *loc)
{
	(void) serv;
	
	std::cout << "response: " <<  request.getMethod() << std::endl;

	if (!check_method(request.getMethod(), loc))
	{
		this->do_405(loc);
		return ;
	}
	/*aqui habria que gestionar redireccion, antes que nada = check_return()*/
	if (request.request_line.method == "GET")
		this->do_get(request, serv, loc);
}

void Response::do_get(Request &request, const Server *serv, const Locations *loc)
{
	/*primero chequeamos directiva return, lo paramos todo y enviamos una nueva url a cliente mediante 301, u otros*/
	std::string return_str = checkReturn(loc);
	if (return_str != "")
	{
		do_redirection(request, return_str);
		return ;
	}

	/*luego ya comprobamos el path del request y realizamos comprobaciones pertinentes*/
	std::string path = get_path(request, serv, loc);
	//cout << "resolved path is " << path << endl;
	if (path == "none") // no hay root directives, solo daremos una pagina de webserv si se accede al '/', si no 404
	{
		if (request.getTarget() == "/")
			do_default();
		else
			do_404();
	}
	else
	{
		if (!checkGood(path))  // si el path que ha resultado no existe, 404
		{
			do_404();
			return ;
		}
		if (checkFileOrDir(path) == "file")
		{
			cout << "path is good and it's a file"  << endl; // si corresponde a un archivo, lo servimos con un 200
			do_200_get_path(path);
		}
		else // si corresponde a un directorio, primero miramos que no haya un index file
		{
			cout << "path is good and it's a dir"  << endl;
			if (!checkTrailingSlash(path))  // comprobamos si tiene o no trailing slash, nginx hace una redireccion 301 a URL con final slash
				return do_301(request.ip + ":" + request.port + request.getTarget() + "/");
/* 			if (findIndex(path, serv, loc)) // checquearemos si hay un index directive, para intentar servir archivo index
			{

			} */
			if (findIndexHtml(path)) // sino, buscamos un archivo index.html para servir.
			{
				path += "index.html";
				do_200_get_path(path);
				return ;
			}
			else // sino, comprobamos si tiene autoindex activado para mostrar listado directorio
			{
				if (!loc->getAutoindex()) // si no tiene autoindex, devolvemos 404 ya que no esta activado el directorylisting
				{
					do_404();
					return ;
				}
				else
				{
					std::string content = generateDirectoryListing(path);
					if (content == "Error 500")
						do_500();
					else
						do_200_get_content(content);
					return ;
				}
			}		
		}
	}
}

Response::~Response()
{

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
//	for (size_t i = 0; i < split.size(); i++)
//		std::cout << split[i] << std::endl;

	this->status_line.protocol = split[0];
	this->status_line.code = split[1];
	this->status_line.text = split[2];

/* 	std::cout << "Response line elements:" << std::endl;
	std::cout << this->status_line.protocl << std::endl;
	std::cout << this->status_line.code << std::endl;
	std::cout << this->status_line.text << std::endl; */
}

void	Response::setHeader(std::string _header)
{
	this->headers.setHeader(_header);
}

void	Response::setBody(std::string _body)
{
	this->body = _body;
}
