#include <webserv.hpp>

int	cgi(Response &response, Request &request, std::string path, std::string method)
{
	if (method != "output") // no es la respuesta de un proceso de cgi
	{
		if (method == "PUT" && request.getUploadStore() == "") 
			return (403);
		if (request.getHeader("Expect") == "100-continue")
			return (100);
		int pipe_to_child[2];
		int pipe_from_child[2];
		
		if (pipe(pipe_to_child) == -1 || pipe(pipe_from_child) == -1) 
			return (500);
		fcntl(pipe_to_child[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		fcntl(pipe_to_child[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		fcntl(pipe_from_child[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		fcntl(pipe_from_child[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		if (method == "POST" || method == "PUT")
		{
			ssize_t bytes_written = write(pipe_to_child[1], request.getBody().c_str(), request.getBody().size());
			if (bytes_written == -1)
				return (500);
		}
		pid_t	pid = fork();
		if (pid == -1)
			return (500);
		if (pid == 0)
		{
			close(pipe_to_child[1]);
			close(pipe_from_child[0]);
			char* const* argv = setArgv(request, path, method);
			char * const* envp = setEnvp(request, path, method);

			if (dup2(pipe_to_child[0], STDIN_FILENO) == -1 || dup2(pipe_from_child[1], STDOUT_FILENO) == -1)
				cout << "Status: 500 Internal Server Error" << endl;
			close(pipe_to_child[0]);
			close(pipe_from_child[1]);
			if (request.getMethod() == "PUT" || request.getMethod() == "DELETE")
				execve("/usr/bin/python3", argv, envp);
			else
				execve(request.getCgiBinary().c_str(), argv, envp); // POST - GET CGI
			cout << "Status: 500 Internal Server Error\r\n" << endl;
			return (0);
		}
		else
		{
			close(pipe_to_child[0]);
			close(pipe_to_child[1]);
			close(pipe_from_child[1]);
			request.getClient().setCgiFd(pipe_from_child[0]);
			request.getClient().setCgi(true);
			request.getCluster().setPid(pid, pipe_from_child[0], request.getClient());
			return (CGI);
		}
	}
	else
	{
		cout << "entra en cgi output" << endl;
		close(request.getClient().getCgiFd());
		if (response.getCgiHeader("Status") != "not found")
			return (str_to_int(response.getCgiHeader("Status").substr(0, response.getHeader("Status").find(" "))));
		else
			return (200);
	}
}