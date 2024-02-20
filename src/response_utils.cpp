#include<webserv.hpp>

bool check_method(std::string method, const Locations *loc)
{	
	int idx;

	if (method == "GET")
		idx = 0;
	else if (method == "POST")
		idx = 1;
	else if (method == "DELETE")
		idx = 2;
	else
		return (true);
	if (loc)
	{
		if (loc->getMethods()[idx] == 0)
			return (false);
	}
	return (true);
}

std::string removeDoubleSlashes(const std::string& input) 
{
    std::string result;
    result.reserve(input.length());

    bool previousSlash = false;

    for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
        char c = *it;

        if (c == '/' && previousSlash) 
		{
            continue;
        }
        result.push_back(c);
        previousSlash = (c == '/');
    }
    return result;
}

std::string get_path(Request &request, const Server *serv, const Locations *loc)
{	
	std::string path;

	if (loc)
	{
		if (loc->getRoot().length() > 0)
		{
			cout << "location root is " << loc->getRoot() << endl;
			cout << "request target is " << request.request_line.target << endl;
			path = loc->getRoot() + request.request_line.target;
			return (removeDoubleSlashes(path));
		}
	}
	cout << "location has no root directive " << endl;
	if (serv->getRoot().length() > 0)
	{
		cout << "server root is " << serv->getRoot() << endl;
		path = serv->getRoot() + request.request_line.target;
		return (removeDoubleSlashes(path));
	}
	else
	{
		cout << "server has no root directive " << endl;
		cout << "request target is " << request.request_line.target << endl;
		return ("none"); // vacio, sin root directives no hay camino al filesystem del server
					// y solo devolveremos una pagina de cortesia si se accede al mismo '/', como nginx
	}
}

std::string readFileContents(const std::string& filename) 
{
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }

    std::ostringstream content;
    content << file.rdbuf();
    file.close();
	
    return content.str();
}

std::string getLengthAsString(std::string &content)
{
	std::stringstream ss;
    ss << content.length();
    std::string lengthAsString = ss.str();
	
	return (lengthAsString);
}

bool	checkGood(std::string &path)
{
	struct stat fileInfo;
	//cout << "entra en checkGood" << endl;
    return stat(path.c_str(), &fileInfo) == 0;
}

std::string checkFileOrDir(std::string &path)
{
	struct stat fileInfo;

	stat(path.c_str(), &fileInfo);

	if(fileInfo.st_mode & S_IFDIR )
    {
		cout << "es dir" << endl;
        return ("dir");
    }
    else if(fileInfo.st_mode & S_IFREG )
    {	
		cout << "es file" << endl;
        return ("file");
    }
	return ("");
}