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

char* const*	setEnvp(Request &request, std::string &path)
{	
	std::string file = request.getTarget().substr(request.getTarget().find_last_of("/"), request.getTarget().length()); // nos quedamos con lo que hay tras el ultimo slash
    std::vector<std::string>env;

    if (request.getHeaders().map.count("Authorization") > 0)
    {
        env.push_back("AUTH_TYPE = " + request.getHeader("Authorization").substr(0, request.getHeader("Authorization").find(" ")));
    }
    if (request.getBody().length() > 0)
    {
        env.push_back("CONTENT_LENGTH = " + int_to_str(request.getBody().length()));
        if (request.getHeaders().map.count("Content-Type") > 0)
            env.push_back("CONTENT_TYPE = " + request.getHeader("Content-Type"));
    }
    env.push_back("GATEWAY_INTERFACE = CGI/1.1");
    env.push_back("PATH_INFO = " + path);
    if (request.getMethod() == "GET")
	    env.push_back("QUERY_STRING = " + file.substr(file.find("?") + 1, file.length()));
    env.push_back("REMOTE_ADDR = " + request.ip);
    if (request.host.length() > 0)
        env.push_back("REMOTE_HOST = " + request.host);
    env.push_back("REQUEST_METHOD = " + request.getMethod());
    env.push_back("SCRIPT_NAME = " + path);
    env.push_back("SERVER_NAME = " + request.host);
    env.push_back("SERVER_PORT = " + request.port);
    env.push_back("SERVER_PROTOCOL = " + request.getVersion());
    env.push_back("SERVER_SOFTWARE = Webserv");

	char** envp = new char*[env.size() + 1];

    for (size_t i = 0; i < env.size(); ++i)
    {
        cout << env[i] << endl;
        envp[i] = strdup_cpp98(env[i].c_str());
    }
	envp[env.size()] = NULL;

	return (envp);
}

char* const* 	setArgv(std::string &path)
{	
	std::vector<std::string> arg;
    arg.push_back("." + path);

	char** argv = new char*[arg.size() + 1];

    for (size_t i = 0; i < arg.size(); ++i)
    {
        argv[i] = strdup_cpp98(arg[i].c_str());
    }
	argv[arg.size()] = NULL;

	return (argv);
}

static bool caseInsensitiveCompare(char c1, char c2) {
    return std::toupper(static_cast<unsigned char>(c1)) == std::toupper(static_cast<unsigned char>(c2));
}

std::string bounceContent(int *pipe_fd)
{
	char buffer[4096];
    ssize_t bytesRead;
	std::string content;
	while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
		content.append(buffer,bytesRead);
	return (content);
}

std::string getCgiHeader(const std::string& content, const std::string &header) 
{
    std::string::const_iterator pos = content.begin();

    while (true) 
	{
        pos = std::search(pos, content.end(), header.begin(), header.end(), caseInsensitiveCompare);

        if (pos == content.end()) {
            break;  // no se encontró header
        }
        // Check if "Content-Type:" is at the beginning of a line or the start of the content
        if (pos == content.begin() || *(pos - 1) == '\n' || *(pos - 1) == '\r') 
		{
            break;
        }
        // Iteramos para seguir buscando
        ++pos;
    }

    if (pos != content.end()) {
        // Extract the value of the "Content-Type" header
        std::string::const_iterator start = pos + header.size(); // Move past "Content-Type:"
        std::string::const_iterator end = std::find_if(start, content.end(), std::bind2nd(std::equal_to<int>(), '\n'));

        // Trim leading and trailing whitespace
        std::string::const_iterator first = std::find_if(start, end, std::not1(std::ptr_fun<int, int>(std::isspace)));
        std::string::const_iterator last = std::find_if(std::reverse_iterator<std::string::const_iterator>(end), std::reverse_iterator<std::string::const_iterator>(first), std::not1(std::ptr_fun<int, int>(std::isspace))).base();

        // Return the trimmed value
        return std::string(first, last);
    }

    // No se encontró header
    return "";
}

/* std::string removeHeaders(std::string& content) 
{
    std::string target[3] = {"Content-Type:", "Location:", "Status:"};
	for (unsigned int i = 0; i < 3; i++)
	{
		std::string::iterator pos = content.begin();

    	while (true) 
		{
			pos = std::search(pos, content.end(), target[i].begin(), target[i].end(), caseInsensitiveCompare);

			if (pos == content.end()) {
				break;  // Header not found
			}

			// Check if "Content-Type:" is at the beginning of a line or the start of the content
			if (pos == content.begin() || *(pos - 1) == '\n' || *(pos - 1) == '\r') {
				break;
			}

			// Move to the next character to continue the search
			++pos;
   		}

    	if (pos != content.end()) 
		{
			// Find the end of the line or content after the header
			std::string::iterator end = std::find_if(pos, content.end(), std::bind2nd(std::equal_to<int>(), '\n'));

			// Erase the header and the following line break
			content.erase(pos, end);

			// Remove any leading whitespace after erasing the header
			content.erase(std::find_if(content.begin(), content.end(), std::not1(std::ptr_fun<int, int>(std::isspace))), content.begin());
    	}
	}
	return content;
} */

static bool isNewline(char c) 
{
    return c == '\n';
}

void removeHeaderLine(std::string& content) 
{	
	std::string header = "Content-Type: ";

    std::string::iterator pos = std::search
	(
        content.begin(), content.end(),
        header.begin(), header.end(),
        caseInsensitiveCompare
    );

    if (pos != content.end()) {
        // Find the end of the line containing the header
         std::string::iterator endPos = std::find_if(pos, content.end(), isNewline);
        
        // Erase the line
        content.erase(pos - content.begin(), endPos - pos);
    }
}