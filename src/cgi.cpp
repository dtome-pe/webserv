#include <webserv.hpp>

void setDel(Response &response, Request &request, std::string &path, std::string method)
{
	pid_t	pid = fork();
	if (pid == -1)
	{
		strerror(errno);
		setResponse(500, response, "", NULL, NULL);
		return ;
	}
	if (method == "DELETE")
	{
		if (pid == 0)
		{
			char* const* argv = setArgv(request, path, method);
			execve("/usr/bin/php8.1", argv, NULL);
			cout << strerror(errno) << endl;
		}
		else
		{
			int	status;
			int result = waitpid(pid, &status, 0);
			//cout << result << endl;
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
						setResponse(204, response, "", NULL, NULL);
					}
				}
			}
		}
	}
}

void	setPut(Response &response, Request &request, std::string &path, std::string method)
{
	int pipe_to_child[2];
	if (pipe(pipe_to_child) == -1) 
	{
        strerror(errno);
        setResponse(500, response, "", NULL, NULL);
		return ;
    }
	fcntl(pipe_to_child[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(pipe_to_child[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	ssize_t bytes_written = write(pipe_to_child[1], request.getBody().c_str(), request.getBody().size());
	cout << "bytes written: " << bytes_written << endl;
	if (bytes_written == -1)
	{
		strerror(errno);
		setResponse(500, response, "", NULL, NULL);
		return;
	}
	pid_t	pid = fork();
	if (pid == -1)
	{
		strerror(errno);
		setResponse(500, response, "", NULL, NULL);
		return ;
	}
	if (method == "PUT")
	{
		if (pid == 0)
		{	
			char* const* argv = setArgv(request, path, method);
			char* const* envp = setEnvp(request, path, method);
			close(pipe_to_child[1]);
			if (dup2(pipe_to_child[0], STDIN_FILENO) == -1)
			{
				strerror(errno);
				setResponse(500, response, "", NULL, NULL);
				return ;
			}
			execve("/usr/bin/php8.1", argv, envp);
		}
		else
	{	
		close(pipe_to_child[0]);
		close(pipe_to_child[1]);
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
				cout << "exit status: " << exitStatus << endl;
				if (exitStatus == -1)
				{
					setResponse(500, response, "", NULL, NULL);
					return ;
				}
				else
				{	
					setResponse(exitStatus, response, path, NULL, NULL);
				}
       		}
		}
	}
	}
}


void	cgi(Response &response, Request &request, std::string &path, std::string method)
{
	if (method == "PUT")
	{
		setPut(response, request, path, method);
		return ;
	}
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
	if (method== "POST")
	{
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

		char* const* argv = setArgv(request, path, method);
		char * const* envp = setEnvp(request, path, method);

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
					content = parseCgiHeader(response, content);
					//cout << "content: " << content << endl;
					setResponse(200, response, content, NULL, NULL);
				}
       		}
		}
	}
}