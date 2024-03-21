#include<webserv.hpp>

bool check_method(std::string method, const Locations *loc, const Server *serv)
{	
	(void) serv;
	int idx;

	if (method == "GET")
		idx = 0;
	else if (method == "POST")
		idx = 1;
	else if (method == "DELETE")
		idx = 2;
	else if (method == "PUT")
		idx = 3;
	else
		return (true);
	if (loc)
	{
		if (loc->getMethods()[idx] == 0)
			return (false);
		return (true);
	}
	/* if (serv->getMethods()[idx] == 0)
		return (false); */
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

std::string getPath(Request &request, const Server *serv, const Locations *loc)
{	
	std::string path;

	if (loc)
	{
		if (loc->getRoot().length() > 0)
		{
			//cout << "location root is " << loc->getRoot() << endl;
			//cout << "request target is " << request.request_line.target << endl;
			path = loc->getRoot() + request.request_line.target;
			path = removeDoubleSlashes(path);
			return (path.substr(0, path.find('?')));
		}
		//cout << "location has no root directive " << endl;
	}
/* 	else
		cout << "no location was selected" << endl; */
	if (serv->getRoot().length() > 0)
	{
		//cout << "server root is " << serv->getRoot() << endl;
		path = serv->getRoot() + request.request_line.target;
		path = removeDoubleSlashes(path);
		return (path.substr(0, path.find('?')));
	}
	else
	{
		//cout << "server has no root directive " << endl;
		//cout << "request target is " << request.request_line.target << endl;
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

bool	checkGood(const std::string &path)
{
	struct stat fileInfo;
	//cout << "entra en checkGood" << endl;
    return stat(path.c_str(), &fileInfo) == 0;
}

std::string checkFileOrDir(const std::string &path)
{
	struct stat fileInfo;

	stat(path.c_str(), &fileInfo);

	if(fileInfo.st_mode & S_IFDIR )
    {
		//cout << "es dir" << endl;
        return ("dir");
    }
    else if(fileInfo.st_mode & S_IFREG )
    {	
		//cout << "es file" << endl;
        return ("file");
    }
	return ("");
}

bool findIndexHtml(std::string &path) 
{
    DIR* dir = opendir(path.c_str());

    if (dir) 
	{
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) 
		{
            if (entry->d_type == DT_REG && std::string(entry->d_name) == "index.html") 
			{
				closedir(dir);
                return (true);
            }
        }
        closedir(dir);
    }
    return (false);
}

std::string formatFileSize(off_t size) 
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;

    while (size >= 1024 && unitIndex < 4) 
	{
        size /= 1024;
        ++unitIndex;
    }

    std::ostringstream formattedSize;
    formattedSize << size << " " << units[unitIndex];
    return formattedSize.str();
}

std::string generateDirectoryListing(const std::string& path) 
{
    DIR* dir = opendir(path.c_str());

    if (dir) 
	{
        std::ostringstream html;
        html << "<html><head><title>Index of " << path << "</title></head><body>\n";
        html << "<h1>Index of " << path << "</h1><hr><pre>";

		if (path != "/") 
		{
            std::string parentPath = path.substr(0, path.find_last_of('/'));
            html << "<a href=\"../\">../</a>";
            html << std::string(40, ' '); // Padding
            html << std::string(20, ' '); // Padding
            html << "\n";
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            const char* filename = entry->d_name;

            // Skip the current and parent directory entries
            if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
                continue;

            // Get file information to determine modification time and size
            struct stat fileStat;
            std::string filePath = path + "/" + filename;
            stat(filePath.c_str(), &fileStat);

            char dateStr[64];
            strftime(dateStr, sizeof(dateStr), "%d-%b-%Y %H:%M", localtime(&fileStat.st_mtime));

            html << "<a href=\"" << filename << "\">" << filename << "</a>";
            html << std::string(40 - strlen(filename), ' '); // Padding
            html << std::right << std::setw(20) << dateStr;
            html << std::right << std::setw(20) << formatFileSize(fileStat.st_size) << "\n";
        }
        html << "</pre><hr></body></html>";
        closedir(dir);
        return html.str();
    }
	else 
	{
        cerr << "Error opening directory: " << strerror(errno) << endl;
        return "Error 500";
    }
}

bool checkTrailingSlash(std::string &path)
{
	if (path[path.length() - 1] == '/')
		return true;
   	else
		return false;
}

std::string checkReturn(const Locations *loc)
{	
	
	if (loc)
	{
		//cout << "entra en checkReturn: " << loc->getRedirection()  << endl;
		if (loc->getRedirection() != "")
			return (loc->getRedirection());
	}
	return ("");
}

bool checkDefaultPath()
{
	const char *env_path = std::getenv("DEFAULT_DIR");
	if (env_path)
		return (true);
	else
		return (false);
}

std::string getDefaultFile(const std::string &file)
{
	std::string default_path;

	default_path = "../default" + file;
	
	return (default_path);
}

void	makeDefault(int code, Response &response, const std::string &file, const Server *serv)
{
	/*comprobariamos si serv tiene un error page establecido en conf file*/
	std::string	content = "";
	std::map<int, std::string>::const_iterator it = serv->getErrorPage().find(code);
	if (it != serv->getErrorPage().end())
	{
		std::string path = serv->getRoot() + it->second;
		if (checkGood(path))
			content = readFileContents(path);
	}
	else
		 content = readFileContents("default" + file);
	response.setHeader("Content-Length: " + getLengthAsString(content));
	response.setHeader("Content-Type: text/html");
	response.setBody(content);
}


std::string findIndex(std::string &path, const Server *serv, const Locations *loc)
{	
	std::string index_file = "";

	if (loc && loc->getIndex().size() > 0)
	{
		std::vector<std::string>indexVector = loc->getIndex();
		for (std::vector<std::string>::iterator it = indexVector.begin(); it != indexVector.end(); it++)
		{
			if (checkGood(path + *it) && checkFileOrDir(path + *it) == "file")
			{
				index_file = path + *it;
				return (index_file);
			}
		}
	}
	std::vector<std::string>indexVector = serv->getVIndex();
	for (std::vector<std::string>::const_iterator it = indexVector.begin(); it != indexVector.end(); it++)
	{	
		cout << path + *it << endl;
		if (checkGood(path + *it) && checkFileOrDir(path + *it) == "file")
		{
			index_file = path + *it;
			return (index_file);
		}
	}
	return (index_file);
}

bool checkCgi(std::string &path, const Locations *loc) {
    if (loc) {
        const std::map<std::string, std::string>& cgiMap = loc->getCGI();
        if (!cgiMap.empty()) {
            std::map<std::string, std::string>::const_iterator it = cgiMap.find(path.substr(path.length() - 3, 3));
            if (it != cgiMap.end()) {
                return true;
            }
        }
    }
    return false;
}

