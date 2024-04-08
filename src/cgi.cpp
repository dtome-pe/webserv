#include <webserv.hpp>

int	cgi(Response &response, Request &request, std::string path, std::string method)
{
	if (method != "output") // no es la respuesta de un proceso de cgi
	{
			/* Si el metodo es PUT, que siempre resulta en subir o modificar un archivo en el server, 
		pero no hay upload_store, vamos a entender que el location no permite subir archivos y daremos 403.*/
		if (method == "PUT" && request.getUploadStore() == "") 
			return (403);
		/*si hemos recibido un expect 100 para recibir archivo grande, aceptamos*/
		if (request.getHeader("Expect") == "100-continue")
			return (100);
		int pipe_to_child[2];
		int pipe_from_child[2];
		
		if (pipe(pipe_to_child) == -1 || pipe(pipe_from_child) == -1) 
		{
			cerr << strerror(errno) << endl;
			return (500);
		}
		fcntl(pipe_to_child[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		fcntl(pipe_to_child[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		fcntl(pipe_from_child[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		fcntl(pipe_from_child[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		if (method == "POST" || method == "PUT")
		{
			ssize_t bytes_written = write(pipe_to_child[1], request.getBody().c_str(), request.getBody().size());
			if (bytes_written == -1)
			{
				cerr << strerror(errno) << endl;
				return (500);
			}
		}
		pid_t	pid = fork();
		if (pid == -1)
		{
			cerr << strerror(errno) << endl;
			return (500);
		}
		if (pid == 0)
		{
			close(pipe_to_child[1]);
			close(pipe_from_child[0]);
			char* const* argv = setArgv(request, path, method);
			char * const* envp = setEnvp(request, path, method);

			if (dup2(pipe_to_child[0], STDIN_FILENO) == -1 || dup2(pipe_from_child[1], STDOUT_FILENO) == -1)
			{
				cerr << strerror(errno) << endl;
				return (500);
			}
			close(pipe_to_child[0]);
			close(pipe_from_child[1]);
			if (request.getMethod() == "PUT" || request.getMethod() == "DELETE")
				execve("/usr/bin/python3", argv, envp);
			else
				execve(request.getCgiBinary().c_str(), argv, envp); // POST - GET CGI
			return (500);
		}
		else
		{
			close(pipe_to_child[0]);
			close(pipe_to_child[1]);
			close(pipe_from_child[1]);
			request.getSocket().setCgiFd(pipe_from_child[0]);
			request.getSocket().setCgi(true);
			request.getCluster().setPid(pid, pipe_from_child[0], request.getSocket());
			return (CGI);
		}
	}
	else
	{
		string content = parseCgiHeader(response, request.getCgiOutput());
		response.setBody(content);
		close(request.getSocket().getCgiFd());
		if (response.getHeader("Status") != "not found")
			return (str_to_int(response.getHeader("Status").substr(0, response.getHeader("Status").find(" "))));
		else
			return (200);
	}
}