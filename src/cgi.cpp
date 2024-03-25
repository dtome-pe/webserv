#include <webserv.hpp>

void	cgi(Response &response, Request &request, std::string &path)
{
	int pipe_to_child[2];
	int pipe_from_child[2];
	
    if (pipe(pipe_to_child) == -1) 
	{
        strerror(errno);
        setResponse(500, response, "", NULL, NULL);
		return ;
    }
	if (pipe(pipe_from_child) == -1) 
	{
        strerror(errno);
        setResponse(500, response, "", NULL, NULL);
		return ;
    }
	fcntl(pipe_to_child[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(pipe_to_child[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(pipe_from_child[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(pipe_from_child[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	if (request.getMethod() == "POST")
	{	
		cout << "entra a escribir" << endl;
		ssize_t bytes_written = write(pipe_to_child[1], request.getBody().c_str(), request.getBody().size());
		if (bytes_written == -1)
		{
			strerror(errno);
			setResponse(500, response, "", NULL, NULL);
			return;
		}
	}
	pid_t	pid = fork();
	if (pid == -1)
	{
		strerror(errno);
		setResponse(500, response, "", NULL, NULL);
		return ;
	}
	if (pid == 0)
	{
		close(pipe_to_child[1]);
		close(pipe_from_child[0]);

		char* const* argv = setArgv(request, path);
		char * const* envp = setEnvp(request, path);

		if (dup2(pipe_to_child[0], STDIN_FILENO) == -1)
		{
			strerror(errno);
			setResponse(500, response, "", NULL, NULL);
			return ;
		}
		if (dup2(pipe_from_child[1], STDOUT_FILENO) == -1)
		{
			strerror(errno);
			setResponse(500, response, "", NULL, NULL);
			return ;
		}
		close(pipe_to_child[0]);
		close(pipe_from_child[1]);
		execve(request.getCgiBinary().c_str(), argv, envp);
	}
	else
	{	
		close(pipe_to_child[0]);
		close(pipe_to_child[1]);
		close(pipe_from_child[1]);
		int	status;
		cout << "path to execve: " << path << endl;
        int result = waitpid(pid, &status, 0);
		cout << result << endl;
		if (result == -1)
		{
			strerror(errno);
			setResponse(500, response, "", NULL, NULL);
			return ;
		}
		else
		{
			if (WIFEXITED(status))
       		{
            	int exitStatus = WEXITSTATUS(status);
				if (exitStatus == -1)
				{
					setResponse(500, response, "", NULL, NULL);
					return ;
				}
				else
				{
					std::string content = bounceContent(pipe_from_child[0]);
				//	cout << "content: " << content << endl;
					std::string contentType = getCgiHeader(content, "Content-Type:");
					removeHeaderLine(content);
					response.setStatusLine("HTTP/1.1 200 OK");
					response.setHeader("Content-Type: text/html");
					response.setHeader("Content-Length: " + getLengthAsString(content));
					response.setBody(content);
					close(pipe_from_child[0]);  // Close read end in the parent
				}
       		}
		}
	}
}