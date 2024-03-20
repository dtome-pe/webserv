#include <webserv.hpp>

void Response::do_cgi(Request &request, std::string &path)
{
	(void) path;
	
	int pipe_fd[2];
	
    if (pipe(pipe_fd) == -1) 
	{
        strerror(errno);
        setResponse(500, *this, "", NULL, NULL);
		return ;
    }
	pid_t	pid = fork();
	if (pid == -1)
	{
		strerror(errno);
		setResponse(500, *this, "", NULL, NULL);
		return ;
	}
	if (pid == 0)
	{
		close(pipe_fd[0]);
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
		{
			strerror(errno);
			setResponse(500, *this, "", NULL, NULL);
			return ;
		}
		close(pipe_fd[1]);
		execve(path.c_str(), setArgv(path), setEnvp(request, path));
	}
	else
	{
        close(pipe_fd[1]);
		int	status;
        int result = waitpid(pid, &status, 0);
		if (result == -1)
		{
			strerror(errno);
			setResponse(500, *this, "", NULL, NULL);
			return ;
		}
		else
		{
			if (WIFEXITED(status))
       		{
            	int exitStatus = WEXITSTATUS(status);
				if (exitStatus == -1)
				{
					setResponse(500, *this, "", NULL, NULL);
					return ;
				}
				else
				{
					/* char buffer[4096];
        			ssize_t bytesRead;
					std::string content;
					while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
					{
						content.append(buffer,bytesRead);
					} */
					std::string content = bounceContent(pipe_fd);
					std::string contentType = getCgiHeader(content, "Content-Type:");
					removeHeaderLine(content);
					cout << "content type value parsed was " << contentType << endl;
					this->setStatusLine("HTTP/1.1 200 OK");
					this->setHeader("Content-Type: " + contentType);
					this->setHeader("Content-Length: " + getLengthAsString(content));
					this->setBody(content);
					cout << content << endl;
					close(pipe_fd[0]);  // Close read end in the parent
				}
       		}
		}
	}
}

void Response::do_redirection(Request &request, std::string return_str)
{	
	(void) request;
	cout << "entra en redirection. return str is " << return_str << endl;

	std::string code = return_str.substr(0, return_str.find(" "));
	cout << "code is: " << code << endl;

	std::string location = return_str.substr(return_str.find(" ") + 1, return_str.length());

	cout << "url is: " << location << endl;

/* 	if (code == location) // solo hay un código
	{
		int c;
		stringstream(code) >> c;

		makeReturnCode(c, *this);
	} */
	if (code == "301")
		setResponse(301, *this, location, NULL, NULL);
}

Response::Response(Request &request, const Server *serv, const Locations *loc)
{
	/*comprobamos el path del request y realizamos comprobaciones pertinentes*/
	std::string path = getPath(request, serv, loc); // tambien parseamos una posible question query, para conducir a archivo cgi de manera correcta
	cout << path << endl;
	if (path == "none") // no hay root directives, solo daremos una pagina de webserv si se accede al '/', si no 404
	{
		if (request.getTarget() == "/")
			setResponse(200, *this, readFileContents(getDefaultFile("/default.html")), NULL, NULL);
		else
			setResponse(404, *this, "", serv, NULL);
	}
	else
	{
		if (!check_method(request.getMethod(), loc)) // si metodo no permitido, 405
		{
			setResponse(405, *this, "", serv, loc);
			return ;
		}
		std::string return_str = checkReturn(loc); // luego se comprueban redirecciones
		if (return_str != "")
		{
			do_redirection(request, return_str);
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
			if (checkCgi(path)) // chequearemos si location tiene activado el cgi y para que extensiones
			{
				do_cgi(request, path);
				return ;
			}
			else
				setResponse(200, *this, readFileContents(path), NULL, NULL); //sino servimos el recurso de manera normal
		}
		else // si corresponde a un directorio, primero miramos que no haya un index file
		{
			if (serv->getVIndex().size() > 0 || (loc && loc->getIndex().size() > 0)) // si hay index directive
			{
				cout << "entra aqui" << endl;
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
