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
		cout << "entra en checkReturn: " << loc->getRedirection()  << endl;
		if (loc->getRedirection() != "")
			return (loc->getRedirection());
	}
	return ("");
}


/* std::string findIndex(std::string &path, Server *serv, Locations *loc)
{
	if (loc)
	{
		for (std::vector<class Index>iterator it = loc->getVIndex().begin(); it != loc->getVIndex().end(); it++)
		{

		}
	}
} */