#include<webserv.hpp>
#include <poll.h>

ConfFile::ConfFile()
{
	
}

ConfFile::ConfFile(std::string _file)
{
	file = _file;
}

ConfFile::~ConfFile()
{
}

int ConfFile::countServers(std::string content)
{
	int i = 0;
	size_t pos = content.find("server ");
	if (pos == std::string::npos)
		return (i);
	while (pos != std::string::npos)
	{
		i++;
		pos = content.find("server ", pos + 1);
	}
	return (i);
}

void ConfFile::checkLine(std::string line)
{
	size_t semicolon;
	size_t curly;

	semicolon = line.find(";");
	curly = line.find("{");
	if (line[0] == '}' && (line[1] == '\0' || line[1] == '\n'))
		return ;
	else if (line[0] == '}' && line[1] != '\0')
		throw std::runtime_error(line + " => invalid line");
	if (semicolon == std::string::npos && curly == std::string::npos)
	{
		if (line[0] != '\n' && line[0] != '\0')
			throw std::runtime_error(line + " => semicolon or curly brace missing.");
	}
	if (semicolon != std::string::npos && curly != std::string::npos)
		throw std::runtime_error(line + " => invalid line");
	else
	{
		if (semicolon != std::string::npos)
		{
			if (line[semicolon + 1] != '\0')
				throw std::runtime_error(line + " => invalid line. Semicolon must be end of line");
		}
		if (curly != std::string::npos)
		{
			if (line[curly + 1] != '\0' && line[curly + 1] != '\n')
				throw std::runtime_error(line + " => invalid line. Curly brace must be end of line");
		}
	}
}

void	ConfFile::checkInfo(std::string line)
{
	size_t pos = line.find(" ");
	
	if (pos != std::string::npos)
		throw std::runtime_error(line + " => invalid line. This variable only needs one parameter.");
}

void ConfFile::trimSpaces(std::string& str)
{
	str.erase(0, str.find_first_not_of(" \t"));

	str.erase(str.find_last_not_of(" \t") + 1);
}
	
void ConfFile::copyInfo(Cluster &cluster)
{
	cluster.getSocketVector() = this->getSocketVector();
	cluster.getServerVector() = this->getServerVector();
	cluster.getPollVector() = this->getPollVector();
}

void	ConfFile::parse_config(Cluster &cluster, char *file)
{
	std::ifstream in;
	std::string line;
	std::string content;
	std::string result;
	int servers;

	in.open(file, std::ios::in);
 	if (!in.is_open())
		throw std::runtime_error("File could not be found or opened.");
	while (std::getline(in, line))
		content += line + "\n";
	servers = countServers(content);
	if (servers == 0)
		throw std::runtime_error("at least one server needed.");
	std::istringstream iss(content);
	int i = 0;
	while (i < servers)
	{
		while (std::getline(iss, line, '\n'))
		{
			trimSpaces(line);
			if (line.find("server ") == 0)
			{
				checkLine(line);
				parse_element(content, i);
				i++;
			}
		}
	}
	copyInfo(cluster);
}

std::string ConfFile::checkPath(std::string relativepath)
{
	std::string path = relativepath;
	int flag = 0;

	if (path.length() == 1 && path[0] == '.')
		return (static_cast<std::string>(getenv("PWD")));
	if (path.substr(0, 2) == "./")
	{
		path = static_cast<std::string>(getenv("PWD")) + path.substr(1, path.length());
	}
	else
	{
		std::string pwd = static_cast<std::string>(getenv("PWD"));
		while (relativepath.substr(0, 3) == "../")
		{
			relativepath = relativepath.substr(3, relativepath.length());
			flag++;
		}
		for (int i = pwd.length(); i >= 0; --i)
		{
			if (pwd[i] == '/')
			{
				path = (pwd.substr(0, i)) + "/" + relativepath;
				flag--;
				if (flag == 0)
					break ;
  			}
		}
	}
	return (path);
}

std::string ConfFile::findInfo(std::string line, std::string tofind)
{
	int tofindpos = line.find(tofind);
	int semicolonpos = line.find(";");
	std::string result;
	std::size_t firstNS;
	std::size_t lastNS;

	if (tofindpos < 0 || semicolonpos < 0)
		throw std::runtime_error("invalid configuration file. Semicolon missing");
	result = line.substr(tofindpos + tofind.length(), semicolonpos);
	firstNS = result.find_first_not_of(" \t\n\r");
    lastNS = result.find_last_not_of(" \t\n\r;");
	result = result.substr(firstNS, lastNS - firstNS + 1);
	return (result);
}

void	ConfFile::findIp(Server& Serv, std::string newserv)
{
	std::size_t firstNonSpace;
	std::size_t lastNonSpace;
	std::string line;
	std::string res;
	std::string host;
	std::istringstream iss(newserv);
	std::size_t hasip;
	size_t pos;

	std::getline(iss, line, '\n');
	pos = line.find(";");
	if (pos == std::string::npos)
		throw std::runtime_error("invalid configuration file. Semicolon missing");
	hasip = line.find(":");
	pos = line.find("listen ");
	if (pos == std::string::npos)
		throw std::runtime_error("invalid configuration file. Listen directive missing");
	if (hasip != std::string::npos)
	{
		res = line.substr(pos + 6, hasip - (pos + 6));
		firstNonSpace = res.find_first_not_of(" \t\n\r");
		lastNonSpace = res.find_last_not_of(" \t\n\r;");
		res = res.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
		host = res;
	}
	if (hasip == std::string::npos)
	{	
		host = "0.0.0.0";
		res = line.substr(pos + 6, line.find(";"));
	}
	else
		res = line.substr(hasip + 1, line.find(";"));
	firstNonSpace = res.find_first_not_of(" \t\n\r");
	lastNonSpace = res.find_last_not_of(" \t\n\r;");
	res = res.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
	Serv.setHostPort(host, res);
}

void	ConfFile::parse_location(std::string line, Location& loc)
{
	std::size_t pos;
	std::size_t fpos;
	std::string res;

	trimSpaces(line);
	if (line[0] == '#')
		return ;
	checkLine(line);
	if (line.find(" /") != std::string::npos && loc.getLocation().empty())
	{
		pos = line.find("/");
		fpos = line.find(" {");
		if (pos == std::string::npos || fpos == std::string::npos)
			throw std::runtime_error("invalid configuration file. Semicolon or curly brace missing");
		res = line.substr(pos, fpos - pos);
		trimSpaces(res);
		loc.setLocation(res);
	}
	else
	{
		fpos = line.find(";");
		if (fpos == std::string::npos)
			throw std::logic_error("invalid configuration file. Semicolon or curly brace missing");
		if (line.find("autoindex ") != std::string::npos)
		{
			pos = line.find("autoindex ");
			res = line.substr(pos + 10, fpos - pos);
			if (res == "on;" || res == "on")
				loc.setAutoindex(1);
			if (res == "off" || res == "off;")
				loc.setAutoindex(0);
		}
		else if (line.find("index ") != std::string::npos)
		{
			std::string index = findInfo(line, "index ");
			loc.setVIndex(splitString(index));
		}
		else if (line.find("allow_methods ") != std::string::npos)
		{
			int methods[4] = {0, 0, 0, 0};
			pos = line.find("allow_methods ");
			res = (line.substr(pos + 14, fpos - pos));
			if (res.find("GET") != std::string::npos)
				methods[GET] = 1;
			if (res.find("POST") != std::string::npos)
				methods[POST] = 1;
			if (res.find("DELETE") != std::string::npos)
				methods[DELETE] = 1;
			if (res.find("PUT") != std::string::npos)
				methods[PUT] = 1;
			loc.setMethods(methods);
		}
		else if (line.find("return ") != std::string::npos)
		{
			pos = line.find("return ");
			res = line.substr(pos + 7, fpos - pos);
			loc.setRedirection(res.erase(res.size() - 1));
		}
		else if (line.find("root ") != std::string::npos)
		{
			pos = line.find("root ");
			res = line.substr(pos + 5, fpos - pos - 4);
			res = checkPath(res);
			loc.setRoot(res);
		}
		else if (line.find("upload_store") != std::string::npos)
		{
			pos = line.find("upload_store ");
			res = line.substr(pos + 13, fpos - pos - 13);
			res = checkPath(res);
			if (access(res.c_str(), F_OK) == -1 || access(res.c_str(), W_OK) == -1)
				throw std::runtime_error(res + " => this folder does not exist or you don't have permission");
			loc.setUploadStore(res);
		}
		else if (line.find("cgi") != std::string::npos)
		{
			pos = line.find("cgi ");
			res = line.substr(pos + 4, fpos - pos - 4);
			pos = res.find(" ");
			fpos = res.find(";");
			std::string execute = res.substr(0, pos);
			trimSpaces(execute);
			std::string path = res.substr(pos, fpos);
			trimSpaces(path);
			loc.setCGI(execute, path);
		}
		else
			throw std::logic_error(line + " => invalid line or curly brace missing");
	}
}

std::vector<std::string> ConfFile::splitString(std::string& input)
{
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;

    while (iss >> token)
        tokens.push_back(token);
    return tokens;
}

int		ConfFile::parse_element(std::string &content, int i)
{
	size_t servpos = content.find("server ");
	static int id = 1;
	std::string newserv;
	std::string line;
	Server Serv;

	Serv.id = id;
	id++;
	if (servpos == std::string::npos)
		throw std::runtime_error("server is missing or the file structure is incorrect.");
	while (i > 0)
	{
		servpos = content.find("server ", servpos + 1);
		i--;
	}
	newserv = content.substr(servpos, content.length());
	std::istringstream iss(newserv);
	std::getline(iss, line, '\n');
	while (std::getline(iss, line, '\n'))
	{
		trimSpaces(line);
		checkLine(line);
		if (line[0] != '#' && line[0] != '\n')
		{
			if (line.find("server ") == 0)
				break ;
			else if (line.find("listen ") != std::string::npos)
				findIp(Serv, &line[line.find("listen ")]); 
			else if (line.find("server_name ") != std::string::npos)
				Serv.addVServerName(findInfo(line, "server_name"));
			else if (line.find("error_page ") != std::string::npos)
				Serv.setErrorPage(findInfo(line, "error_page "));
			else if (line.find("root ") != std::string::npos)
			{
				std::string root = checkPath(findInfo(line, "root "));
				Serv.setRoot(root);
			}
			else if (line.find("upload_store ") != std::string::npos)
				Serv.setUploadStore(findInfo(line, "upload_store"));
			else if (line.find("host ") != std::string::npos)
				continue ;
			else if (line.find("index ") != std::string::npos)
			{
				std::string index = findInfo(line, "index ");
				Serv.addVIndex(splitString(index));
			}
			else if (line.find("client_max_body_size ") != std::string::npos)
				Serv.setMaxBodySize(findInfo(line, "client_max_body_size "));
			else if (line.find("location ") == std::string::npos && line.find("}") == std::string::npos)
			{
				if (!line.empty())
					throw std::runtime_error("incorrect param in config file => " + line);
			}
			else if (line.find("location ") != std::string::npos)
			{
				Location loc;
				while (line.find("}") == std::string::npos)
				{
					parse_location(line, loc);
					std::getline(iss, line, '\n');
				}
				if (loc.getRoot().empty())
				{
					std::string res = loc.getLocation();
					res = res.erase(0, 1);
					loc.setAllUrl(Serv.getRoot() + res);
				}
				else
				{
					loc.setAllUrl(removeDoubleSlashes(loc.getRoot() + loc.getLocation()));
				}
				Serv.setLocation(loc);
			}
		}
	}
	if (!Serv.host_port.size())
		Serv.setHostPort("0.0.0.0", "8080"); 
	this->serv_vec.push_back(Serv);
	return (0);
}

void	ConfFile::print_servers()
{
	for (size_t i = 0; i < this->serv_vec.size(); i++)
	{
		std::cout << BGRED "Server " << i + 1 << ":" RESET << std::endl;
		this->serv_vec[i].printHostPort();
		this->serv_vec[i].printIpPort();
		this->serv_vec[i].printServer_Names();
		this->serv_vec[i].printRoot();
		this->serv_vec[i].printBodySize();
		this->serv_vec[i].printErrorPages();
		this->serv_vec[i].printindex();
		this->serv_vec[i].printUploadStore();
		this->serv_vec[i].printLocations();
	}
}

void ConfFile::print_sockets()
{
	for (unsigned int i = 0; i < sock_vec.size(); i++)
	{
		cout << "Socket " << i + 1 <<  " with fd " << sock_vec[i].getFd()  << ". Is listener? " << sock_vec[i].listener << endl
		<< "IP: " << sock_vec[i].getIp() << " Port: " << sock_vec[i].getPort() << endl;  
	}
}
