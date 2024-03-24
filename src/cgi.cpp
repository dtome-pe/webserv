#include <webserv.hpp>

void	cgi(Response &response, Request &request, std::string &path)
{
	(void) path;
	
	int pipe_fd[2];
	
	cout << "entra en cgi" << endl;
    if (pipe(pipe_fd) == -1) 
	{
        strerror(errno);
        setResponse(500, response, "", NULL, NULL);
		return ;
    }
	fcntl(pipe_fd[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(pipe_fd[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	pid_t	pid = fork();
	if (pid == -1)
	{
		strerror(errno);
		setResponse(500, response, "", NULL, NULL);
		return ;
	}
	if (pid == 0)
	{
		if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
		{
			strerror(errno);
			setResponse(500, response, "", NULL, NULL);
			return ;
		}
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
		{
			strerror(errno);
			setResponse(500, response, "", NULL, NULL);
			return ;
		}
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		execve(path.c_str(), setArgv(path), setEnvp(request, path));
	}
	else
	{	
		if (request.getMethod() == "POST")
		{
			cout << "entra aqui" << endl;
			ssize_t bytes_written = write(pipe_fd[1], request.getBody().c_str(), request.getBody().size());
			if (bytes_written == -1)
			{
				strerror(errno);
				setResponse(500, response, "", NULL, NULL);
				return;
			}
		}
		close(pipe_fd[0]);
        close(pipe_fd[1]);
		int	status;
        int result = waitpid(pid, &status, 0);
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
					std::string content = bounceContent(pipe_fd);
					std::string contentType = getCgiHeader(content, "Content-Type:");
					removeHeaderLine(content);
					response.setStatusLine("HTTP/1.1 200 OK");
					response.setHeader("Content-Type: " + contentType);
					response.setHeader("Content-Length: " + getLengthAsString(content));
					response.setBody(content);
					close(pipe_fd[0]);  // Close read end in the parent
				}
       		}
		}
	}
}