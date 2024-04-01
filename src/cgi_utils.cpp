#include <webserv.hpp>

static char* strdup_cpp98(const char* str)
{
    if (str == NULL)
        return NULL;

    size_t len = std::strlen(str);
    char* newStr = static_cast<char*>(std::malloc(len + 1));

    if (newStr != NULL)
        std::strcpy(newStr, str);

    return (newStr);
}

char* const*	setEnvp(Request &request, std::string &path, std::string &method)
{	
    if (method == "DELETE")
    {
        return (NULL);
    }

	std::string file = request.getTarget().substr(request.getTarget().find_last_of("/"), request.getTarget().length()); // nos quedamos con lo que hay tras el ultimo slash
    std::vector<std::string>env;

    if (request.getHeaders().map.count("Authorization") > 0)
    {
        env.push_back("AUTH_TYPE=" + request.getHeader("Authorization").substr(0, request.getHeader("Authorization").find(" ")));
    }
    if (request.getBody().length() > 0)
    {
        env.push_back("CONTENT_LENGTH=" + int_to_str(request.getBody().length()));
        if (request.getHeaders().map.count("Content-Type") > 0)
            env.push_back("CONTENT_TYPE=" + request.getHeader("Content-Type"));
    }
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("PATH_INFO=" + path);
    if (request.getMethod() == "GET")
	    env.push_back("QUERY_STRING=" + file.substr(file.find("?") + 1, file.length()));
    env.push_back("REMOTE_ADDR=" + request.getIp());
    if (request.getHost().length() > 0)
        env.push_back("REMOTE_HOST=" + request.getHost());
    env.push_back("REQUEST_METHOD=" + request.getMethod());
    env.push_back("SCRIPT_FILENAME=" + path);
    env.push_back("SERVER_NAME=" + request.getHost());
    env.push_back("SERVER_PORT=" + request.getPort());
    env.push_back("SERVER_PROTOCOL=" + request.getVersion());
    env.push_back("SERVER_SOFTWARE=Webserv");
    if (request.getUploadStore() != "")
        env.push_back("UPLOAD_DIR=" + request.getUploadStore());

    if (method == "PUT")
    {
        env.push_back("FILENAME=" + path.substr(path.find_last_of("/"), path.length()));
        if (request.getUploadStore() != "")
            env.push_back("UPLOAD_DIR=" + request.getUploadStore());
        else
            env.push_back("UPLOAD_DIR=" + path.substr(0, path.find_last_of("/")));
    }
	char** envp = new char*[env.size() + 1];

    for (size_t i = 0; i < env.size(); ++i)
    {
        //cout << env[i] << endl;
        envp[i] = strdup_cpp98(env[i].c_str());
    }
	envp[env.size()] = NULL;
	return (envp);
}

char* const* 	setArgv(Request &request, std::string &path, std::string &method)
{	
    std::vector<std::string> arg;
    if (method == "DELETE")
    {   
        arg.push_back("/usr/bin/php8.1");
        arg.push_back("./cgi-bin/delete.php");
        arg.push_back(path);
    }
    else if (method == "PUT")
    {
        arg.push_back("/usr/bin/php8.1");
        arg.push_back("./cgi-bin/put.php");
        arg.push_back(path);
    }
    else
    {
        arg.push_back(request.getCgiBinary());
        arg.push_back(path);
    }

    char** argv = new char*[arg.size() + 1];

    for (size_t i = 0; i < arg.size(); ++i)
    {
        argv[i] = strdup_cpp98(arg[i].c_str());
    }
    argv[arg.size()] = NULL;

	return (argv);
}

std::string bounceContent(int fd)
{
	char buffer[4096];
    ssize_t bytesRead;
	std::string content;
	while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
		content.append(buffer,bytesRead);
    close(fd);  // Close read end in the parent
	return (content);
}

std::string parseCgiHeader(Response &response, std::string& content) 
{  
    std::string line;

    // Read headers until an empty line is encountered
    line = content.substr(0, content.find("\n"));
    cout << "line: " << line << endl << "length: " << line.length() << endl;
    while (line.length() > 1)
    {
        response.setHeader(line);
        content = content.substr(line.length() + 1, content.length());
        line = content.substr(0, content.find("\n"));
        cout << "line: " << line << endl << "length: " << line.length() << endl;
    }
    return (content);
}

bool    checkPut(std::string &path)
{
    if (checkFileOrDir(path) == "dir")
        return (true);
    return (false);
}

bool    checkPutFile(std::string &path)
{
    if (path[path.length() - 1] != '/' && checkGood(path.substr(0, path.find_last_of("/"))))
        return (true);
    return (false);
}
