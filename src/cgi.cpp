#include <webserv.hpp>

int	setDel(Request &request, std::string &path, std::string method)
{
	pid_t	pid = fork();
	if (pid == -1)
	{
		cerr << strerror(errno) << endl;
		return (500);
	}
	if (pid == 0)
	{
		char* const* argv = setArgv(request, path, method);
		execve("/usr/bin/php8.1", argv, NULL);
		cerr << strerror(errno) << endl;
		return (500);
	}
	else
	{
		int	status;
		int result = waitpid(pid, &status, 0);
		//cout << result << endl;
		if (result == -1)
		{
			cerr << strerror(errno) << endl;
			return (500);
		}
		else
		{
			if (WIFEXITED(status))
			{
				int exitStatus = WEXITSTATUS(status);
				if (exitStatus == -1)
					return (500);
				else
					return (204);
			}
			return (500);
		}
	}
}

int	setPut(Response &response, Request &request, std::string &path, std::string method)
{	
	(void) response;

	int pipe_to_child[2];
	if (pipe(pipe_to_child) == -1 || request.getBody() == "") 
        return (500);
	fcntl(pipe_to_child[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(pipe_to_child[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	ssize_t bytes_written = write(pipe_to_child[1], request.getBody().c_str(), request.getBody().size());
	cout << "bytes written: " << bytes_written << endl;
	if (bytes_written == -1)
	{
		cerr << strerror(errno) << endl;
		return (500);
	}
	pid_t	pid = fork();
	if (pid == -1)
	{
		cerr << strerror(errno) << endl;
		return (500);
	}
	if (pid == 0)
	{	
		char* const* argv = setArgv(request, path, method);
		char* const* envp = setEnvp(request, path, method);
		close(pipe_to_child[1]);
		if (dup2(pipe_to_child[0], STDIN_FILENO) == -1)
		{
			cerr << strerror(errno) << endl;
			return (500);
		}
		execve("/usr/bin/php8.1", argv, envp);
		return (500);
	}
	else
	{	
		close(pipe_to_child[0]);
		close(pipe_to_child[1]);
		int	status;
        int result = waitpid(pid, &status, 0);
		if (result == -1)
		{
			cerr << strerror(errno) << endl;
			return (500);
		}
		else
		{
			if (WIFEXITED(status))
       		{
            	int exitStatus = WEXITSTATUS(status);
				cout << "exit status: " << exitStatus << endl;
				if (!exitStatus)
					return (500);
				else
					return (exitStatus);
       		}
			return (500);
		}
	}
}


int	cgi(Response &response, Request &request, std::string &path, std::string method)
{
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
	if (method== "POST")
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
		execve(request.getCgiBinary().c_str(), argv, envp);
		return (500);
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
			cerr << strerror(errno) << endl;
			return (500);
		}
		else
		{
			if (WIFEXITED(status))
       		{
            	int exitStatus = WEXITSTATUS(status);
				if (exitStatus == -1)
					return (500);
				else
				{
					std::string content = bounceContent(pipe_from_child[0]);
					content = parseCgiHeader(response, content);
					//cout << "content: " << content << endl;
					response.setBody(content);
					//cout << "content: " << content << endl;
					return (200);
				}
       		}
			return (500);
		}
	}
}